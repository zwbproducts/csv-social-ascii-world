#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <algorithm>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

using namespace std;

// ============================================================
// ASCII ART COLORS (Simple terminal colors)
// ============================================================
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

// ============================================================
// LOGGING SYSTEM
// ============================================================
class Logger {
public:
    static void log(const string& event) {
        ofstream logfile("game_log.txt", ios::app);
        time_t now = time(0);
        char* dt = ctime(&now);
        string ts(dt);
        ts.pop_back(); // remove newline
        logfile << "[" << ts << "] " << event << endl;
        logfile.close();
    }
    
    static void social(const string& who, const string& action) {
        string msg = "SOCIAL: " + who + " " + action;
        log(msg);
        cout << CYAN << "[SOCIAL] " << who << " " << action << RESET << endl;
    }
    
    static void movement(const string& who, const string& where) {
        string msg = "MOVE: " + who + " -> " + where;
        log(msg);
        cout << YELLOW << "[MOVE] " << who << " -> " << where << RESET << endl;
    }
    
    static void stat_update(const string& who, const string& stat, int val) {
        string msg = "STAT: " + who + " " + stat + "=" + to_string(val);
        log(msg);
        cout << GREEN << "[STAT] " << who << " " << stat << "=" << val << RESET << endl;
    }
};

// ============================================================
// CSV STAT SYSTEM
// ============================================================
struct StatBonus {
    string name;
    int health_bonus;
    int speed_bonus;
    int strength_bonus;
    int charm_bonus;
    int intelligence_bonus;
};

class CSVManager {
public:
    static StatBonus parseCSV(const string& filename) {
        StatBonus bonus = {"empty", 0, 0, 0, 0, 0};
        ifstream file(filename);
        if (!file.is_open()) {
            cout << RED << "Cannot open CSV: " << filename << RESET << endl;
            return bonus;
        }
        
        string line;
        bool header = true;
        while (getline(file, line)) {
            if (header) { header = false; continue; }
            stringstream ss(line);
            string token;
            
            getline(ss, bonus.name, ',');
            getline(ss, token, ','); bonus.health_bonus = stoi(token);
            getline(ss, token, ','); bonus.speed_bonus = stoi(token);
            getline(ss, token, ','); bonus.strength_bonus = stoi(token);
            getline(ss, token, ','); bonus.charm_bonus = stoi(token);
            getline(ss, token, ','); bonus.intelligence_bonus = stoi(token);
        }
        file.close();
        return bonus;
    }
    
    static void createSampleCSVs() {
        // Create apple.csv
        ofstream apple("apple.csv");
        apple << "name,health,speed,strength,charm,intelligence\n";
        apple << "Apple,10,2,0,1,0\n";
        apple.close();
        
        // Create energy_drink.csv
        ofstream ed("energy_drink.csv");
        ed << "name,health,speed,strength,charm,intelligence\n";
        ed << "EnergyDrink,5,15,3,0,2\n";
        ed.close();
        
        // Create steak.csv
        ofstream steak("steak.csv");
        steak << "name,health,speed,strength,charm,intelligence\n";
        steak << "Steak,25,0,10,0,0\n";
        steak.close();
        
        // Create book.csv
        ofstream book("book.csv");
        book << "name,health,speed,strength,charm,intelligence\n";
        book << "Book,0,0,0,2,15\n";
        book.close();
    }
};

// ============================================================
// CHARACTER BASE CLASS
// ============================================================
class Character {
public:
    string name;
    char symbol;
    int x, y;
    int health, max_health;
    int speed;
    int strength;
    int charm;
    int intelligence;
    string color;
    
    Character(string n, char sym, string col, int startX, int startY) 
        : name(n), symbol(sym), color(col), x(startX), y(startY) {
        health = max_health = 100;
        speed = 10;
        strength = 10;
        charm = 10;
        intelligence = 10;
    }
    
    virtual void display() {
        cout << color << symbol << RESET;
    }
    
    void applyBonus(const StatBonus& bonus) {
        if (bonus.name == "empty") return;
        health += bonus.health_bonus;
        speed += bonus.speed_bonus;
        strength += bonus.strength_bonus;
        charm += bonus.charm_bonus;
        intelligence += bonus.intelligence_bonus;
        if (health > max_health + 50) health = max_health + 50;
        
        Logger::stat_update(name, "health", health);
        Logger::stat_update(name, "speed", speed);
        Logger::stat_update(name, "strength", strength);
        Logger::stat_update(name, "charm", charm);
        Logger::stat_update(name, "intelligence", intelligence);
    }
    
