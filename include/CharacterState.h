#ifndef CHARACTERSTATE_H
#define CHARACTERSTATE_H

#include <string>

// Forward declaration
class Character;

/**
 * @brief Abstract Base Class representing a character condition/state.
 * 
 * DESIGN PATTERN: State Pattern (State Interface)
 * WHY: CharacterState encapsulates state-dependent behaviors (canAct, onTurnStart, state transitions)
 * into concrete state objects. This eliminates scattered boolean flags (isPoisoned, isStunned, isDead)
 * and complex conditional branching across the Character codebase.
 */
class CharacterState {
public:
    virtual ~CharacterState() = default;

    /**
     * @brief Determines if the character can take actions during their turn.
     */
    virtual bool canAct() const = 0;

    /**
     * @brief Lifecycle hook executed at the beginning of the character's turn.
     * @param c Reference to the Character currently in this state.
     */
    virtual void onTurnStart(Character& c) = 0;

    /**
     * @brief Returns the human-readable name of the state.
     */
    virtual std::string name() const = 0;
};

#endif // CHARACTERSTATE_H
