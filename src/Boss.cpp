#include "Boss.h"
#include "BossPhaseStates.h"
#include "CharacterStates.h"
#include "Item.h"
#include <iostream>
#include <algorithm>

Boss::Boss(std::string name, int health, int attackPower, int defense)
    : Enemy(std::move(name), health, attackPower, defense, 5, 100),
      m_baseDefense(defense),
      m_phaseState(std::make_unique<Phase1State>()) {
    
    // High-tier Boss Loot Table
    m_lootTable.push_back(std::make_unique<Item>(
        "Dragon Heart Gem", ItemType::Material,
        "A pulsing crimson gem brimming with primordial draconic energy.", 0, 0));
    m_lootTable.push_back(std::make_unique<Item>(
        "Overlord Crown", ItemType::Material,
        "An ancient golden crown recovered from the defeated dungeon ruler.", 0, 0));
}

void Boss::attack(Character& target) {
    if (m_phaseState) {
        m_phaseState->attack(*this, target);
    }
}

void Boss::specialAbility(Character* target) {
    if (m_phaseState && target) {
        m_phaseState->specialAbility(*this, *target);
    }
}

void Boss::takeDamage(int amount) {
    // 1. Let the current phase state calculate / modify effective damage
    int effectiveDamage = amount;
    if (m_phaseState) {
        effectiveDamage = m_phaseState->modifyIncomingDamage(*this, amount);
    } else {
        effectiveDamage = std::max(1, amount - m_defense);
    }

    m_health -= effectiveDamage;

    // Check for death
    if (m_health <= 0) {
        m_health = 0;
        std::cout << m_name << " takes " << effectiveDamage 
                  << " fatal damage! (Health: 0/" << m_maxHealth << ")\n";
        if (dynamic_cast<DeadState*>(m_state.get()) == nullptr) {
            transitionTo(std::make_unique<DeadState>());
        }
        return;
    }

    int healthPercent = (m_health * 100) / m_maxHealth;
    std::cout << m_name << " takes " << effectiveDamage << " damage! (Health remaining: " 
              << m_health << "/" << m_maxHealth << " [" << healthPercent << "%] - " 
              << getPhaseName() << ")\n";

    // 2. Health Threshold Milestones for Phase State Machine Transitions
    // Check Enrage threshold (< 15% HP)
    if (healthPercent < 15) {
        if (dynamic_cast<EnrageState*>(m_phaseState.get()) == nullptr) {
            transitionPhase(std::make_unique<EnrageState>());
        }
    }
    // In NewGamePlus, trigger the EXTRA BOSS PHASE threshold (< 35% HP)
    else if (m_isNewGamePlus && healthPercent < 35) {
        if (dynamic_cast<AscendedState*>(m_phaseState.get()) == nullptr &&
            dynamic_cast<EnrageState*>(m_phaseState.get()) == nullptr) {
            transitionPhase(std::make_unique<AscendedState>());
        }
    }
    // Check Phase 2 threshold (< 70% in NG+, < 50% in Normal/Hard)
    else if ((m_isNewGamePlus && healthPercent < 70) || (!m_isNewGamePlus && healthPercent < 50)) {
        if (dynamic_cast<Phase1State*>(m_phaseState.get()) != nullptr) {
            transitionPhase(std::make_unique<Phase2State>());
        }
    }
}

void Boss::transitionPhase(std::unique_ptr<BossPhaseState> newPhase) {
    if (newPhase) {
        m_phaseState = std::move(newPhase);
        m_phaseState->onEnterPhase(*this);
    }
}

std::string Boss::getPhaseName() const {
    return m_phaseState ? m_phaseState->getName() : "None";
}
