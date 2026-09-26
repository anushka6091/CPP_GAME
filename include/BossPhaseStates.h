#ifndef BOSSPHASESTATES_H
#define BOSSPHASESTATES_H

#include "BossPhaseState.h"
#include <string>

/**
 * @brief Concrete Boss Phase 1: Standard combat mode.
 * Standard melee claw attacks and occasional dragon flame bursts.
 * Normal defense applies to incoming attacks.
 */
class Phase1State : public BossPhaseState {
public:
    Phase1State() = default;

    void attack(Boss& self, Character& target) override;
    void specialAbility(Boss& self, Character& target) override;
    int modifyIncomingDamage(Boss& self, int rawDamage) const override;
    std::string getName() const override { return "Phase 1 (Normal)"; }
};

/**
 * @brief Concrete Boss Phase 2: Reinforced defense & ally summoner.
 * Triggers when boss health drops below 50%.
 * Summons a Goblin Assassin ally who assists with strikes and poison attacks.
 * Boss scale armor hardens, becoming resistant to normal attacks (-50% damage taken).
 */
class Phase2State : public BossPhaseState {
private:
    bool m_hasSummonedAlly;
    std::string m_allyName;
    int m_allyAttackPower;

public:
    Phase2State();

    void onEnterPhase(Boss& self) override;
    void attack(Boss& self, Character& target) override;
    void specialAbility(Boss& self, Character& target) override;
    int modifyIncomingDamage(Boss& self, int rawDamage) const override;
    std::string getName() const override { return "Phase 2 (Reinforced & Goblin Ally Summoned)"; }

    bool hasSummonedAlly() const { return m_hasSummonedAlly; }
};

/**
 * @brief Concrete Boss Phase 3: Enraged Berserk state.
 * Triggers when boss health drops below 15%.
 * Boss enters an all-out blood frenzy with devastating attack power (1.8x multiplier).
 * Defense drops to 0, leaving the Boss completely unshielded and vulnerable to counter-attacks.
 */
class EnrageState : public BossPhaseState {
public:
    EnrageState() = default;

    void onEnterPhase(Boss& self) override;
    void attack(Boss& self, Character& target) override;
    void specialAbility(Boss& self, Character& target) override;
    int modifyIncomingDamage(Boss& self, int rawDamage) const override;
    std::string getName() const override { return "Enrage (Frenzy - 0 Defense / Maximum Power)"; }
};

/**
 * @brief Concrete Boss Phase 3 (NG+ Exclusive Extra Phase): Ascended Dragon Overlord.
 * Triggers below 35% HP in New Game+ mode.
 * Combines hardened scale resilience with apocalyptic Dragon Firestorm bursts.
 */
class AscendedState : public BossPhaseState {
public:
    AscendedState() = default;

    void onEnterPhase(Boss& self) override;
    void attack(Boss& self, Character& target) override;
    void specialAbility(Boss& self, Character& target) override;
    int modifyIncomingDamage(Boss& self, int rawDamage) const override;
    std::string getName() const override { return "Phase 3 [NG+ Ascended Overlord]"; }
};

#endif // BOSSPHASESTATES_H
