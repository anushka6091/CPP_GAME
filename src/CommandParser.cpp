#include "CommandParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

std::unique_ptr<Command> CommandParser::parse(
    const std::string& rawInput, 
    Player& player, 
    Room*& currentRoom, 
    const CommandHistory& history,
    EventBus* eventBus,
    const Quest* requiredQuest,
    const std::vector<std::unique_ptr<Quest>>& quests,
    CraftingStation* craftingStation
) {
    if (!currentRoom) return nullptr;

    std::string trimmed = rawInput;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
    trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);

    if (trimmed.empty()) {
        return nullptr;
    }

    std::stringstream ss(trimmed);
    std::string action;
    ss >> action;
    std::transform(action.begin(), action.end(), action.begin(), ::tolower);

    std::string targetName;
    std::getline(ss, targetName);
    if (!targetName.empty()) {
        targetName.erase(0, targetName.find_first_not_of(" \t\r\n"));
        targetName.erase(targetName.find_last_not_of(" \t\r\n") + 1);
    }

    if (action == "attack" || action == "fight") {
        return std::make_unique<AttackCommand>(player, *currentRoom, eventBus);
    } 
    else if (action == "use") {
        if (targetName.empty()) {
            std::cout << "[CommandParser] Error: Usage is 'use <item name>'\n";
            return nullptr;
        }
        return std::make_unique<UseItemCommand>(player, targetName);
    }
    else if (action == "equip") {
        if (targetName.empty()) {
            std::cout << "[CommandParser] Error: Usage is 'equip <weapon name>'\n";
            return nullptr;
        }
        return std::make_unique<EquipCommand>(player, targetName);
    }
    else if (action == "move" || action == "go") {
        if (targetName.empty()) targetName = "north";
        return std::make_unique<MoveCommand>(currentRoom, targetName, eventBus, requiredQuest);
    }
    else if (action == "north" || action == "south" || action == "east" || action == "west" ||
             action == "n" || action == "s" || action == "e" || action == "w") {
        return std::make_unique<MoveCommand>(currentRoom, action, eventBus, requiredQuest);
    }
    else if (action == "flee" || action == "run" || action == "escape") {
        return std::make_unique<FleeCommand>(player, *currentRoom);
    }
    else if (action == "look" || action == "inspect") {
        return std::make_unique<LookCommand>(*currentRoom);
    }
    else if (action == "pickup" || action == "take" || (action == "pick" && targetName.rfind("up", 0) == 0)) {
        if (action == "pick" && targetName.rfind("up", 0) == 0) {
            targetName = targetName.substr(2);
            targetName.erase(0, targetName.find_first_not_of(" \t\r\n"));
        }
        if (targetName.empty()) {
            std::cout << "[CommandParser] Error: Usage is 'pickup <item name>'\n";
            return nullptr;
        }
        return std::make_unique<PickupCommand>(player, *currentRoom, targetName, eventBus);
    }
    else if (action == "inventory" || action == "inv") {
        return std::make_unique<InventoryCommand>(player);
    }
    else if (action == "quests" || action == "quest" || action == "objectives") {
        return std::make_unique<QuestsCommand>(quests);
    }
    else if ((action == "craft") && craftingStation) {
        if (targetName.empty()) {
            std::cout << "[CommandParser] Error: Usage is 'craft <recipe name>'\n";
            return nullptr;
        }
        return std::make_unique<CraftCommand>(player, *craftingStation, targetName);
    }
    else if ((action == "recipes" || action == "crafts") && craftingStation) {
        return std::make_unique<RecipesCommand>(*craftingStation);
    }
    else if (action == "replay" || action == "history") {
        return std::make_unique<ReplayCommand>(history);
    }

    std::cout << "[CommandParser] Error: Unknown command '" << action << "'. Type 'help' for command list.\n";
    return nullptr;
}
