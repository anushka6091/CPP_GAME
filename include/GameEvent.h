#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include <string>

/**
 * @brief Types of events that can occur in the game engine.
 */
enum class GameEventType {
    EnemyKilled,
    ItemCollected,
    RoomEntered
};

/**
 * @brief Data structure representing a game event published over the EventBus.
 */
struct GameEvent {
    GameEventType type;
    std::string targetName; // e.g. "Vicious Goblin", "Golden Key", "Alchemy Lab"
    int count = 1;

    GameEvent(GameEventType t, std::string target, int c = 1)
        : type(t), targetName(std::move(target)), count(c) {}
};

#endif // GAMEEVENT_H
