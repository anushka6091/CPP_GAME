#include "GameEngine.h"
#include "DungeonGenerator.h"
#include "CommandParser.h"
#include "ConcreteQuests.h"
#include "CharacterStates.h"
#include "Item.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <random>

GameEngine::GameEngine()
    : m_currentRoom(nullptr) {}

void GameEngine::initialize() {
    std::cout << "====================================================\n";
    std::cout << "          WELCOME TO MYSTICAL MYTH (PART 6)         \n";
    std::cout << "====================================================\n\n";

    std::cout << "Enter your Hero's name: ";
    std::string playerName;
    if (!(std::cin >> playerName)) {
        playerName = "Hero";
    }

    m_player = std::make_unique<Player>(playerName, 45, 12, 3);

    std::cout << "Enter Random Seed for Procedural Generation (e.g. 42 or 0 for random): ";
    int seed = 42;
    if (!(std::cin >> seed) || seed == 0) {
        std::random_device rd;
        seed = rd();
    }
    m_seed = seed;

    std::cout << "\nSelect Difficulty Mode:\n";
    std::cout << "  1. Normal  - Balanced combat and standard loot drops\n";
    std::cout << "  2. Hard    - 1.5x enemy health/attack, enhanced rewards\n";
    std::cout << "Choice (1 or 2, default 1): ";
    int diffChoice = 1;
    if (!(std::cin >> diffChoice) || diffChoice != 2) {
        m_difficultyLevel = DifficultyLevel::Normal;
    } else {
        m_difficultyLevel = DifficultyLevel::Hard;
    }
    m_difficulty = (m_difficultyLevel == DifficultyLevel::Hard) ? 2 : 1;

    m_turnCount = 0;
    m_enemiesKilled = 0;

    // 1. Procedural Dungeon Generation
    StandardDungeonGenerator generator;
    m_dungeon = generator.generate(m_seed, m_difficulty);

    // Apply difficulty modifiers (enemy stat scaling, loot pool enhancement)
    DifficultyManager::applyModifiers(*m_dungeon, m_difficultyLevel);
    m_currentRoom = m_dungeon->getStartRoom();

    bool solvable = m_dungeon->verifySolvability();

    std::cout << "\n[Dungeon Generator] Generated " << m_dungeon->getRoomCount() 
              << "-room procedural dungeon level using seed " << seed 
              << " (" << difficultyToString(m_difficultyLevel) << " Mode).\n";
    std::cout << "[Solvability Audit] Graph BFS Path Verification: " 
              << (solvable ? "PASSED (100% Solvable Path to Boss Room Guaranteed)" : "FAILED") << "\n";

    // 2. Initialize Observer Quests and register with EventBus
    m_quests.clear();
    
    // Quest 1 (Main Boss Key Quest): ItemCollectionQuest
    auto keyQuest = std::make_unique<ItemCollectionQuest>(
        "Golden Key Quest", 
        "Find the Golden Key hidden in the dungeon to unseal the Boss Gate", 
        "Golden Key", 
        1
    );

    // Quest 2 (Side Bounty Quest): KillCountQuest
    auto goblinQuest = std::make_unique<KillCountQuest>(
        "Goblin Slayer Quest", 
        "Slay 2 Goblins to clear the dungeon corridors", 
        "Goblin", 
        2
    );

    // Subscribe quests as Observers on EventBus
    m_eventBus.subscribe(keyQuest.get());
    m_eventBus.subscribe(goblinQuest.get());

    m_quests.push_back(std::move(keyQuest));
    m_quests.push_back(std::move(goblinQuest));

    std::cout << "[EventBus & Observer Quests] Initialized and subscribed " << m_quests.size() 
              << " active quests to EventBus.\n";

    // 3. Initialize CraftingStation with known recipes
    m_craftingStation = CraftingStation{};

    // Recipe 1: 2x Healing Herb -> Health Potion
    m_craftingStation.addRecipe(CraftingRecipe(
        "Health Potion",
        "Brew 2 Healing Herbs into a restorative potion.",
        {{"Healing Herb", 2}},
        []() { return std::make_unique<Item>("Health Potion", ItemType::Potion,
                   "A fresh handcrafted potion restoring 30 HP.", 30, 0); }
    ));

    // Recipe 2: 1x Iron Ore + 1x Monster Fang -> Reinforced Sword
    m_craftingStation.addRecipe(CraftingRecipe(
        "Reinforced Sword",
        "Forge Iron Ore and a Monster Fang into a savage upgraded blade.",
        {{"Iron Ore", 1}, {"Monster Fang", 1}},
        []() { return std::make_unique<Item>("Reinforced Sword", ItemType::Weapon,
                   "A fang-tipped iron blade crackling with dark energy.", 0, 14); }
    ));

    // Recipe 3: 1x Healing Herb + 1x Venom Sac -> Antidote Potion
    m_craftingStation.addRecipe(CraftingRecipe(
        "Antidote Potion",
        "Combine a Healing Herb with a Venom Sac to create an antidote.",
        {{"Healing Herb", 1}, {"Venom Sac", 1}},
        []() { return std::make_unique<Item>("Antidote Potion", ItemType::Potion,
                   "A murky antidote that cures poison and restores 15 HP.", 15, 0); }
    ));

    std::cout << "[Crafting Station] Registered " << m_craftingStation.getRecipes().size() 
              << " crafting recipes. Type 'recipes' to view them.\n";
}