    void eatCSV(const string& filename) {
        cout << BOLD << name << " eats " << filename << "!" << RESET << endl;
        StatBonus bonus = CSVManager::parseCSV(filename);
        if (bonus.name != "empty") {
            applyBonus(bonus);
            cout << GREEN << "  >> Stats boosted!" << RESET << endl;
        } else {
            cout << RED << "  >> Nothing happened..." << RESET << endl;
        }
    }
    
    void chat(Character& other) {
        vector<string> greetings = {
            "says hi to", "greets", "waves at", "smiles at",
            "talks to", "chats with", "converses with"
        };
        string action = greetings[rand() % greetings.size()];
        string msg = name + " " + action + " " + other.name;
        Logger::social(msg, "");
    }
    
    void move(int dx, int dy, int maxW, int maxH) {
        int nx = x + dx, ny = y + dy;
        if (nx >= 1 && nx < maxW-1 && ny >= 1 && ny < maxH-1) {
            x = nx; y = ny;
            Logger::movement(name, "(" + to_string(x) + "," + to_string(y) + ")");
        }
    }
    
    void randomMove(int maxW, int maxH) {
        int dir = rand() % 4;
        switch(dir) {
            case 0: move(-1, 0, maxW, maxH); break;
            case 1: move(1, 0, maxW, maxH); break;
            case 2: move(0, -1, maxW, maxH); break;
            case 3: move(0, 1, maxW, maxH); break;
        }
    }
};

// ============================================================
// WORLD MAP
// ============================================================
const int MAP_W = 40;
const int MAP_H = 20;

class World {
public:
    char tiles[MAP_H][MAP_W];
    vector<Character*> characters;
    vector<string> csv_items;
    map<pair<int,int>, string> csv_positions;
    
    World() {
        // Initialize tiles
        for (int y = 0; y < MAP_H; y++) {
            for (int x = 0; x < MAP_W; x++) {
                if (x == 0 || x == MAP_W-1 || y == 0 || y == MAP_H-1)
                    tiles[y][x] = '#';
                else
                    tiles[y][x] = '.';
            }
        }
        
        // Add some decorations
        for (int i = 0; i < 15; i++) {
            int rx = 1 + rand() % (MAP_W-2);
            int ry = 1 + rand() % (MAP_H-2);
            tiles[ry][rx] = '+';
        }
        
        // CSV items on ground
        csv_items = {"apple.csv", "steak.csv", "energy_drink.csv", "book.csv"};
        for (int i = 0; i < 8; i++) {
            int rx = 1 + rand() % (MAP_W-2);
            int ry = 1 + rand() % (MAP_H-2);
            string item = csv_items[rand() % csv_items.size()];
            csv_positions[{rx, ry}] = item;
        }
    }
    
    void addCharacter(Character* c) {
        characters.push_back(c);
    }
    
    void display() {
        system("clear");
        cout << BOLD << CYAN;
        cout << "========================================\n";
        cout << "  ASCII ART CSV SOCIAL SIMULATOR v1.0   \n";
        cout << "========================================" << RESET << endl;
        
        char displayMap[MAP_H][MAP_W];
        for (int y = 0; y < MAP_H; y++)
            for (int x = 0; x < MAP_W; x++)
                displayMap[y][x] = tiles[y][x];
        
        // Place CSV items
        for (auto& p : csv_positions) {
            displayMap[p.first.second][p.first.first] = '$';
        }
        
        // Place characters
        for (auto c : characters) {
            if (c->health > 0)
                displayMap[c->y][c->x] = c->symbol;
        }
        
        // Draw
        for (int y = 0; y < MAP_H; y++) {
            for (int x = 0; x < MAP_W; x++) {
                char tile = displayMap[y][x];
                if (tile == '#') cout << BOLD << WHITE << tile << RESET;
                else if (tile == '+') cout << GREEN << tile << RESET;
                else if (tile == '$') cout << YELLOW << tile << RESET;
                else if (tile == '@') cout << BOLD << BLUE << tile << RESET;
                else if (tile == 'P') cout << BOLD << MAGENTA << tile << RESET;
                else if (tile == 'N') cout << RED << tile << RESET;
                else cout << tile;
            }
            cout << endl;
        }
        
        // Status
        cout << BOLD << CYAN << "\n--- STATUS ---" << RESET << endl;
        for (auto c : characters) {
            string col = (c->health > 50) ? GREEN : RED;
            cout << c->color << c->symbol << " " << c->name 
                 << " HP:" << col << c->health 
                 << " S:" << c->speed 
                 << " STR:" << c->strength 
                 << " CHR:" << c->charm 
                 << " INT:" << c->intelligence << RESET << endl;
        }
        cout << CYAN << "$=CSV  #=Wall  +=Bush  @=You  P=Player  N=NPC" << RESET << endl;
        cout << BOLD << CYAN << "Controls: WASD=move  E=eat CSV  Q=quit" << RESET << endl;
    }
    
