#ifndef BOSS_H
#define BOSS_H

#include "Enemy.h"
#include "BossPhaseState.h"
#include <memory>
#include <string>

/**
 * @brief Concrete Enemy subclass representing a multi-phase dungeon Boss.
 * 
 * DESIGN PATTERN: State Pattern Context (Boss Phase State Machine)
 * WHY: Boss extends Enemy and delegates combat behavior and damage mitigation
 * to its dynamic m_phaseState object (Phase1State -> Phase2State -> EnrageState).
 * Phase transitions are evaluated automatically inside takeDamage() whenever
 * health thresholds are breached.
 */
class Boss : public Enemy {
private:
    std::unique_ptr<BossPhaseState> m_phaseState;
    int m_baseDefense;

public:
    Boss(
        std::string name = "Dungeon Dragon Overlord",
        int health = 70,
        int attackPower = 12,
        int defense = 4
    );

    // Character / Enemy Virtual Methods
    void attack(Character& target) override;
    void specialAbility(Character* target = nullptr) override;
    void takeDamage(int amount) override;

    // Phase State Machine Methods
    void transitionPhase(std::unique_ptr<BossPhaseState> newPhase);
    BossPhaseState* getPhaseState() const { return m_phaseState.get(); }
    std::string getPhaseName() const;

    int getBaseDefense() const { return m_baseDefense; }
    void setDefense(int def) { m_defense = def; }

    void enableNewGamePlusMode(bool enable = true) { m_isNewGamePlus = enable; }
    bool isNewGamePlus() const { return m_isNewGamePlus; }
private:
    bool m_isNewGamePlus{false};
};

#endif // BOSS_H
