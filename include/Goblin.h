#ifndef GOBLIN_H
#define GOBLIN_H

#include "Enemy.h"

/**
 * @brief Concrete class representing a Goblin enemy.
 * 
 * DESIGN PATTERN: Subtype Polymorphism / Concrete Strategy Implementation
 * WHY: Goblin defines weak melee attack behavior and a no-op special ability.
 */
class Goblin : public Enemy {
public:
    Goblin(const std::string& name = "Goblin Scavenger", int health = 15, int attackPower = 5, int defense = 1);

    void attack(Character& target) override;
    void specialAbility(Character* target = nullptr) override;
};

#endif // GOBLIN_H
