#ifndef CHARACTERSTATES_H
#define CHARACTERSTATES_H

#include "CharacterState.h"
#include <string>

/**
 * @brief Concrete State representing a normal, healthy character.
 */
class AliveState : public CharacterState {
public:
    bool canAct() const override { return true; }
    void onTurnStart(Character& c) override;
    std::string name() const override { return "Alive"; }
};

/**
 * @brief Concrete State representing a stunned character who loses turn actions.
 */
class StunnedState : public CharacterState {
private:
    int m_turnsRemaining;

public:
    explicit StunnedState(int durationTurns = 1);
    bool canAct() const override { return false; }
    void onTurnStart(Character& c) override;
    std::string name() const override { return "Stunned"; }
};

/**
 * @brief Concrete State representing a poisoned character taking periodic damage.
 */
class PoisonedState : public CharacterState {
private:
    int m_turnsRemaining;
    int m_poisonDamage;

public:
    PoisonedState(int durationTurns = 3, int damagePerTurn = 3);
    bool canAct() const override { return true; }
    void onTurnStart(Character& c) override;
    std::string name() const override { return "Poisoned"; }
};

/**
 * @brief Concrete Terminal State representing a deceased character.
 */
class DeadState : public CharacterState {
public:
    bool canAct() const override { return false; }
    void onTurnStart(Character& c) override;
    std::string name() const override { return "Dead"; }
};

#endif // CHARACTERSTATES_H
