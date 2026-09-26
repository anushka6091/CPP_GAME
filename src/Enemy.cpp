#include "Enemy.h"

Enemy::Enemy(std::string name, int health, int attackPower, int defense, int level, int xpReward)
    : Character(std::move(name), health, attackPower, defense, level, 0), m_xpReward(xpReward) {}

std::vector<std::unique_ptr<Item>> Enemy::takeLoot() {
    return std::move(m_lootTable);
}
