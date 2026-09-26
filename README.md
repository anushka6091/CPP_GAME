# Mystical Myth: Pattern-Driven C++17 Roguelike Dungeon Engine

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg?logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
[![Build](https://img.shields.io/badge/Build-CMake-brightgreen.svg?logo=cmake)](https://cmake.org/)
[![Database](https://img.shields.io/badge/Database-SQLite3-lightgrey.svg?logo=sqlite)](https://www.sqlite.org/)
[![Serialization](https://img.shields.io/badge/JSON-nlohmann--json-orange.svg)](https://github.com/nlohmann/json)
[![Tests](https://img.shields.io/badge/Tests-100%25%20Passing-success.svg)](src/TestSolvability.cpp)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**Mystical Myth** is an extensible, pattern-driven roguelike dungeon crawler engine written in modern C++17. Far more than a simple terminal game, this project serves as a showcase of software engineering craft: zero-raw-pointer resource management (RAII), SOLID architectural principles, loose coupling via pub/sub telemetry, relational database persistence, and eight classic Gang of Four (GoF) design patterns working in concert. Every subsystem—from procedural dungeon graphs guaranteed 100% beatable via automated BFS solvers to an autonomous multi-phase boss state machine—is designed for modular expansion, clear testability, and enterprise-grade code maintainability.

---

## Table of Contents
- [Key Features](#key-features)
- [Architecture & Design Patterns](#architecture--design-patterns)
- [Tech Stack](#tech-stack)
- [Getting Started](#getting-started)
- [How to Play](#how-to-play)
- [Project Structure](#project-structure)
- [Automated Verification & Solvability](#automated-verification--solvability)
- [Future Roadmap](#future-roadmap)
- [License](#license)

---

## Key Features

- **Procedural Dungeon Graph Generation & Solvability Guarantees:**
  Generates connected non-linear dungeon graphs with configurable depth and seed determinism. Includes a built-in Breadth-First Search (BFS) graph solver that runs at generation time to mathematically guarantee a reachable critical path from spawn to boss room before gameplay begins.
- **Turn-Based Tactical Combat & Dynamic Conditions:**
  Engage monsters in tactical turn-based combat with damage calculation, defense mitigation, critical strikes, and active status condition lifecycles (`AliveState`, `PoisonedState`, `StunnedState`, `DeadState`).
- **Decoupled Observer Quest Pipeline:**
  Quests operate as autonomous observers subscribed to an asynchronous event bus (`EventBus`). The core engine publishes gameplay telemetry (`EnemyDefeated`, `ItemCollected`, `RoomEntered`) without direct knowledge of quest objectives, seamlessly driving item bounties, kill targets, and gating the Boss Chamber door.
- **Composite Item & Inventory Hierarchy:**
  Treats individual items and hierarchical containers (`Chest`, `Pouch`) uniformly through the Composite pattern. Containers can nest within player inventories or room environments to arbitrary depths.
- **Multi-Recipe Crafting System:**
  In-dungeon crafting workbench allowing players to combine raw gathered materials (ores, monster fangs, herbs) into upgraded weaponry and potent antidotes.
- **Multi-Phase Boss Fights:**
  Boss encounters utilize an independent state machine (`BossPhaseState`). The boss dynamically adapts tactics across health thresholds: slashing claws in Phase 1, hardening scales and summoning minion allies in Phase 2, unleashing meteor storms in New Game+ Ascended mode, and entering an unshielded berserk frenzy during Enrage (<15% HP).
- **SQLite3 Persistence & Hall of Fame Leaderboard:**
  Complete session serialization into opaque `GameMemento` snapshots stored inside an embedded SQLite database (`saves.db`). Features full save/load capabilities and a persistent leaderboard tracking turn efficiency, kill counts, and completed difficulty levels.
- **Difficulty Scaling & New Game+ (NG+):**
  Three distinct gameplay modes: *Normal*, *Hard* (1.5x enemy stats, enhanced draughts), and *New Game+* (2.0x stats, mythic weapons like the *Astral Rune Blade*, and an exclusive 4th boss phase). Upon boss victory, players can ascend directly to NG+, carrying over character level, equipment, and inventory into newly generated procedural depths.

---

## Architecture & Design Patterns

The engine was architected from day one to demonstrate how classic Gang of Four (GoF) design patterns solve real-world decoupling and extensibility challenges in C++.

| Pattern | Where It's Used (Class / File) | Why It Fits Here |
| :--- | :--- | :--- |
| **Inheritance & Polymorphism** | [`Character`](include/Character.h), [`Player`](include/Player.h), [`Enemy`](include/Enemy.h), [`Goblin`](include/Goblin.h), [`Skeleton`](include/Skeleton.h), [`Boss`](include/Boss.h) | Establishes a strict combat and stat lifecycle contract across all living entities, allowing the engine combat loop to operate on abstract references without type branching. |
| **Composite** | [`GameObject`](include/GameObject.h), [`Item`](include/Item.h), [`Container`](include/Container.h) | Allows leaf items (potions, swords) and composite containers (chests, pouches containing nested items) to share the same interface. Players can pick up, inspect, or search containers recursively without special cases. |
| **Command** | [`Command`](include/Command.h), [`MoveCommand`](include/ConcreteCommands.h), [`AttackCommand`](include/ConcreteCommands.h), [`CommandHistory`](include/CommandHistory.h) | Encapsulates player user actions as standalone first-class objects. Decouples CLI input parsing from gameplay execution, enabling instant command validation, action logging, and full session replay. |
| **State (Character Conditions)** | [`CharacterState`](include/CharacterState.h), [`AliveState`](include/CharacterStates.h), [`PoisonedState`](include/CharacterStates.h), [`StunnedState`](include/CharacterStates.h), [`DeadState`](include/CharacterStates.h) | Encapsulates temporary condition mechanics (damage-over-turn ticks, action suppression) cleanly without cluttering the `Character` class with nested `if/switch` flag checks. |
| **State (Boss Phases)** | [`BossPhaseState`](include/BossPhaseState.h), [`Phase1State`](include/BossPhaseStates.h), [`Phase2State`](include/BossPhaseStates.h), [`AscendedState`](include/BossPhaseStates.h), [`EnrageState`](include/BossPhaseStates.h) | Independent second use of the State pattern. Controls multi-tier boss combat behavior (stat modifications, scale hardening, minion ally summoning, meteor attacks) driven by health thresholds. Kept distinct from `CharacterState` because boss phase logic is a behavioral strategy machine rather than a temporary debuff condition. |
| **Factory / Builder** | [`DungeonGenerator`](include/DungeonGenerator.h), [`StandardDungeonGenerator`](include/DungeonGenerator.h) | Encapsulates complex procedural generation algorithms (seed randomization, room graph instantiation, exit cross-linking, item/enemy placement, and BFS path solvability verification) away from the game loop. |
| **Observer** | [`EventBus`](include/EventBus.h), [`GameEvent`](include/GameEvent.h), [`Quest`](include/Quest.h), [`ConcreteQuests`](include/ConcreteQuests.h) | Implements an asynchronous event notification pipeline. Combat and movement commands publish events (`ItemCollected`, `EnemyDefeated`) to the bus without coupling gameplay logic directly to quest progress or door locks. |
| **Memento** | [`GameMemento`](include/GameMemento.h), [`Player`](include/Player.h), [`GameEngine`](include/GameEngine.h), [`SaveManager`](include/SaveManager.h) | Captures deep snapshots of player stats, equipment, inventory trees, quest states, turn metrics, and dungeon seeds without exposing private fields. `SaveManager` acts as the Caretaker, serializing mementos to JSON blobs in SQLite. |

---

## Tech Stack

- **Core Language:** C++17
  - Strict RAII memory management (`std::unique_ptr`, `std::shared_ptr`, zero manual `delete` calls)
  - STL containers and algorithms (`std::vector`, `std::map`, `std::queue`, `std::find_if`)
  - Modern idioms: lambdas, move semantics, smart pointers, enum classes
- **Build System:** CMake (Version 3.14+)
- **Embedded Database:** SQLite3 (C library / amalgamation linked via CMake)
- **JSON Serialization:** `nlohmann/json` (header-only modern C++ JSON library)
- **Testing & Solvability Audit:** Integrated multi-suite test runner with 100-seed BFS graph path verifier (`mystical_myth --test`)

---

## Getting Started

### Prerequisites

Ensure you have the following installed on your machine:
- A C++17 compliant compiler:
  - **GCC 7+** (Linux / MinGW-w64 on Windows)
  - **Clang 5+** (macOS / Linux)
  - **MSVC 2019+** (Windows Visual Studio)
- **CMake** (v3.14 or later)
- **Git**

*(Note: SQLite3 amalgamation headers and `nlohmann/json` are bundled within the repository or fetched automatically by CMake, requiring no manual external library installation).*

### Clone, Build, and Run

#### Using CMake (Standard Cross-Platform)

```bash
# 1. Clone repository
git clone https://github.com/anushka6091/mystical-myth.git
cd mystical-myth

# 2. Configure build with CMake
mkdir build
cd build
cmake ..

# 3. Compile the executable
cmake --build .

# 4. Run the interactive game
./mystical_myth

# 5. Run the automated test suite
./mystical_myth --test
```

#### Windows MinGW (Direct GCC Compile)

If compiling directly on Windows via MinGW GCC:

```powershell
g++ -std=c++14 -Iinclude src/*.cpp third_party/sqlite3.o -o mystical_myth.exe
.\mystical_myth.exe --test
.\mystical_myth.exe
```

---

## How to Play

Upon launch, you will name your hero, configure a dungeon generation seed, and choose your starting difficulty (**Normal** or **Hard**).

### Console Command Reference

| Command | Example | Description |
| :--- | :--- | :--- |
| `look` | `look` | Inspect current room surroundings, exits, enemies, and items |
| `move <direction>` | `move north` (or `go east`) | Navigate to an adjacent room through an open doorway |
| `pickup <item>` | `pickup Iron Sword` | Pick up an item or container from the room or nested chest |
| `inventory` / `inv` | `inventory` | Display character stats, equipped weapon, and inventory items |
| `use <item>` | `use Health Potion` | Drink a restorative potion or apply a consumable |
| `equip <weapon>` | `equip Reinforced Sword` | Equip a weapon to increase attack power |
| `attack` / `fight` | `attack` | Strike the enemy in the room |
| `flee` / `run` | `flee` | Attempt to retreat from combat to an adjacent room |
| `quests` | `quests` | View current active quests, objectives, and completion status |
| `recipes` | `recipes` | View all available crafting station recipes |
| `craft <recipe>` | `craft Health Potion` | Combine required materials into upgraded items |
| `save [name]` | `save Arthur` | Save the current game session snapshot into the SQLite database |
| `load <name>` | `load Arthur` | Restore an existing saved game session from the SQLite database |
| `leaderboard [sort] [filter]` | `leaderboard turns hard` | View Hall of Fame high scores (sorted by `turns` or `kills`, filtered by difficulty) |
| `replay` | `replay` | Re-print the chronological command history of your run |
| `help` | `help` | Display the in-game command cheat sheet |
| `quit` / `exit` | `quit` | Exit the game engine |

---

## Project Structure

```text
mystical-myth/
├── CMakeLists.txt                # CMake build configuration (FetchContent / targets)
├── README.md                     # Project documentation & design pattern breakdown
├── include/                      # Header declarations & contracts
│   ├── Boss.h                    # Boss enemy entity with NG+ mode support
│   ├── BossPhaseState.h          # Abstract State interface for boss combat phases
│   ├── BossPhaseStates.h         # Phase 1, Phase 2, Ascended (NG+), & Enrage states
│   ├── Character.h               # Abstract base character (stats, health, conditions)
│   ├── CharacterState.h          # Abstract State interface for character conditions
│   ├── CharacterStates.h         # Alive, Poisoned, Stunned, and Dead states
│   ├── Command.h                 # Abstract Command interface
│   ├── CommandHistory.h          # Command execution log and replay recorder
│   ├── CommandParser.h           # Input tokenization and Command factory pipeline
│   ├── ConcreteCommands.h        # Move, Attack, Pickup, Use, Craft, Save, Load, etc.
│   ├── ConcreteQuests.h          # KillCountQuest and ItemCollectionQuest observers
│   ├── Container.h               # Composite pattern container (Chests, Pouches)
│   ├── CraftingSystem.h          # Recipe definitions and CraftingStation manager
│   ├── DifficultyManager.h       # Difficulty enum & stat/loot modifier engine
│   ├── Dungeon.h                 # Dungeon graph container & room management
│   ├── DungeonGenerator.h        # Procedural generator & BFS solvability validator
│   ├── Enemy.h                   # Abstract Enemy base class with loot drops & XP
│   ├── EventBus.h                # Observer pattern publish/subscribe event dispatcher
│   ├── GameEngine.h              # Game orchestrator, Originator for Memento snapshots
│   ├── GameEvent.h               # Telemetry event payload types
│   ├── GameMemento.h             # Memento state snapshot for deep serialization
│   ├── GameObject.h              # Base component for items and composite containers
│   ├── Goblin.h                  # Concrete enemy subclass
│   ├── Inventory.h               # Player inventory wrapper
│   ├── Item.h                    # Leaf component in Composite pattern
│   ├── Player.h                  # Playable character entity (equipment, inventory)
│   ├── Quest.h                   # Abstract Observer interface for quests
│   ├── Room.h                    # Graph vertex with directional edges & items
│   ├── SaveManager.h             # SQLite3 Caretaker for session persistence & leaderboard
│   ├── Skeleton.h                # Concrete enemy subclass
│   ├── sqlite3.h                 # SQLite3 C API header
│   └── nlohmann/                 # Modern C++ JSON library headers
│       └── json.hpp
├── src/                          # Implementation files (.cpp)
│   ├── Boss.cpp                  # Boss phase transition and combat dispatch
│   ├── BossPhaseStates.cpp       # Boss phase behaviors, defense scaling, minion summoning
│   ├── Character.cpp             # Base character combat and condition transitions
│   ├── CharacterStates.cpp       # Status condition turn logic & debuff processing
│   ├── CommandHistory.cpp        # Command history recording
│   ├── CommandParser.cpp         # String token parsing & command instantiation
│   ├── ConcreteCommands.cpp      # Implementation of all gameplay commands
│   ├── ConcreteQuests.cpp        # Event processing and quest completion logic
│   ├── Container.cpp             # Composite recursive search, display, and transfer
│   ├── CraftingSystem.cpp        # Recipe matching and item crafting execution
│   ├── DifficultyManager.cpp     # Multiplier calculation and dungeon loot/stat injection
│   ├── Dungeon.cpp               # Room storage and BFS path solvability verification
│   ├── DungeonGenerator.cpp      # Procedural generation algorithm & room layout
│   ├── Enemy.cpp                 # Loot dropping and enemy stats
│   ├── EventBus.cpp              # Observer registration and event broadcasting
│   ├── GameEngine.cpp            # Main loop, CLI handling, New Game+, Memento save/load
│   ├── GameMemento.cpp           # JSON serialization and deserialization
│   ├── GameObject.cpp            # Item base definitions
│   ├── Goblin.cpp                # Goblin combat actions
│   ├── Inventory.cpp             # Inventory querying and item storage
│   ├── Item.cpp                  # Potion healing and weapon damage stats
│   ├── Main.cpp                  # Application entry point & CLI flag handler
│   ├── Player.cpp                # Player leveling, equipment, and state snapshotting
│   ├── Quest.cpp                 # Quest state strings and completion tracking
│   ├── Room.cpp                  # Directional links and room item manipulation
│   ├── SaveManager.cpp           # SQLite table setup, query execution, transactions
│   ├── Skeleton.cpp              # Skeleton combat actions
│   └── TestSolvability.cpp       # Comprehensive unit tests & 100-seed solvability test suite
└── third_party/
    └── sqlite3.o                 # Precompiled SQLite3 engine object
```

---

## Automated Verification & Solvability

The project includes an integrated automated test suite covering all architectural subsystems. Run it via:

```bash
./mystical_myth --test
```

### Verified Test Suites:
1. **Observer Quest EventBus Suite:**
   Validates subscription, telemetry publishing (`EnemyDefeated`, `ItemCollected`), live progress tracking, and quest completion triggers.
2. **Multi-Phase Boss State Suite:**
   Simulates combat damage against the Boss, validating phase state transitions at exact health thresholds:
   - Phase 1 (>50% HP) &rarr; Phase 2 (<50% HP: scale defense hardening + minion ally summon).
   - Enrage Phase (<15% HP: 0 defense berserk frenzy, attack scaling, defeat sequence).
3. **Memento & SQLite Persistence Suite:**
   Verifies full deep snapshotting of Player stats, composite inventories, weapons, quest progress, and dungeon seeds into JSON blobs. Validates database write/read roundtrips and sorting orders in the SQLite Hall of Fame leaderboard.
4. **Difficulty & New Game+ Suite:**
   Tests stat scaling formulas (1.5x on Hard, 2.0x on NG+), verified placement of mythic loot pools (*Astral Rune Blade*, *Mythic Dragon Elixir*), the activation of the NG+ exclusive 4th boss phase (*AscendedState* at <35% HP), and leaderboard filtering by difficulty.
5. **Procedural Solvability Audit (100 Seeds):**
   Executes automated BFS graph traversals across 100 random dungeon generation seeds to verify that a reachable path always exists between the spawn room, key objectives, and the gated Boss room.

---

## Future Roadmap

The strict decoupling and clean abstraction boundaries of the engine make several natural extensions straightforward to implement:

- **AI Personalities via Strategy Pattern:**
  Refactor enemy decision-making into interchangeable strategies (`AggressiveStrategy`, `DefensiveStrategy`, `AmbushStrategy`, `FleeingStrategy`), enabling dynamic tactical behaviors based on remaining monster health and player equipment.
- **Branching Dialogue Trees via Interpreter / Composite Pattern:**
  Introduce friendly NPCs in neutral dungeon rooms, using composite dialogue nodes with condition-evaluated choices (e.g. reputation checks, item trades).
- **WebAssembly (WASM) / GUI Presentation Layer:**
  Because the engine core (`GameEngine`, `Dungeon`, `CommandParser`) is completely decoupled from terminal I/O, a lightweight frontend using **Raylib**, **ImGui**, or **Emscripten (WASM)** can be dropped in without changing a single line of backend game logic.

---

## License

Distributed under the MIT License. See `LICENSE` for more information.