void GameEngine::start() {
    if (!m_player || !m_dungeon || !m_currentRoom) {
        std::cerr << "Engine not initialized properly!\n";
        return;
    }

    std::cout << "\n--- ENTERING DUNGEON ---\n";
    m_currentRoom->look();

    // Trigger initial RoomEntered event over EventBus
    m_eventBus.publish(GameEvent(GameEventType::RoomEntered, m_currentRoom->getDescription(), 1));

    printHelp();
    runCommandLoop();
}

void GameEngine::printHelp() const {
    std::cout << "\n=== AVAILABLE COMMANDS (Quest Observer + Crafting + Persistence Enabled) ===\n";
    std::cout << "  look                      : Inspect room surroundings, exits, and items\n";
    std::cout << "  move <north|south|east|west>: Navigate to adjacent room through exit\n";
    std::cout << "  pickup <item>             : Pick up an item or container from room\n";
    std::cout << "  inventory (or inv)        : List items in player's inventory\n";
    std::cout << "  use <item>                : Use an item (consume potion or equip weapon)\n";
    std::cout << "  equip <weapon>            : Equip a weapon from inventory\n";
    std::cout << "  attack                    : Execute AttackCommand against room enemy\n";
    std::cout << "  flee                      : Execute FleeCommand to retreat from combat\n";
    std::cout << "  quests                    : Display active quest log and progress\n";
    std::cout << "  recipes                   : List all available crafting recipes\n";
    std::cout << "  craft <recipe>            : Craft an item using materials in your inventory\n";
    std::cout << "  save [name]               : Save current game session to SQLite database\n";
    std::cout << "  load <player name>        : Restore saved session from SQLite database\n";
    std::cout << "  leaderboard [turns|kills] : View Hall of Fame high scores\n";
    std::cout << "  replay                    : Re-print full sequence of executed command history\n";
    std::cout << "  help                      : Print this command menu\n";
    std::cout << "  quit                      : Exit game engine\n";
    std::cout << "===========================================================================\n";
}

void GameEngine::runCommandLoop() {
    std::string line;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    const Quest* requiredQuest = !m_quests.empty() ? m_quests[0].get() : nullptr;

    while (m_player->isAlive()) {
        std::cout << "\n> ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue;

        if (line == "help") {
            printHelp();
            continue;
        }
        if (line == "quit" || line == "exit") {
            std::cout << "Exiting Mystical Myth Engine. Goodbye!\n";
            break;
        }

        m_turnCount++;

        // Command Pattern Execution Pipeline with EventBus, Quest Observer, Crafting, and Persistence support:
        auto cmd = CommandParser::parse(
            line, *m_player, m_currentRoom, m_history, &m_eventBus, 
            requiredQuest, m_quests, &m_craftingStation, this, &m_saveManager
        );
        if (cmd) {
            bool success = cmd->execute();
            if (success) {
                m_history.record(cmd->description());
            }
        }
    }
}

