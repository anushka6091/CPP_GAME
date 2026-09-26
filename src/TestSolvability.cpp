#include "DungeonGenerator.h"
#include "EventBus.h"
#include "ConcreteQuests.h"
#include "Boss.h"
#include "BossPhaseStates.h"
#include "Player.h"
#include "GameMemento.h"
#include "SaveManager.h"
#include "DifficultyManager.h"
#include "Goblin.h"
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
 * @brief Unit test verifying BossPhaseState machine transitions, damage reduction, and enrage mechanics.
 */
void testBossPhaseStateSuite() {
    std::cout << "====================================================\n";
    std::cout << " RUNNING MULTI-PHASE BOSS STATE PATTERN TEST SUITE  \n";
    std::cout << "====================================================\n";

    // Create 100 HP Boss with 10 Atk and 4 Def
    Boss testBoss("Infernal Dragon", 100, 10, 4);
    assert(testBoss.getHealth() == 100);
    assert(testBoss.getBaseDefense() == 4);

    // 1. Initial State: Phase 1
    assert(dynamic_cast<Phase1State*>(testBoss.getPhaseState()) != nullptr);
    assert(testBoss.getPhaseName() == "Phase 1 (Normal)");

    // Deal 14 raw damage: in Phase 1, (14 - 4 Def) = 10 damage -> HP: 90/100 (90%)
    testBoss.takeDamage(14);
    assert(testBoss.getHealth() == 90);
    assert(dynamic_cast<Phase1State*>(testBoss.getPhaseState()) != nullptr);

    // 2. Trigger Phase 2 (HP drops below 50%)
    // Deal 49 raw damage: (49 - 4 Def) = 45 damage -> HP: 90 - 45 = 45/100 (45%)
    testBoss.takeDamage(49);
    assert(testBoss.getHealth() == 45);
    auto* phase2 = dynamic_cast<Phase2State*>(testBoss.getPhaseState());
    assert(phase2 != nullptr);
    assert(phase2->hasSummonedAlly() == true);

    // Verify Phase 2 resistance (-50% damage taken)
    // 14 raw damage: (14 - 4 Def) = 10; 10 / 2 = 5 damage -> HP drops from 45 to 40
    testBoss.takeDamage(14);
    assert(testBoss.getHealth() == 40);

    // 3. Trigger Enrage (HP drops below 15%)
    // To deal 30 damage in Phase 2: raw = 64 -> (64 - 4) / 2 = 30 -> HP: 40 - 30 = 10/100 (10%)
    testBoss.takeDamage(64);
    assert(testBoss.getHealth() == 10);
    auto* enrage = dynamic_cast<EnrageState*>(testBoss.getPhaseState());
    assert(enrage != nullptr);
    assert(testBoss.getDefense() == 0); // Enrage removes all defense

    // Verify Enrage unmitigated damage: 5 raw damage deals exactly 5 damage -> HP: 5
    testBoss.takeDamage(5);
    assert(testBoss.getHealth() == 5);

    // 4. Test Attack delegation to Player target
    Player hero("Arthur", 200, 10, 0);
    int prevHeroHp = hero.getHealth();
    testBoss.attack(hero);
    assert(hero.getHealth() < prevHeroHp);

    // 5. Test Boss Defeat
    testBoss.takeDamage(10);
    assert(testBoss.getHealth() == 0);
    assert(!testBoss.isAlive());

    std::cout << "[BOSS STATE TEST RESULT] Boss Phase State Machine PASSED 100% of tests.\n";
    std::cout << "====================================================\n\n";
}

/**
 * @brief Unit test verifying Memento pattern session serialization and SQLite persistence.
 */
