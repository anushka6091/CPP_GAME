#ifndef SKELETON_H
#define SKELETON_H

#include "Enemy.h"

/**
 * @brief Concrete class representing a Skeleton enemy.
 * 
 * DESIGN PATTERN: Specialization / Polymorphic Hook
 * WHY: Skeleton overrides takeDamage() to intercept fatal hits and trigger 
 * specialAbility() ("reassemble"), demonstrating polymorphic behavior 
 * specialized for unique enemy mechanics.
 */
class Skeleton : public Enemy {
private:
    bool m_hasReassembled;

public:
    Skeleton(const std::string& name = "Skeletal Warrior", int health = 20, int attackPower = 7, int defense = 2);

    void attack(Character& target) override;
    void specialAbility(Character* target = nullptr) override;
    void takeDamage(int amount) override;

    bool hasReassembled() const { return m_hasReassembled; }
};

#endif // SKELETON_H