void GameEngine::runCombatLoop() {
    if (m_currentRoom) {
        AttackCommand attackCmd(*m_player, *m_currentRoom, &m_eventBus, this);
        attackCmd.execute();
    }
}

GameMemento GameEngine::saveState() const {
    GameMemento memento;
    memento.playerName = m_player ? m_player->getName() : "Hero";
    memento.dungeonSeed = m_seed;
    memento.difficultyLevel = m_difficulty;
    memento.difficultyName = difficultyToString(m_difficultyLevel);
    memento.turnCount = m_turnCount;
    memento.enemiesKilled = m_enemiesKilled;
    memento.currentRoomDescription = m_currentRoom ? m_currentRoom->getDescription() : "";

    if (m_player) {
        memento.player = m_player->saveState();
    }

    for (const auto& q : m_quests) {
        QuestMemento qm;
        qm.name = q->getName();
        qm.state = q->getStateString();
        if (auto* kq = dynamic_cast<KillCountQuest*>(q.get())) {
            qm.currentProgress = kq->getCurrentKills();
            qm.requiredProgress = kq->getRequiredKills();
        } else if (auto* iq = dynamic_cast<ItemCollectionQuest*>(q.get())) {
            qm.currentProgress = iq->getCurrentAmount();
            qm.requiredProgress = iq->getRequiredAmount();
        }
        memento.quests.push_back(qm);
    }

    return memento;
}

bool GameEngine::restoreState(const GameMemento& memento) {
    m_seed = memento.dungeonSeed;
    m_difficulty = memento.difficultyLevel;
    m_difficultyLevel = stringToDifficulty(memento.difficultyName);
    m_turnCount = memento.turnCount;
    m_enemiesKilled = memento.enemiesKilled;

    // 1. Re-generate identical procedural dungeon
    StandardDungeonGenerator generator;
    m_dungeon = generator.generate(m_seed, m_difficulty);
    DifficultyManager::applyModifiers(*m_dungeon, m_difficultyLevel);

    // 2. Locate saved room by description
    m_currentRoom = nullptr;
    for (const auto& r : m_dungeon->getRooms()) {
        if (r->getDescription() == memento.currentRoomDescription) {
            m_currentRoom = r.get();
            break;
        }
    }
    if (!m_currentRoom) {
        m_currentRoom = m_dungeon->getStartRoom();
    }

    // 3. Restore player state
    if (!m_player) {
        m_player = std::make_unique<Player>(memento.player.name);
    }
    m_player->restoreState(memento.player);

    // 4. Restore quests & re-subscribe to EventBus
    m_quests.clear();
    auto keyQuest = std::make_unique<ItemCollectionQuest>(
        "Golden Key Quest", 
        "Find the Golden Key hidden in the dungeon to unseal the Boss Gate", 
        "Golden Key", 
        1
    );
    auto goblinQuest = std::make_unique<KillCountQuest>(
        "Goblin Slayer Quest", 
        "Slay 2 Goblins to clear the dungeon corridors", 
        "Goblin", 
        2
    );

    for (const auto& qm : memento.quests) {
        if (qm.name == keyQuest->getName()) {
            if (qm.state == "COMPLETED") keyQuest->setState(QuestState::Completed);
        } else if (qm.name == goblinQuest->getName()) {
            if (qm.state == "COMPLETED") goblinQuest->setState(QuestState::Completed);
        }
    }

    m_eventBus.subscribe(keyQuest.get());
    m_eventBus.subscribe(goblinQuest.get());
    m_quests.push_back(std::move(keyQuest));
    m_quests.push_back(std::move(goblinQuest));

    return true;
}

