#include "Room.h"
#include "Container.h"
#include <iostream>
#include <algorithm>
#include <sstream>

std::string directionToString(Direction dir) {
    switch (dir) {
        case Direction::North: return "North";
        case Direction::South: return "South";
        case Direction::East:  return "East";
        case Direction::West:  return "West";
        default:               return "Unknown";
    }
}

Direction stringToDirection(const std::string& str) {
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "north" || s == "n") return Direction::North;
    if (s == "south" || s == "s") return Direction::South;
    if (s == "east"  || s == "e") return Direction::East;
    if (s == "west"  || s == "w") return Direction::West;
    return Direction::North; // Default fallback
}

Direction getOppositeDirection(Direction dir) {
    switch (dir) {
        case Direction::North: return Direction::South;
        case Direction::South: return Direction::North;
        case Direction::East:  return Direction::West;
        case Direction::West:  return Direction::East;
        default:               return Direction::South;
    }
}

Room::Room(std::string description, std::unique_ptr<Enemy> enemy)
    : m_description(std::move(description)), m_enemy(std::move(enemy)) {}

void Room::setExit(Direction dir, Room* room) {
    if (room) {
        m_exits[dir] = room;
    }
}

Room* Room::getExit(Direction dir) const {
    auto it = m_exits.find(dir);
    if (it != m_exits.end()) {
        return it->second;
    }
    return nullptr;
}

std::string Room::getExitsString() const {
    if (m_exits.empty()) {
        return "None (Dead End)";
    }
    std::ostringstream oss;
    bool first = true;
    for (const auto& exitPair : m_exits) {
        if (!first) oss << ", ";
        oss << directionToString(exitPair.first);
        first = false;
    }
    return oss.str();
}

void Room::addItem(std::unique_ptr<GameObject> item) {
    if (item) {
        m_items.push_back(std::move(item));
    }
}

std::unique_ptr<GameObject> Room::removeItem(const std::string& name) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    // 1. Check top-level items in room
    for (auto it = m_items.begin(); it != m_items.end(); ++it) {
        std::string objName = (*it)->getName();
        std::transform(objName.begin(), objName.end(), objName.begin(), ::tolower);

        if (objName == lowerName) {
            std::unique_ptr<GameObject> found = std::move(*it);
            m_items.erase(it);
            return found;
        }
    }

    // 2. Check inside top-level containers in room recursively
    for (auto& obj : m_items) {
        if (auto* container = dynamic_cast<Container*>(obj.get())) {
            auto nestedFound = container->remove(name);
            if (nestedFound) {
                std::cout << "[Room] Picked up '" << nestedFound->getName() 
                          << "' from inside container '" << container->getName() << "'!\n";
                return nestedFound;
            }
        }
    }

    return nullptr;
}

void Room::look() const {
    std::cout << "\n----------------------------------------------------\n";
    std::cout << "ROOM SURROUNDINGS:\n";
    std::cout << m_description << "\n";

    std::cout << "\n[EXITS]: " << getExitsString() << "\n";

    if (hasEnemy()) {
        std::cout << "\n[DANGER] Enemy Present: " << m_enemy->getName() 
                  << " (HP: " << m_enemy->getHealth() << "/" << m_enemy->getMaxHealth() << ")\n";
    } else {
        std::cout << "\n[SAFE] No active hostiles in room.\n";
    }

    std::cout << "\nROOM OBJECTS & CONTAINERS:\n";
    if (m_items.empty()) {
        std::cout << "  (There are no items lying on the ground)\n";
    } else {
        for (const auto& item : m_items) {
            std::cout << item->describe(2) << "\n";
        }
    }
    std::cout << "----------------------------------------------------\n";
}
