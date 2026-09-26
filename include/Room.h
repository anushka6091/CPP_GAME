#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "Enemy.h"
#include "GameObject.h"

/**
 * @brief Enum representing cardinal navigation directions.
 */
enum class Direction {
    North,
    South,
    East,
    West
};

// Helper functions for Direction enum
std::string directionToString(Direction dir);
Direction stringToDirection(const std::string& str);
Direction getOppositeDirection(Direction dir);

/**
 * @brief Represents a dungeon room containing directional exits, optional enemies, and GameObjects.
 * 
 * DESIGN PATTERN: Graph Node & Composite Integration
 * WHY: Room functions as a graph vertex with directional pointer edges to adjacent Rooms, 
 * owning its contained items and enemies via RAII unique_ptrs.
 */
class Room {
private:
    std::string m_description;
    std::unique_ptr<Enemy> m_enemy;
    std::vector<std::unique_ptr<GameObject>> m_items;
    std::map<Direction, Room*> m_exits;
    bool m_isBossRoom{false};

public:
    Room(std::string description, std::unique_ptr<Enemy> enemy = nullptr);

    const std::string& getDescription() const { return m_description; }
    Enemy* getEnemy() const { return m_enemy.get(); }
    bool hasEnemy() const { return m_enemy != nullptr && m_enemy->isAlive(); }

    bool isBossRoom() const { return m_isBossRoom; }
    void setBossRoom(bool isBoss) { m_isBossRoom = isBoss; }


    void setEnemy(std::unique_ptr<Enemy> enemy) { m_enemy = std::move(enemy); }

    // Exit Map Navigation
    void setExit(Direction dir, Room* room);
    Room* getExit(Direction dir) const;
    const std::map<Direction, Room*>& getExits() const { return m_exits; }
    std::string getExitsString() const;

    // GameObject Management inside Room
    void addItem(std::unique_ptr<GameObject> item);
    std::unique_ptr<GameObject> removeItem(const std::string& name);
    void look() const;

    const std::vector<std::unique_ptr<GameObject>>& getItems() const { return m_items; }
};

#endif // ROOM_H
