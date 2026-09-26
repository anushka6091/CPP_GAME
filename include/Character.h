#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <memory>
#include "CharacterState.h"

/**
 * @brief Abstract Base Class representing any entity in the game (Player or Enemy).
 * 
 * DESIGN PATTERN: State Pattern Context & Subtype Polymorphism
 * WHY: Character delegates state-dependent behavior (canAct, turn initialization, death handling)
 * to its m_state object. Equipping state objects dynamically handles condition lifecycles.
 */
class Character {
protected:
    std::string m_name;
    int m_health;
    int m_maxHealth;
    int m_attackPower;
    int m_defense;
    int m_level;
    int m_xp;
    std::unique_ptr<CharacterState> m_state;

public:
    Character(std::string name, int health, int attackPower, int defense, int level = 1, int xp = 0);
    virtual ~Character() = default;

    // Pure virtual functions defining combat contract
    virtual void attack(Character& target) = 0;
    virtual void specialAbility(Character* target = nullptr) = 0;

    // State Pattern Methods
    void transitionTo(std::unique_ptr<CharacterState> newState);
    void onTurnStart();
    bool canAct() const;
    std::string getStateName() const;
    CharacterState* getState() const { return m_state.get(); }

    // Combat methods
    virtual void takeDamage(int amount);
    virtual bool isAlive() const;

    // Getters & Setters
    std::string getName() const { return m_name; }
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    int getAttackPower() const { return m_attackPower; }
    int getDefense() const { return m_defense; }
    int getLevel() const { return m_level; }
    int getXp() const { return m_xp; }

    void setHealth(int health) { m_health = health; }
    void setMaxHealth(int maxHp) { m_maxHealth = maxHp; m_health = maxHp; }
    void setAttackPower(int atk) { m_attackPower = atk; }
    void setDefense(int def) { m_defense = def; }
    void addXp(int amount) { m_xp += amount; }

    void scaleStats(double multiplier) {
        m_maxHealth = static_cast<int>(m_maxHealth * multiplier);
        m_health = m_maxHealth;
        m_attackPower = static_cast<int>(m_attackPower * multiplier);
        m_defense = static_cast<int>(m_defense * multiplier);
    }
};

#endif // CHARACTER_H
