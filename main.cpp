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
#include <sys/select.h>
#include <signal.h>
#include <errno.h>

using namespace std;

// ============================================================
// ASCII ART COLORS
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

static bool useColor = true;
static string C(const string& code) { return useColor ? code : string(); }

// ============================================================
// TERMINAL (RAII raw mode)
// ============================================================
static struct termios savedTermios;
static bool termSaved = false;

static void restoreTerminal() {
    if (termSaved) tcsetattr(STDIN_FILENO, TCSANOW, &savedTermios);
}

static void handleSignal(int) {
    restoreTerminal();
    _exit(0);
}

struct RawMode {
    RawMode() {
        if (tcgetattr(STDIN_FILENO, &savedTermios) == 0) {
            termSaved = true;
            struct termios t = savedTermios;
            t.c_lflag &= ~(ICANON | ECHO);
            t.c_cc[VMIN] = 0;
            t.c_cc[VTIME] = 0;
            tcsetattr(STDIN_FILENO, TCSANOW, &t);
            atexit(restoreTerminal);
            signal(SIGINT, handleSignal);
            signal(SIGTERM, handleSignal);
        }
    }
    ~RawMode() { restoreTerminal(); }
};

// ============================================================
// LOGGING SYSTEM
// ============================================================
class Logger {
public:
    static void log(const string& event) {
        ofstream logfile("game_log.txt", ios::app);
        time_t now = time(0);
        char* dt = ctime(&now);
        string ts(dt ? dt : "");
        if (!ts.empty() && ts.back() == '\n') ts.pop_back();
        logfile << "[" << ts << "] " << event << endl;
        logfile.close();
    }
    static void social(const string& who, const string& action) {
        string msg = "SOCIAL: " + who + " " + action;
        log(msg);
        cout << C(CYAN) << "[SOCIAL] " << who << " " << action << C(RESET) << endl;
    }
    static void movement(const string& who, const string& where) {
        string msg = "MOVE: " + who + " -> " + where;
        log(msg);
        cout << C(YELLOW) << "[MOVE] " << who << " -> " << where << C(RESET) << endl;
    }
    static void stat_update(const string& who, const string& stat, int val) {
        string msg = "STAT: " + who + " " + stat + "=" + to_string(val);
        log(msg);
        cout << C(GREEN) << "[STAT] " << who << " " << stat << "=" << val << C(RESET) << endl;
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

static int toInt(const string& s) {
    try { return stoi(s); } catch (...) { return 0; }
}

class CSVManager {
public:
    static StatBonus parseCSV(const string& filename) {
        StatBonus bonus = {"empty", 0, 0, 0, 0, 0};
        ifstream file(filename);
        if (!file.is_open()) {
            cout << C(RED) << "Cannot open CSV: " << filename << C(RESET) << endl;
            return bonus;
        }
        string line;
        bool header = true;
        while (getline(file, line)) {
            if (line.empty()) continue;
            if (header) { header = false; continue; }
            stringstream ss(line);
            string token;
            getline(ss, bonus.name, ',');
            getline(ss, token, ','); bonus.health_bonus = toInt(token);
            getline(ss, token, ','); bonus.speed_bonus = toInt(token);
            getline(ss, token, ','); bonus.strength_bonus = toInt(token);
            getline(ss, token, ','); bonus.charm_bonus = toInt(token);
            getline(ss, token, ','); bonus.intelligence_bonus = toInt(token);
            if (!bonus.name.empty()) break;
        }
        file.close();
        if (bonus.name.empty()) bonus.name = "empty";
        return bonus;
    }

    static void createSampleCSVs() {
        struct F { string name; string content; };
        vector<F> files = {
            {"apple.csv", "name,health,speed,strength,charm,intelligence\nApple,10,2,0,1,0\n"},
            {"steak.csv", "name,health,speed,strength,charm,intelligence\nSteak,25,0,10,0,0\n"},
            {"energy_drink.csv", "name,health,speed,strength,charm,intelligence\nEnergyDrink,5,15,3,0,2\n"},
            {"book.csv", "name,health,speed,strength,charm,intelligence\nBook,0,0,0,2,15\n"},
        };
        for (auto& f : files) {
            ifstream check(f.name);
            if (check.is_open()) { check.close(); continue; }
            ofstream out(f.name);
            out << f.content;
            out.close();
        }
    }
};

// ============================================================
// CHARACTER
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
    int eaten = 0;

    Character(string n, char sym, string col, int startX, int startY)
        : name(n), symbol(sym), x(startX), y(startY), color(col) {
        health = max_health = 100;
        speed = 10;
        strength = 10;
        charm = 10;
        intelligence = 10;
    }

    virtual void display() {
        cout << color << symbol << RESET;
    }

    virtual ~Character() {}

    void applyBonus(const StatBonus& bonus) {
        if (bonus.name == "empty") return;
        health += bonus.health_bonus;
        speed += bonus.speed_bonus;
        strength += bonus.strength_bonus;
        charm += bonus.charm_bonus;
        intelligence += bonus.intelligence_bonus;
        max_health += bonus.health_bonus;
        if (health > max_health + 50) health = max_health + 50;
        if (health < 1) health = 1;

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
            eaten++;
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

    void move(int dx, int dy, int maxW, int maxH, const vector<Character*>& others, bool checkOcc = true) {
        int nx = x + dx, ny = y + dy;
        if (nx < 1 || nx >= maxW-1 || ny < 1 || ny >= maxH-1) return;
        if (checkOcc) {
            for (auto o : others) {
                if (o && o != this && o->health > 0 && o->x == nx && o->y == ny) return;
            }
        }
        x = nx; y = ny;
        Logger::movement(name, "(" + to_string(x) + "," + to_string(y) + ")");
    }

    void randomMove(int maxW, int maxH, const vector<Character*>& others) {
        int dir = rand() % 4;
        switch(dir) {
            case 0: move(-1, 0, maxW, maxH, others); break;
            case 1: move(1, 0, maxW, maxH, others); break;
            case 2: move(0, -1, maxW, maxH, others); break;
            case 3: move(0, 1, maxW, maxH, others); break;
        }
    }
};

// ============================================================
// WORLD
// ============================================================
const int MAP_W = 40;
const int MAP_H = 20;

class World {
public:
    char tiles[MAP_H][MAP_W];
    vector<Character*> characters;
    vector<string> csv_items;
    map<pair<int,int>, string> csv_positions;
    int turnsSinceSpawn = 0;

    World() {
        for (int y = 0; y < MAP_H; y++)
            for (int x = 0; x < MAP_W; x++)
                tiles[y][x] = (x == 0 || x == MAP_W-1 || y == 0 || y == MAP_H-1) ? '#' : '.';

        for (int i = 0; i < 15; i++) {
            int rx = 1 + rand() % (MAP_W-2);
            int ry = 1 + rand() % (MAP_H-2);
            tiles[ry][rx] = '+';
        }

        csv_items = {"apple.csv", "steak.csv", "energy_drink.csv", "book.csv"};
        for (int i = 0; i < 8; i++) {
            int rx = 1 + rand() % (MAP_W-2);
            int ry = 1 + rand() % (MAP_H-2);
            if (tiles[ry][rx] == '+') continue;
            csv_positions[{rx, ry}] = csv_items[rand() % csv_items.size()];
        }
    }

    void addCharacter(Character* c) { characters.push_back(c); }

    void display(bool showHelp) {
        system("clear");
        cout << BOLD << CYAN;
        cout << "========================================\n";
        cout << "  ASCII ART CSV SOCIAL SIMULATOR v1.1   \n";
        cout << "========================================" << RESET << endl;

        char displayMap[MAP_H][MAP_W];
        for (int y = 0; y < MAP_H; y++)
            for (int x = 0; x < MAP_W; x++)
                displayMap[y][x] = tiles[y][x];

        for (auto& p : csv_positions)
            displayMap[p.first.second][p.first.first] = '$';

        for (auto c : characters)
            if (c->health > 0)
                displayMap[c->y][c->x] = c->symbol;

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

        cout << BOLD << CYAN << "\n--- STATUS ---" << RESET << endl;
        for (auto c : characters) {
            string col = (c->health > 50) ? GREEN : RED;
            cout << c->color << c->symbol << " " << c->name
                 << " HP:" << col << c->health
                 << " S:" << c->speed
                 << " STR:" << c->strength
                 << " CHR:" << c->charm
                 << " INT:" << c->intelligence
                 << " Eaten:" << c->eaten << RESET << endl;
        }
        cout << CYAN << "$=CSV  #=Wall  +=Bush  @=You  P=Player  N=NPC" << RESET << endl;
        cout << BOLD << CYAN << "Controls: WASD=move  E=eat  H=help  P=pause  Q=quit" << RESET << endl;

        if (showHelp) {
            cout << BOLD << YELLOW << "\n--- HELP ---" << RESET << endl;
            cout << "Move with WASD. Stand on $ and press E to eat a CSV for stat boosts.\n";
            cout << "NPCs wander, chat, and eat CSVs on their own.\n";
            cout << "Goal: out-eat the NPCs. Paused: press P to toggle. Quit: Q.\n";
        }
    }

    void updateNPCs() {
        for (auto c : characters) {
            if (c == nullptr || c->health <= 0) continue;
            if (c->symbol != 'N' && c->symbol != 'P') continue;
            // Players (P) move slower
            if (c->symbol == 'P' && (rand() % 2 != 0)) continue;
            c->randomMove(MAP_W, MAP_H, characters);
            if (rand() % 5 == 0) {
                vector<Character*> others;
                for (auto o : characters) {
                    if (o != c && o->health > 0 && abs(o->x - c->x) <= 3 && abs(o->y - c->y) <= 3)
                        others.push_back(o);
                }
                if (!others.empty())
                    c->chat(*others[rand() % others.size()]);
            }
            if (csv_positions.count({c->x, c->y})) {
                c->eatCSV(csv_positions[{c->x, c->y}]);
                csv_positions.erase({c->x, c->y});
            }
        }
        // Respawn CSVs occasionally to keep demo alive
        turnsSinceSpawn++;
        if (turnsSinceSpawn >= 15 && csv_positions.size() < 4) {
            turnsSinceSpawn = 0;
            for (int tries = 0; tries < 5; tries++) {
                int rx = 1 + rand() % (MAP_W-2);
                int ry = 1 + rand() % (MAP_H-2);
                if (tiles[ry][rx] == '+' ) continue;
                if (csv_positions.count({rx, ry})) continue;
                bool occupied = false;
                for (auto c : characters) if (c->health>0 && c->x==rx && c->y==ry) { occupied=true; break; }
                if (occupied) continue;
                csv_positions[{rx, ry}] = csv_items[rand() % csv_items.size()];
                break;
            }
        }
    }

    bool checkCSV(int x, int y) { return csv_positions.count({x, y}); }
    void removeCSV(int x, int y) { csv_positions.erase({x, y}); }
};

// ============================================================
// MAIN
// ============================================================
int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        string a = argv[i];
        if (a == "--no-color") useColor = false;
        else if (a == "--help" || a == "-h") {
            cout << "Usage: ./ascii_game [--no-color]\n";
            cout << "Controls: WASD move, E eat CSV, H help, P pause, Q quit\n";
            return 0;
        }
    }