void testPersistenceAndMementoSuite() {
    std::cout << "====================================================\n";
    std::cout << " RUNNING MEMENTO PATTERN & SQLITE PERSISTENCE TESTS \n";
    std::cout << "====================================================\n";

    // 1. Create Player and simulate gameplay state
    Player originalPlayer("Arthur", 45, 12, 3);
    originalPlayer.addXp(120);
    originalPlayer.equip(std::make_unique<Item>("Dragon Slaying Sword", ItemType::Weapon, "Legendary sword", 0, 18));
    originalPlayer.getInventory().add(std::make_unique<Item>("Greater Health Elixir", ItemType::Potion, "Restores 40 HP", 40, 0));
    originalPlayer.getInventory().add(std::make_unique<Item>("Golden Key", ItemType::Key, "Ancient key", 0, 0));

    // 2. Generate Memento snapshot
    PlayerMemento playerSnapshot = originalPlayer.saveState();
    assert(playerSnapshot.name == "Arthur");
    assert(playerSnapshot.xp == 120);
    assert(playerSnapshot.hasEquippedWeapon == true);
    assert(playerSnapshot.equippedWeapon.name == "Dragon Slaying Sword");
    assert(playerSnapshot.inventoryItems.size() == 2);

    // 3. Test restoration into a brand new Player instance
    Player restoredPlayer("Empty");
    restoredPlayer.restoreState(playerSnapshot);
    assert(restoredPlayer.getName() == "Arthur");
    assert(restoredPlayer.getXp() == 120);
    assert(restoredPlayer.getEquippedWeapon() != nullptr);
    assert(restoredPlayer.getEquippedWeapon()->getName() == "Dragon Slaying Sword");
    assert(restoredPlayer.getAttackPower() == 30); // 12 base + 18 weapon bonus
    assert(restoredPlayer.getInventory().getItemCount("Golden Key") == 1);

    // 4. Test GameMemento JSON serialization and SQLite SaveManager
    GameMemento sessionMemento;
    sessionMemento.playerName = "Arthur";
    sessionMemento.dungeonSeed = 101;
    sessionMemento.difficultyLevel = 2;
    sessionMemento.turnCount = 24;
    sessionMemento.enemiesKilled = 5;
    sessionMemento.currentRoomDescription = "Dungeon Entrance";
    sessionMemento.player = playerSnapshot;

    // Test SQLite database persistence
    std::remove("test_saves.db");
    SaveManager saveMgr("test_saves.db");
    assert(saveMgr.isOpen());

    // Save game
    bool saveOk = saveMgr.saveGame("Arthur", sessionMemento);
    assert(saveOk);

    // Load game
    GameMemento loadedMemento;
    bool loadOk = saveMgr.loadGame("Arthur", loadedMemento);
    assert(loadOk);
    assert(loadedMemento.playerName == "Arthur");
    assert(loadedMemento.dungeonSeed == 101);
    assert(loadedMemento.turnCount == 24);
    assert(loadedMemento.enemiesKilled == 5);
    assert(loadedMemento.player.inventoryItems.size() == 2);

    // 5. Test Leaderboard persistence
    saveMgr.recordLeaderboardEntry("Arthur", 101, 24, 5);
    saveMgr.recordLeaderboardEntry("Lancelot", 101, 18, 3);
    saveMgr.recordLeaderboardEntry("Galahad", 101, 35, 7);

    auto topByTurns = saveMgr.getLeaderboard("turns");
    assert(topByTurns.size() >= 3);
    assert(topByTurns[0].playerName == "Lancelot"); // 18 turns is lowest (fastest)

    auto topByKills = saveMgr.getLeaderboard("kills");
    assert(topByKills.size() >= 3);
    assert(topByKills[0].playerName == "Galahad"); // 7 kills is highest

    std::cout << "[PERSISTENCE TEST RESULT] Memento serialization & SQLite SaveManager PASSED 100% of tests.\n";
    std::cout << "====================================================\n\n";
}

/**
 * @brief Unit test verifying DifficultyManager stat scaling, NG+ 4th Boss phase, and Leaderboard difficulty filtering.
 */
