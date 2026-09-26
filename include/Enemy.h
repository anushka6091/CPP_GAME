#ifndef ENEMY_H
#define ENEMY_H

#include "Character.h"
#include "Item.h"
#include <vector>
#include <memory>

/**
 * @brief Abstract Base Class for all dungeon monsters.
 * Extends Character with XP rewards and a loot table dropped on death.
 */
class Enemy : public Character {
protected:
    int m_xpReward;
    std::vector<std::unique_ptr<Item>> m_lootTable;

public:
    Enemy(std::string name, int health, int attackPower, int defense, int level = 1, int xpReward = 20);
    virtual ~Enemy() = default;

    int getXpReward() const { return m_xpReward; }

    /**
     * @brief Transfers and returns all loot drops from this enemy (consumed on call).
     */
    std::vector<std::unique_ptr<Item>> takeLoot();

    bool hasLoot() const { return !m_lootTable.empty(); }
};

#endif // ENEMY_H