bool GameEngine::saveGame(const std::string& customName) {
    std::string name = customName.empty() ? (m_player ? m_player->getName() : "Hero") : customName;
    GameMemento memento = saveState();
    return m_saveManager.saveGame(name, memento);
}

bool GameEngine::loadGame(const std::string& playerName) {
    GameMemento memento;
    bool success = m_saveManager.loadGame(playerName, memento);
    if (success) {
        restoreState(memento);
        std::cout << "\n--- RESUMING RESTORED SESSION ---\n";
        if (m_currentRoom) {
            m_currentRoom->look();
        }
        return true;
    }
    return false;
}

void GameEngine::onBossDefeated() {
    m_enemiesKilled++;
    std::cout << "\n============================================================\n";
    std::cout << ">>> Would you like to record your victory on the Leaderboard? (y/n): ";
    std::string choice;
    if (std::cin >> choice && (choice == "y" || choice == "yes" || choice == "Y" || choice == "YES")) {
        std::string heroName = m_player ? m_player->getName() : "Hero";
        m_saveManager.recordLeaderboardEntry(heroName, m_seed, m_turnCount, m_enemiesKilled, difficultyToString(m_difficultyLevel));
        m_saveManager.printLeaderboard("turns", difficultyToString(m_difficultyLevel));
    }

    std::cout << "\n============================================================\n";
    std::cout << ">>> Would you like to ascend to NEW GAME+ (NG+)? (y/n): ";
    std::string ngChoice;
    if (std::cin >> ngChoice && (ngChoice == "y" || ngChoice == "yes" || ngChoice == "Y" || ngChoice == "YES")) {
        startNewGamePlus();
    }
}

void GameEngine::startNewGamePlus() {
    std::cout << "\n====================================================\n";
    std::cout << "      ASCENDING TO NEW GAME+ (NG+) MODE!           \n";
    std::cout << "====================================================\n";
    std::cout << "You awaken with all your honed skills, equipment, and items,\n";
    std::cout << "but the dungeon has reshaped itself into a deadly trial...\n\n";

    m_difficultyLevel = DifficultyLevel::NewGamePlus;
    m_seed += 1007; // Deterministic seed progression for NG+
    m_difficulty = 3;

    // 1. Procedural Dungeon Generation with new seed
    StandardDungeonGenerator generator;
    m_dungeon = generator.generate(m_seed, m_difficulty);

    // 2. Apply NewGamePlus difficulty modifiers (2.0x stats, rare loot, extra boss phase)
    DifficultyManager::applyModifiers(*m_dungeon, m_difficultyLevel);
    m_currentRoom = m_dungeon->getStartRoom();

    // 3. Restore player health to max for the new adventure (carrying over level/equipment/inventory)
    if (m_player) {
        m_player->setHealth(m_player->getMaxHealth());
        m_player->transitionTo(std::make_unique<AliveState>());
    }

    // 4. Reset & re-subscribe Quests for the NG+ dungeon run
    m_quests.clear();
    auto keyQuest = std::make_unique<ItemCollectionQuest>(
        "Golden Key Quest", 
        "Find the Golden Key hidden in the dungeon to unseal the Boss Gate", 
        "Golden Key", 
        1
    );
    auto goblinQuest = std::make_unique<KillCountQuest>(
        "Goblin Slayer Quest", 
        "Slay 2 Goblins to clear the dungeon corridors", 
        "Goblin", 
        2
    );
    m_eventBus.subscribe(keyQuest.get());
    m_eventBus.subscribe(goblinQuest.get());
    m_quests.push_back(std::move(keyQuest));
    m_quests.push_back(std::move(goblinQuest));

    std::cout << "[NG+ System] New procedural dungeon initialized with seed " << m_seed << ".\n";
    std::cout << "[NG+ System] Boss has unlocked its ASCENDED 4th phase threshold (<35% HP)!\n";

    if (m_currentRoom) {
        std::cout << "\n--- ENTERING NEW GAME+ DUNGEON ---\n";
        m_currentRoom->look();
        m_eventBus.publish(GameEvent(GameEventType::RoomEntered, m_currentRoom->getDescription(), 1));
    }
}
