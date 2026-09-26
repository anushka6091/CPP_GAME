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

/**
 * @brief High-level Orchestrator controlling dungeon generation, movement, exploration, combat, and Quests.
 * 
 * DESIGN PATTERN: Controller / Observer Publisher Host
 * WHY: GameEngine owns the EventBus and Quest observers, initializing active quest lines 
 * and passing EventBus references to command execution pipelines.
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

public:
    GameEngine();
    ~GameEngine() = default;

    void initialize();
    void start();
    void runCommandLoop();
    void runCombatLoop();
    void printHelp() const;

    const CommandHistory& getHistory() const { return m_history; }
    Dungeon* getDungeon() const { return m_dungeon.get(); }
    EventBus& getEventBus() { return m_eventBus; }
};

#endif // GAMEENGINE_H