    void updateNPCs() {
        for (auto c : characters) {
            if (c->symbol == 'N' || c->symbol == 'P') {
                if (c->symbol == 'N') {
                    // NPCs move randomly and chat
                    c->randomMove(MAP_W, MAP_H);
                    if (rand() % 5 == 0) {
                        // NPC chats with random character
                        vector<Character*> others;
                        for (auto o : characters) {
                            if (o != c && abs(o->x - c->x) <= 3 && abs(o->y - c->y) <= 3)
                                others.push_back(o);
                        }
                        if (!others.empty()) {
                            c->chat(*others[rand() % others.size()]);
                        }
                    }
                    // NPCs eat CSV if on one
                    if (csv_positions.count({c->x, c->y})) {
                        c->eatCSV(csv_positions[{c->x, c->y}]);
                        csv_positions.erase({c->x, c->y});
                    }
                }
            }
        }
    }
    
    bool checkCSV(int x, int y) {
        return csv_positions.count({x, y});
    }
    
    void removeCSV(int x, int y) {
        csv_positions.erase({x, y});
    }
};

// ============================================================
// MAIN GAME
// ============================================================
int main() {
    srand(time(0));
    
    // Clear old log
    ofstream clearlog("game_log.txt");
    clearlog << "=== ASCII ART CSV SOCIAL SIMULATOR LOG ===\n";
    clearlog.close();
    
    // Create sample CSVs
    CSVManager::createSampleCSVs();
    
    // Create world
    World world;
    
    // Create player
    Character* player = new Character("Hero", '@', BOLD BLUE, 5, 5);
    world.addCharacter(player);
    
    // Create NPCs
    world.addCharacter(new Character("Alice", 'P', MAGENTA, 15, 8));
    world.addCharacter(new Character("Bob", 'P', MAGENTA, 25, 12));
    world.addCharacter(new Character("Goblin", 'N', RED, 30, 5));
    world.addCharacter(new Character("Elf", 'N', GREEN, 10, 15));
    world.addCharacter(new Character("Orc", 'N', RED, 20, 18));
    
    Logger::log("Game started!");
    
    int turn = 0;
    while (true) {
        world.display();
        
        cout << BOLD << WHITE << "\nTurn " << turn << " | Action: ";
        
        // Non-blocking input check
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        
        char ch = 0;
        if (read(STDIN_FILENO, &ch, 1) == 1) {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            
            if (ch == 'q' || ch == 'Q') {
                Logger::log("Game ended by player");
                break;
            }
            
            switch(ch) {
                case 'w': case 'W': player->move(0, -1, MAP_W, MAP_H); break;
                case 's': case 'S': player->move(0, 1, MAP_W, MAP_H); break;
                case 'a': case 'A': player->move(-1, 0, MAP_W, MAP_H); break;
                case 'd': case 'D': player->move(1, 0, MAP_W, MAP_H); break;
                case 'e': case 'E':
                    if (world.checkCSV(player->x, player->y)) {
                        player->eatCSV(world.csv_positions[{player->x, player->y}]);
                        world.removeCSV(player->x, player->y);
                    } else {
                        cout << RED << "No CSV here!" << RESET << endl;
                        usleep(500000);
                    }
                    break;
            }
        } else {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        }
        
        // Update NPCs every few turns
        if (turn % 2 == 0) {
            world.updateNPCs();
        }
        
        // NPCs chat with nearby players
        for (auto c : world.characters) {
            if (c->symbol == 'N' || c->symbol == 'P') {
                if (c != player && abs(c->x - player->x) <= 2 && abs(c->y - player->y) <= 2) {
                    if (rand() % 3 == 0) {
                        c->chat(*player);
                    }
                }
            }
        }
        
        turn++;
        usleep(100000); // 100ms
    }
    
    cout << BOLD << CYAN << "\n=== GAME OVER ===" << RESET << endl;
    cout << "Check game_log.txt for the full interaction log!" << endl;
    
    // Cleanup
    for (auto c : world.characters) delete c;
    
    return 0;
}
