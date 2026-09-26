#include "Dungeon.h"
#include <queue>
#include <unordered_set>

Dungeon::Dungeon()
    : m_startRoom(nullptr), m_bossRoom(nullptr) {}

void Dungeon::addRoom(std::unique_ptr<Room> room) {
    if (room) {
        m_rooms.push_back(std::move(room));
    }
}

bool Dungeon::verifySolvability() const {
    if (!m_startRoom || !m_bossRoom) {
        return false;
    }
    if (m_startRoom == m_bossRoom) {
        return true;
    }

    std::queue<Room*> queue;
    std::unordered_set<Room*> visited;

    queue.push(m_startRoom);
    visited.insert(m_startRoom);

    while (!queue.empty()) {
        Room* current = queue.front();
        queue.pop();

        if (current == m_bossRoom) {
            return true; // Path found from Start to Boss room!
        }

        for (const auto& exitPair : current->getExits()) {
            Room* neighbor = exitPair.second;
            if (neighbor && visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                queue.push(neighbor);
            }
        }
    }

    return false; // Boss room unreachable from Start room
}
