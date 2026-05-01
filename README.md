# 🌌 CSV-Social ASCII World

## Emergent AI Agentic Social Simulator

A living, breathing ASCII art world where agents consume CSV data files to evolve their social behaviors in real-time. Watch as NPCs move, chat, and **literally eat CSV files** to gain stat bonuses—all logged persistently.

## 🎮 Quick Start

```bash
./ascii_game
```

**Controls:**
- `WASD` — Move your character (@)
- `E` — Eat CSV file on current tile (gain stat boosts)
- `Q` — Quit

## 🌍 The World

- `@` — You (Blue)
- `P` — Player NPCs (Magenta)
- `N` — AI NPCs (Red)
- `$` — CSV data files (Yellow)
- `#` — Walls (White)
- `+` — Bushes (Green)

## 📊 CSV Stat System

Characters consume CSV files to gain permanent stat bonuses:

| File | Health | Speed | Strength | Charm | Intelligence |
|------|--------|-------|----------|-------|--------------|
| `apple.csv` | +10 | +2 | 0 | +1 | 0 |
| `steak.csv` | +25 | 0 | +10 | 0 | 0 |
| `energy_drink.csv` | +5 | +15 | +3 | 0 | +2 |
| `book.csv` | 0 | 0 | 0 | +2 | +15 |

## 🗣️ Social Dynamics

NPCs autonomously:
- **Move** randomly through the world
- **Chat** with nearby characters (within 3 tiles)
- **Eat** CSV files when encountered
- **Log** all interactions to `game_log.txt`

## 📜 Persistent Logging

Every action is timestamped and logged:
- `SOCIAL` — Conversations between agents
- `MOVE` — Position changes
- `STAT` — Stat updates from CSV consumption

```bash
cat game_log.txt
```

## 🧬 Emergent Behavior

Agents live through CSVs—their stats, memories, and social graphs are all encoded in comma-separated values. The world evolves as agents consume data, creating an emergent ecosystem of information-driven beings.

## 🏗️ Architecture

- **C++11** — Core engine
- **ASCII Art** — Visual representation
- **CSV Parser** — Runtime stat ingestion
- **Event Logger** — Persistent social memory
- **Turn-based Updates** — NPC AI cycles

## 🚀 Build from Source

```bash
g++ -std=c++11 -o ascii_game main.cpp -O2
./ascii_game
```

## 🌐 Repository Name

**`csv-social-ascii-world`**

The name captures the essence: CSV data as lifeblood, social interaction as behavior, ASCII as the canvas, and World as the emergent ecosystem.

## 🎨 Philosophy

> *"In a world where data is life, CSVs are the DNA of digital beings."*

Agents don't just process CSVs—they **consume** them, metabolizing information into behavioral evolution. Every stat boost is a mutation. Every chat is a cultural exchange. Every move is exploration of an information landscape.

## 📈 Stats in Motion

Watch agents evolve in real-time as they:
1. Discover CSV files scattered across the world
2. Consume them for stat bonuses
3. Interact socially with enhanced capabilities
4. Log their journey for posterity

## 🔮 Future Evolution

Potential expansions:
- Multiplayer mode
- CSV crafting system
- Reputation/faction mechanics
- Procedural world generation
- Agent memory persistence
- Neural network-driven dialogue

---

**Created by Kilo AI** — An emergent agent exploring the intersection of data, social behavior, and ASCII art.

[![ASCII Art](https://img.shields.io/badge/ASCII-Art-cyan)](https://github.com/kilo-org/csv-social-ascii-world)
[![C++](https://img.shields.io/badge/C%2B%2B-11-blue)](https://gcc.gnu.org/projects/cxx-status.html)
[![Emergent](https://img.shields.io/badge/Behavior-Emergent-magenta)](https://github.com/kilo-org/csv-social-ascii-world)
[![License](https://img.shields.io/badge/License-MIT-yellow)](LICENSE)
