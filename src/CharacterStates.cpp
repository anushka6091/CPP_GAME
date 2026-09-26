#include "CharacterStates.h"
#include "Character.h"
#include <iostream>

// ==================== AliveState ====================
void AliveState::onTurnStart(Character& c) {
    // Normal healthy state - no turn start penalty or damage
}

// ==================== StunnedState ====================
StunnedState::StunnedState(int durationTurns)
    : m_turnsRemaining(durationTurns) {}

void StunnedState::onTurnStart(Character& c) {
    std::cout << "\n[State Effect: Stunned] " << c.getName() 
              << " is dazed and cannot act this turn!\n";
    
    --m_turnsRemaining;
    if (m_turnsRemaining <= 0) {
        std::cout << "[State Transition] " << c.getName() << " recovered from being Stunned!\n";
        c.transitionTo(std::make_unique<AliveState>());
    }
}

// ==================== PoisonedState ====================
PoisonedState::PoisonedState(int durationTurns, int damagePerTurn)
    : m_turnsRemaining(durationTurns), m_poisonDamage(damagePerTurn) {}

void PoisonedState::onTurnStart(Character& c) {
    std::cout << "\n[State Effect: Poisoned] " << c.getName() 
              << " suffers " << m_poisonDamage << " poison damage!\n";
    
    c.takeDamage(m_poisonDamage);
    --m_turnsRemaining;

    if (c.isAlive() && m_turnsRemaining <= 0) {
        std::cout << "[State Transition] " << c.getName() << "'s poison effect wore off!\n";
        c.transitionTo(std::make_unique<AliveState>());
    }
}

// ==================== DeadState ====================
void DeadState::onTurnStart(Character& c) {
    std::cout << "\n[State Effect: Dead] " << c.getName() << " is deceased.\n";
}
