#ifndef BOSSPHASESTATE_H
#define BOSSPHASESTATE_H

#include <string>

// Forward declarations
class Boss;
class Character;

/**
 * @brief Abstract Base Class representing a behavioral phase of a multi-phase Boss.
 * 
 * DESIGN PATTERN: State Pattern (Boss Behavioral State Interface)
 * WHY: BossPhaseState encapsulates phase-specific combat behavior (attack patterns, 
 * special abilities, damage resistance, and minion summoning) into dedicated state classes.
 * This keeps the Boss class clean and adheres to the Open-Closed Principle (new boss phases
 * can be added without modifying the core Boss entity or the combat loop).
 */
class BossPhaseState {
public:
    virtual ~BossPhaseState() = default;

    /**
     * @brief Executes the phase-specific primary attack logic.
     * @param self Reference to the Boss context.
     * @param target Reference to the attacked character (Player).
     */
    virtual void attack(Boss& self, Character& target) = 0;

    /**
     * @brief Executes the phase-specific special ability (spells, summons, frenzy).
     * @param self Reference to the Boss context.
     * @param target Reference to the targeted character (Player).
     */
    virtual void specialAbility(Boss& self, Character& target) = 0;

    /**
     * @brief Modifies incoming raw damage based on phase mechanics (e.g. resistance or vulnerability).
     * @param self Reference to the Boss context.
     * @param rawDamage The raw damage before phase modifiers.
     * @return The modified effective damage.
     */
    virtual int modifyIncomingDamage(Boss& self, int rawDamage) const {
        return rawDamage;
    }

    /**
     * @brief Optional lifecycle hook invoked when the Boss transitions into this phase.
     * @param self Reference to the Boss context.
     */
    virtual void onEnterPhase(Boss& self) {
        (void)self;
    }

    /**
     * @brief Returns the human-readable display name of the phase.
     */
    virtual std::string getName() const = 0;
};

#endif // BOSSPHASESTATE_H