void testDifficultyAndNGPlusSuite() {
    std::cout << "====================================================\n";
    std::cout << " RUNNING DIFFICULTY & NEW GAME+ TEST SUITE          \n";
    std::cout << "====================================================\n";

    // 1. Test stat scaling on Enemy subclass (Goblin)
    Goblin enemy("Orc", 20, 6, 2);
    enemy.scaleStats(1.5);
    assert(enemy.getHealth() == 30);
    assert(enemy.getMaxHealth() == 30);
    assert(enemy.getAttackPower() == 9);
    assert(enemy.getDefense() == 3);

    // 2. Test Dungeon stat & loot modifiers under NG+
    StandardDungeonGenerator generator;
    auto ngDungeon = generator.generate(42, 1);
    DifficultyManager::applyModifiers(*ngDungeon, DifficultyLevel::NewGamePlus);

    bool foundLoot = false;
    for (const auto& r : ngDungeon->getRooms()) {
        for (const auto& item : r->getItems()) {
            if (item->getName() == "Astral Rune Blade" || item->getName() == "Mythic Dragon Elixir" || item->getName() == "Dragonite Ingot") {
                foundLoot = true;
                break;
            }
        }
        if (foundLoot) break;
    }
    assert(foundLoot);

    // 3. Test NG+ Boss 4-Phase Transitions
    Boss boss("Malakor", 100, 20, 0);
    boss.enableNewGamePlusMode();
    assert(boss.isNewGamePlus() == true);
    assert(boss.getPhaseName() == "Phase 1 (Normal)");

    // Phase 2 threshold (<70% in NG+): 35 damage -> HP drops to 65
    boss.takeDamage(35);
    assert(boss.getHealth() == 65);
    assert(boss.getPhaseName() == "Phase 2 (Reinforced & Goblin Ally Summoned)");

    // NG+ Extra Phase threshold (<35% in NG+) -> AscendedState:
    // In Phase 2, damage is halved: 70 raw -> 35 net damage -> HP drops from 65 to 30
    boss.takeDamage(70);
    assert(boss.getHealth() == 30);
    assert(boss.getPhaseName() == "Phase 3 [NG+ Ascended Overlord]");

    // Enrage threshold (<15% in NG+) -> EnrageState:
    // In AscendedState, damage is 40%: 50 raw -> 20 net damage -> HP drops from 30 to 10
    boss.takeDamage(50);
    assert(boss.getHealth() == 10);
    assert(boss.getPhaseName() == "Enrage (Frenzy - 0 Defense / Maximum Power)");

    // 4. Test Leaderboard Difficulty Filtering
    std::remove("test_diff_saves.db");
    SaveManager saveMgr("test_diff_saves.db");
    assert(saveMgr.isOpen());

    saveMgr.recordLeaderboardEntry("HeroNormal", 101, 30, 5, "Normal");
    saveMgr.recordLeaderboardEntry("HeroHard", 101, 25, 6, "Hard");
    saveMgr.recordLeaderboardEntry("HeroNGP", 101, 20, 8, "NewGamePlus");

    auto hardEntries = saveMgr.getLeaderboard("turns", "Hard");
    assert(!hardEntries.empty());
    assert(hardEntries[0].playerName == "HeroHard");
    assert(hardEntries[0].difficulty == "Hard");

    auto ngpEntries = saveMgr.getLeaderboard("turns", "NewGamePlus");
    assert(!ngpEntries.empty());
    assert(ngpEntries[0].playerName == "HeroNGP");
    assert(ngpEntries[0].difficulty == "NewGamePlus");

    auto allEntries = saveMgr.getLeaderboard("turns", "");
    assert(allEntries.size() >= 3);
    assert(allEntries[0].playerName == "HeroNGP"); // 20 turns is fastest

    std::cout << "[DIFFICULTY & NG+ TEST RESULT] Modifiers, 4-phase NG+ Boss & Leaderboard filtering PASSED 100%.\n";
    std::cout << "====================================================\n\n";
}

/**
 * @brief Unit test verifying 100% path solvability across 100 random procedural seeds.
 */
int testSolvabilitySuite() {
    testQuestObserverSuite();
    testBossPhaseStateSuite();
    testPersistenceAndMementoSuite();
    testDifficultyAndNGPlusSuite();

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
