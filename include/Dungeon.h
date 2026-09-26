#ifndef DUNGEON_H
#define DUNGEON_H

#include "Room.h"
#include <vector>
#include <memory>

/**
 * @brief Class owning all rooms in a dungeon level and managing start/boss room entry points.
 * 
 * DESIGN PATTERN: Aggregate Root & RAII Container
 * WHY: Dungeon strictly owns all Room instances via std::vector<std::unique_ptr<Room>>,
 * providing non-owning raw pointers to m_startRoom and m_bossRoom for navigation.
 */
class Dungeon {
private:
    std::vector<std::unique_ptr<Room>> m_rooms;
    Room* m_startRoom;
    Room* m_bossRoom;

public:
    Dungeon();

    void addRoom(std::unique_ptr<Room> room);
    void setStartRoom(Room* room) { m_startRoom = room; }
    void setBossRoom(Room* room) { 
        m_bossRoom = room; 
        if (m_bossRoom) m_bossRoom->setBossRoom(true); 
    }


    Room* getStartRoom() const { return m_startRoom; }
    Room* getBossRoom() const { return m_bossRoom; }
    size_t getRoomCount() const { return m_rooms.size(); }
    const std::vector<std::unique_ptr<Room>>& getRooms() const { return m_rooms; }

    /**
     * @brief Performs BFS graph traversal from m_startRoom to verify 100% path solvability to m_bossRoom.
     * @return true if m_bossRoom is reachable from m_startRoom; false otherwise.
     */
    bool verifySolvability() const;
};

#endif // DUNGEON_H
