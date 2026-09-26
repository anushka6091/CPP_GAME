#ifndef CONCRETECOMMANDS_H
#define CONCRETECOMMANDS_H

#include "Command.h"
#include "Player.h"
#include "Room.h"
#include "EventBus.h"
#include "Quest.h"
#include "CraftingSystem.h"
#include <string>
#include <vector>


// Forward declaration for ReplayCommand
class CommandHistory;

/**
 * @brief Concrete Command for attacking an enemy in combat.
 */
class AttackCommand : public Command {
private:
    Player& m_player;
    Room& m_room;
    EventBus* m_eventBus;

public:
    AttackCommand(Player& player, Room& room, EventBus* eventBus = nullptr);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command for using/consuming an item from inventory.
 */
class UseItemCommand : public Command {
private:
    Player& m_player;
    std::string m_itemName;

public:
    UseItemCommand(Player& player, std::string itemName);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command for equipping a weapon from inventory.
 */
class EquipCommand : public Command {
private:
    Player& m_player;
    std::string m_weaponName;

public:
    EquipCommand(Player& player, std::string weaponName);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command for moving to a different room, publishing RoomEntered events and checking Boss Room Quest Gating.
 */
class MoveCommand : public Command {
private:
    Room*& m_currentRoomRef;
    std::string m_directionStr;
    EventBus* m_eventBus;
    const Quest* m_requiredQuest;

public:
    MoveCommand(Room*& currentRoomRef, std::string directionStr, EventBus* eventBus = nullptr, const Quest* requiredQuest = nullptr);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command for attempting to flee from hostiles.
 */
class FleeCommand : public Command {
private:
    Player& m_player;
    Room& m_room;

public:
    FleeCommand(Player& player, Room& room);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command for inspecting room surroundings and composite objects.
 */
class LookCommand : public Command {
private:
    const Room& m_room;

public:
    explicit LookCommand(const Room& room);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command for picking up an item from the room/containers and publishing ItemCollected events.
 */
class PickupCommand : public Command {
private:
    Player& m_player;
    Room& m_room;
    std::string m_itemName;
    EventBus* m_eventBus;

public:
    PickupCommand(Player& player, Room& room, std::string itemName, EventBus* eventBus = nullptr);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command for listing inventory contents.
 */
class InventoryCommand : public Command {
private:
    const Player& m_player;

public:
    explicit InventoryCommand(const Player& player);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command for displaying command history replay.
 */
class ReplayCommand : public Command {
private:
    const CommandHistory& m_history;

public:
    explicit ReplayCommand(const CommandHistory& history);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command for displaying active quests and progress.
 */
class QuestsCommand : public Command {
private:
    const std::vector<std::unique_ptr<Quest>>& m_quests;

public:
    explicit QuestsCommand(const std::vector<std::unique_ptr<Quest>>& quests);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command for crafting an item from the CraftingStation.
 * Slots into Command/CommandHistory with zero changes to those systems.
 */
class CraftCommand : public Command {
private:
    Player& m_player;
    CraftingStation& m_station;
    std::string m_recipeName;

public:
    CraftCommand(Player& player, CraftingStation& station, std::string recipeName);
    bool execute() override;
    std::string description() const override;
};

/**
 * @brief Concrete Command listing all available crafting recipes.
 */
class RecipesCommand : public Command {
private:
    const CraftingStation& m_station;

public:
    explicit RecipesCommand(const CraftingStation& station);
    bool execute() override;
    std::string description() const override;
};

#endif // CONCRETECOMMANDS_H
