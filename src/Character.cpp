#include "Character.h"
#include "CharacterStates.h"
#include <iostream>
#include <algorithm>

Character::Character(std::string name, int health, int attackPower, int defense, int level, int xp)
    : m_name(std::move(name)), m_health(health), m_maxHealth(health),
      m_attackPower(attackPower), m_defense(defense), m_level(level), m_xp(xp),
      m_state(std::make_unique<AliveState>()) {}

void Character::transitionTo(std::unique_ptr<CharacterState> newState) {
    if (newState) {
        std::cout << "[State Transition] " << m_name << " transitioned from ["
                  << (m_state ? m_state->name() : "None") << "] -> [" << newState->name() << "]\n";
        m_state = std::move(newState);
    }
}

void Character::onTurnStart() {
    if (m_state) {
        m_state->onTurnStart(*this);
    }
}

bool Character::canAct() const {
    return m_state ? m_state->canAct() : false;
}

std::string Character::getStateName() const {
    return m_state ? m_state->name() : "Unknown";
}

void Character::takeDamage(int amount) {
    int effectiveDamage = std::max(1, amount - m_defense);
    m_health -= effectiveDamage;
    if (m_health <= 0) {
        m_health = 0;
        std::cout << m_name << " takes " << effectiveDamage << " fatal damage! (Health: 0/" << m_maxHealth << ")\n";
        if (dynamic_cast<DeadState*>(m_state.get()) == nullptr) {
            transitionTo(std::make_unique<DeadState>());
        }
    } else {
        std::cout << m_name << " takes " << effectiveDamage << " damage! (Health remaining: " 
                  << m_health << "/" << m_maxHealth << ")\n";
    }
}

bool Character::isAlive() const {
    return m_health > 0 && dynamic_cast<DeadState*>(m_state.get()) == nullptr;
}
