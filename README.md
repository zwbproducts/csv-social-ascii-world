# CSV-Social ASCII World

> An emergent, terminal-based social simulation where autonomous agents consume CSV data files to evolve their stats, interact, and compete — rendered in ASCII art.

![C++11](https://img.shields.io/badge/C%2B%2B-11-blue)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS-lightgrey)
![Status](https://img.shields.io/badge/Status-Playable-success)

---

## Overview

**CSV-Social ASCII World** is a lightweight, dependency-free C++ simulation that blends:

- **Data-driven gameplay** — agents metabolize real CSV files for permanent stat boosts.
- **Emergent social behavior** — NPCs wander, chat, and compete autonomously.
- **Terminal-native rendering** — pure ASCII art with ANSI color, no graphics libraries.
- **Persistent logging** — every move, chat, and stat change is timestamped to `game_log.txt`.

Built as a portfolio piece to demonstrate systems programming in C++11, terminal I/O, event logging, and game-loop design.

---

## Gameplay

You play as `@` (Hero). NPCs (`P`, `N`) roam the world. CSV data files appear as `$`. Stand on one and press `E` to consume it for stat boosts. Out-eat the NPCs to win.

### Controls

| Key | Action |
|-----|--------|
| `W` `A` `S` `D` | Move |
| `E` | Eat CSV on current tile |
| `H` | Toggle help |
| `P` | Pause / resume |
| `Q` | Quit |

### CSV Stat System

| File | Health | Speed | Strength | Charm | Intelligence |
|------|-------:|------:|---------:|------:|-------------:|
| `apple.csv` | +10 | +2 | 0 | +1 | 0 |
| `steak.csv` | +25 | 0 | +10 | 0 | 0 |
| `energy_drink.csv` | +5 | +15 | +3 | 0 | +2 |
| `book.csv` | 0 | 0 | 0 | +2 | +15 |

### Win / Lose

- **Win:** consume 8 CSV files before the NPCs do.
- **Lose:** your health reaches 0.

---

## Quick Start

```bash
git clone https://github.com/zwbproducts/csv-social-ascii-world.git
cd csv-social-ascii-world
./build.sh
./ascii_game
```

Or build manually:

```bash
g++ -std=c++11 -O2 -Wall -Wextra -o ascii_game main.cpp
./ascii_game
```

Optional flags:

```bash
./ascii_game --no-color   # disable ANSI colors
./ascii_game --help       # usage
```

---

## Architecture

```
main.cpp
├── Logger          # timestamped event logging (SOCIAL / MOVE / STAT)
├── CSVManager      # CSV parsing + sample file generation
├── Character       # base entity: stats, movement, eat, chat
├── World           # map, tiles, CSV spawns, NPC AI, rendering
└── main()          # arg parsing, init, game loop, win/lose handling
```

Key design points:

- **RAII terminal mode** — `RawMode` saves/restores `termios` and installs signal handlers for clean shutdown.
- **Non-blocking input** — `select()` with a 100 ms timeout keeps the loop responsive without busy-waiting.
- **Data-driven stats** — `StatBonus` is parsed at runtime from CSV, so new items need no recompilation.
- **Persistent memory** — all events append to `game_log.txt` for post-run analysis.

---

## Project Structure

```
.
├── main.cpp          # single-file engine
├── Makefile
├── build.sh
├── README.md
├── LICENSE
├── *.csv             # sample data files (auto-generated if missing)
└── game_log.txt      # generated at runtime
```

---

## Development

```bash
make          # build
make run      # build and run
make clean    # remove binary
```

Code style: C++11, `-Wall`, no external dependencies.

---

## Roadmap

- [ ] Multiplayer mode
- [ ] CSV crafting system
- [ ] Reputation / faction mechanics
- [ ] Procedural world generation
- [ ] Persistent agent memory
- [ ] Neural-network-driven dialogue

---

## Contributing

Issues and pull requests are welcome. Please keep changes focused and follow the existing code style.

---

## License

Released under the [MIT License](LICENSE).

---

## Author

Built by **zwbproducts** as an exploration of data-driven emergent behavior in terminal environments.
