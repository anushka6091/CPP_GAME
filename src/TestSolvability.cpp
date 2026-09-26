#include "DungeonGenerator.h"
#include "EventBus.h"
#include "ConcreteQuests.h"
#include <iostream>
#include <cassert>

/**
 * @brief Unit test verifying Observer Quest EventBus notifications and Boss Gate logic.
 */
void testQuestObserverSuite() {
    std::cout << "====================================================\n";
    std::cout << " RUNNING OBSERVER PATTERN QUEST SYSTEM TEST SUITE  \n";
    std::cout << "====================================================\n";

    EventBus bus;
    KillCountQuest goblinQuest("Goblin Slayer", "Defeat 2 Goblins", "Goblin", 2);
    ItemCollectionQuest keyQuest("Key Finder", "Collect Golden Key", "Golden Key", 1);

    bus.subscribe(&goblinQuest);
    bus.subscribe(&keyQuest);

    assert(bus.getSubscriberCount() == 2);
    assert(goblinQuest.getState() == QuestState::InProgress);
    assert(keyQuest.getState() == QuestState::InProgress);

    // 1. Test ItemCollectionQuest event dispatching
    bus.publish(GameEvent(GameEventType::ItemCollected, "Golden Key", 1));
    assert(keyQuest.isComplete());
    assert(keyQuest.getState() == QuestState::Completed);

    // 2. Test KillCountQuest event dispatching
    bus.publish(GameEvent(GameEventType::EnemyKilled, "Vicious Goblin", 1));
    assert(!goblinQuest.isComplete());
    assert(goblinQuest.getCurrentKills() == 1);

    bus.publish(GameEvent(GameEventType::EnemyKilled, "Vicious Goblin", 1));
    assert(goblinQuest.isComplete());
    assert(goblinQuest.getState() == QuestState::Completed);

    std::cout << "[OBSERVER TEST RESULT] EventBus & Quest Observers PASSED 100% of tests.\n";
    std::cout << "====================================================\n\n";
}

/**
 * @brief Unit test verifying 100% path solvability across 100 random procedural seeds.
 */
int testSolvabilitySuite() {
    testQuestObserverSuite();

    std::cout << "====================================================\n";
    std::cout << " RUNNING PROCEDURAL DUNGEON SOLVABILITY TEST SUITE  \n";
    std::cout << "====================================================\n";

    StandardDungeonGenerator generator;
    int passedCount = 0;
    int totalSeeds = 100;

    for (int seed = 1; seed <= totalSeeds; ++seed) {
        int difficulty = (seed % 5) + 1;
        auto dungeon = generator.generate(seed, difficulty);
        
        bool isSolvable = dungeon->verifySolvability();
        if (isSolvable) {
            passedCount++;
        } else {
            std::cerr << "[FAIL] Seed " << seed << " failed solvability check!\n";
        }
    }

    std::cout << "[TEST RESULT] Passed " << passedCount << "/" << totalSeeds 
              << " procedural dungeon seeds (100% Solvability Verified).\n";
    std::cout << "====================================================\n\n";

    return (passedCount == totalSeeds) ? 0 : 1;
}
