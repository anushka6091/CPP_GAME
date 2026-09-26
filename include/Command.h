#ifndef COMMAND_H
#define COMMAND_H

#include <string>

/**
 * @brief Abstract Base Class representing an executable action in the game.
 * 
 * DESIGN PATTERN: Command Pattern (Command Interface)
 * WHY: Command encapsulates a request as an object, decoupling the input source 
 * (CommandParser / GameEngine UI) from the execution logic and target objects 
 * (Player, Room, Inventory, Enemy). This enables action queuing, logging, and 
 * replay "almost for free".
 */
class Command {
public:
    virtual ~Command() = default;

    /**
     * @brief Executes the command action.
     * @return true if the command succeeded and produced an action worth recording; false otherwise.
     */
    virtual bool execute() = 0;

    /**
     * @brief Returns a human-readable description of the command for logging and replay.
     */
    virtual std::string description() const = 0;
};

#endif // COMMAND_H
