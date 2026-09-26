#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include "Command.h"
#include "ConcreteCommands.h"
#include "Player.h"
#include "Room.h"
#include "CommandHistory.h"
#include "EventBus.h"
#include "Quest.h"
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Factory/Parser class that converts raw console string input into executable Command objects.
 * 
 * DESIGN PATTERN: Factory Method / Interpreter for Commands
 */
class CommandParser {
public:
    CommandParser() = delete;

    /**
     * @brief Parses a raw user input string into a concrete Command object.
     */
    static std::unique_ptr<Command> parse(
        const std::string& rawInput, 
        Player& player, 
        Room*& currentRoom, 
        const CommandHistory& history,
        EventBus* eventBus = nullptr,
        const Quest* requiredQuest = nullptr,
        const std::vector<std::unique_ptr<Quest>>& quests = {},
        CraftingStation* craftingStation = nullptr,
        GameEngine* engine = nullptr,
        SaveManager* saveManager = nullptr
    );
};

#endif // COMMANDPARSER_H
