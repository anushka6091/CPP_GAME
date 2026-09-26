#include "Goblin.h"
#include "CharacterStates.h"
#include <iostream>

Goblin::Goblin(const std::string& name, int health, int attackPower, int defense)
    : Enemy(name, health, attackPower, defense, 1, 15) {
    // Goblin loot table: drops Healing Herb and/or Monster Fang
    m_lootTable.push_back(std::make_unique<Item>(
        "Healing Herb", ItemType::Material,
        "A fragrant herb with potent restorative properties.", 0, 0));
    m_lootTable.push_back(std::make_unique<Item>(
        "Monster Fang", ItemType::Material,
        "A sharp, curved fang torn from a defeated goblin.", 0, 0));
}

void Goblin::attack(Character& target) {
    std::cout << m_name << " slashes viciously with a venom-coated dagger at " << target.getName() << "!\n";
    target.takeDamage(m_attackPower);
    if (target.isAlive()) {
        specialAbility(&target);
    }
}

void Goblin::specialAbility(Character* target) {
    if (target && target->isAlive()) {
        std::cout << ">>> SPECIAL ABILITY: " << m_name 
                  << " inflicts a Poisonous Bite on " << target->getName() << "! <<<\n";
        target->transitionTo(std::make_unique<PoisonedState>(3, 4));
    }
}
