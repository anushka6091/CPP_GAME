#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <memory>
#include <vector>
#include "Player.h"
#include "Room.h"
#include "Dungeon.h"
#include "CommandHistory.h"
#include "EventBus.h"
#include "Quest.h"
#include "CraftingSystem.h"
#include "GameMemento.h"
#include "SaveManager.h"
#include "DifficultyManager.h"

/**
 * @brief High-level Orchestrator controlling dungeon generation, movement, exploration, combat, and Quests.
 * 
 * DESIGN PATTERN: Controller / Observer Publisher Host / Memento Originator
 * WHY: GameEngine owns the EventBus and Quest observers, initializing active quest lines 
 * and passing EventBus references to command execution pipelines. It also acts as Originator
 * for GameMemento session snapshots.
 */
class GameEngine {
private:
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Dungeon> m_dungeon;
    Room* m_currentRoom;
    CommandHistory m_history;

    EventBus m_eventBus;
    std::vector<std::unique_ptr<Quest>> m_quests;
    CraftingStation m_craftingStation;

    int m_seed{42};
    int m_difficulty{1};
    DifficultyLevel m_difficultyLevel{DifficultyLevel::Normal};
    int m_turnCount{0};
    int m_enemiesKilled{0};
    SaveManager m_saveManager;

public:
    GameEngine();
    ~GameEngine() = default;

    void initialize();
    void start();
    void runCommandLoop();
    void runCombatLoop();
    void printHelp() const;

    // Memento Pattern Persistence Methods
    GameMemento saveState() const;
    bool restoreState(const GameMemento& memento);
    bool saveGame(const std::string& customName = "");
    bool loadGame(const std::string& playerName);
    void onBossDefeated();
    void startNewGamePlus();

    DifficultyLevel getDifficultyLevel() const { return m_difficultyLevel; }
    void setDifficultyLevel(DifficultyLevel level) { m_difficultyLevel = level; }

    const CommandHistory& getHistory() const { return m_history; }
    Dungeon* getDungeon() const { return m_dungeon.get(); }
    EventBus& getEventBus() { return m_eventBus; }
    SaveManager& getSaveManager() { return m_saveManager; }
    const SaveManager& getSaveManager() const { return m_saveManager; }
    int getTurnCount() const { return m_turnCount; }
    int getEnemiesKilled() const { return m_enemiesKilled; }
    void incrementEnemiesKilled() { ++m_enemiesKilled; }
};

#endif // GAMEENGINE_H
