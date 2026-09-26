#include "GameEngine.h"
#include "DungeonGenerator.h"
#include "CommandParser.h"
#include "ConcreteQuests.h"
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
    std::cout << "   WELCOME TO THE DUNGEON CRAWLER ENGINE (PART 6)  \n";
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

    std::cout << "Enter Difficulty Level (1 to 5): ";
    int difficulty = 1;
    if (!(std::cin >> difficulty) || difficulty < 1) {
        difficulty = 1;
    }

    // 1. Procedural Dungeon Generation
    StandardDungeonGenerator generator;
    m_dungeon = generator.generate(seed, difficulty);
    m_currentRoom = m_dungeon->getStartRoom();

    bool solvable = m_dungeon->verifySolvability();

    std::cout << "\n[Dungeon Generator] Generated " << m_dungeon->getRoomCount() 
              << "-room procedural dungeon level using seed " << seed << " (Difficulty " << difficulty << ").\n";
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
    std::cout << "\n=== AVAILABLE COMMANDS (Quest Observer + Crafting System Enabled) ===\n";
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
    std::cout << "  replay                    : Re-print full sequence of executed command history\n";
    std::cout << "  help                      : Print this command menu\n";
    std::cout << "  quit                      : Exit game engine\n";
    std::cout << "==========================================================\n";
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
            std::cout << "Exiting Dungeon Crawler Engine. Goodbye!\n";
            break;
        }

        // Command Pattern Execution Pipeline with EventBus, Quest Observer, and Crafting support:
        auto cmd = CommandParser::parse(line, *m_player, m_currentRoom, m_history, &m_eventBus, requiredQuest, m_quests, &m_craftingStation);
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
        AttackCommand attackCmd(*m_player, *m_currentRoom, &m_eventBus);
        attackCmd.execute();
    }
}