    srand((unsigned)time(0));

    ofstream clearlog("game_log.txt");
    clearlog << "=== ASCII ART CSV SOCIAL SIMULATOR LOG ===\n";
    clearlog.close();

    CSVManager::createSampleCSVs();

    World world;
    Character* player = new Character("Hero", '@', BOLD BLUE, 5, 5);
    world.addCharacter(player);
    world.addCharacter(new Character("Alice", 'P', MAGENTA, 15, 8));
    world.addCharacter(new Character("Bob", 'P', MAGENTA, 25, 12));
    world.addCharacter(new Character("Goblin", 'N', RED, 30, 5));
    world.addCharacter(new Character("Elf", 'N', GREEN, 10, 15));
    world.addCharacter(new Character("Orc", 'N', RED, 20, 18));

    Logger::log("Game started!");

    RawMode raw;
    bool paused = false;
    bool showHelp = false;
    int turn = 0;
    const int GOAL = 8;

    while (true) {
        world.display(showHelp);

        cout << BOLD << WHITE << "\nTurn " << turn
             << " | Player eaten: " << player->eaten << "/" << GOAL
             << " | Action: " << RESET << flush;

        // Wait for input up to 100ms (non-blocking-ish)
        char ch = 0;
        bool gotInput = false;
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tv = {0, 100000};
        int sel = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        if (sel > 0) {
            if (read(STDIN_FILENO, &ch, 1) == 1) gotInput = true;
        } else if (sel < 0 && errno != EINTR) {
            // Transient select error; keep running
        }

        if (gotInput) {
            if (ch == 'q' || ch == 'Q') {
                Logger::log("Game ended by player");
                break;
            }
            if (ch == 'h' || ch == 'H') { showHelp = !showHelp; }
            else if (ch == 'p' || ch == 'P') { paused = !paused; }
            else if (!paused) {
                switch(ch) {
                    case 'w': case 'W': player->move(0, -1, MAP_W, MAP_H, world.characters); break;
                    case 's': case 'S': player->move(0, 1, MAP_W, MAP_H, world.characters); break;
                    case 'a': case 'A': player->move(-1, 0, MAP_W, MAP_H, world.characters); break;
                    case 'd': case 'D': player->move(1, 0, MAP_W, MAP_H, world.characters); break;
                    case 'e': case 'E':
                        if (world.checkCSV(player->x, player->y)) {
                            player->eatCSV(world.csv_positions[{player->x, player->y}]);
                            world.removeCSV(player->x, player->y);
                        } else {
                            cout << RED << "No CSV here!" << RESET << endl;
                            usleep(300000);
                        }
                        break;
                }
            }
        }

        if (!paused) {
            if (turn % 2 == 0) world.updateNPCs();

            for (auto c : world.characters) {
                if (c == player) continue;
                if (c->symbol == 'N' || c->symbol == 'P') {
                    if (c->health > 0 && abs(c->x - player->x) <= 2 && abs(c->y - player->y) <= 2) {
                        if (rand() % 3 == 0) c->chat(*player);
                    }
                }
            }

            turn++;
        }

        if (player->eaten >= GOAL) {
            world.display(showHelp);
            cout << BOLD << GREEN << "\n*** YOU WIN! Ate " << player->eaten << " CSVs! ***" << RESET << endl;
            Logger::log("Player won");
            usleep(2000000);
            break;
        }
        if (player->health <= 0) {
            world.display(showHelp);
            cout << BOLD << RED << "\n*** GAME OVER - You died! ***" << RESET << endl;
            Logger::log("Player died");
            usleep(2000000);
            break;
        }
    }

    cout << BOLD << CYAN << "\n=== GAME OVER ===" << RESET << endl;
    cout << "Check game_log.txt for the full interaction log!" << endl;

    for (auto c : world.characters) delete c;
    return 0;
}
