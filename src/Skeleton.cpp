#include "Skeleton.h"
#include "CharacterStates.h"
#include "Item.h"
#include <iostream>


Skeleton::Skeleton(const std::string& name, int health, int attackPower, int defense)
    : Enemy(name, health, attackPower, defense, 1, 30), m_hasReassembled(false) {
    // Skeleton loot table: drops Iron Ore and Venom Sac
    m_lootTable.push_back(std::make_unique<Item>(
        "Iron Ore", ItemType::Material,
        "A dense chunk of raw iron ore, useful for forging weapons.", 0, 0));
    m_lootTable.push_back(std::make_unique<Item>(
        "Venom Sac", ItemType::Material,
        "A fragile sac collected from a skeletal creature, filled with residual venom.", 0, 0));
}


void Skeleton::attack(Character& target) {
    std::cout << m_name << " thrusts a bone spear at " << target.getName() << "!\n";
    target.takeDamage(m_attackPower);

    // 50% chance to deliver a stunning shield bash
    if (target.isAlive() && (rand() % 2 == 0)) {
        std::cout << ">>> CRITICAL IMPACT: " << m_name << " delivers a heavy Shield Bash, stunning " 
                  << target.getName() << "! <<<\n";
        target.transitionTo(std::make_unique<StunnedState>(1));
    }
}

void Skeleton::specialAbility(Character* target) {
    if (!m_hasReassembled) {
        m_hasReassembled = true;
        m_health = 1;
        std::cout << ">>> SPECIAL ABILITY TRIGGERED: " << m_name 
                  << "'s bones rattle together and reassemble! (Survives with 1 HP) <<<\n";
    }
}

void Skeleton::takeDamage(int amount) {
    int effectiveDamage = std::max(1, amount - m_defense);
    if (m_health - effectiveDamage <= 0 && !m_hasReassembled) {
        std::cout << m_name << " takes a fatal blow of " << effectiveDamage << " damage!\n";
        m_health = 0;
        specialAbility();
    } else {
        Character::takeDamage(amount);
    }
}
