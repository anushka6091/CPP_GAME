#include "ConcreteCommands.h"
#include "CommandHistory.h"
#include "Enemy.h"
#include <iostream>

// ==================== AttackCommand ====================
AttackCommand::AttackCommand(Player& player, Room& room, EventBus* eventBus)
    : m_player(player), m_room(room), m_eventBus(eventBus) {}

bool AttackCommand::execute() {
    if (!m_room.hasEnemy()) {
        std::cout << "[Combat] There are no active enemies in this room to attack.\n";
        return false;
    }

    Enemy* enemy = m_room.getEnemy();
    std::string enemyName = enemy->getName();

    // 1. Player Turn Initialization (State pattern onTurnStart)
    m_player.onTurnStart();
    if (!m_player.isAlive()) {
        std::cout << "[Combat] Player has perished!\n";
        return true;
    }

    if (!m_player.canAct()) {
        std::cout << "[Turn Skipped] " << m_player.getName() 
                  << " cannot act this turn due to condition [" << m_player.getStateName() << "]!\n";
    } else {
        std::cout << "\n[Command: Attack] " << m_player.getName() << " attacks " << enemyName << "!\n";
        m_player.attack(*enemy);
    }

    // Check if enemy died
    if (!enemy->isAlive()) {
        std::cout << "\n*** VICTORY! You defeated " << enemyName << "! ***\n";
        m_player.addXp(enemy->getXpReward());
        std::cout << "Gained " << enemy->getXpReward() << " XP! Total XP: " << m_player.getXp() << "\n";

        // Drop enemy loot into the current room
        auto loot = enemy->takeLoot();
        if (!loot.empty()) {
            std::cout << "[Loot Drop] " << enemyName << " dropped:\n";
            for (auto& drop : loot) {
                std::cout << "  + " << drop->getName() << " (" << Item::itemTypeToString(drop->getType()) << ")\n";
                m_room.addItem(std::move(drop));
            }
        }

        // Publish EnemyKilled Event over EventBus
        if (m_eventBus) {
            m_eventBus->publish(GameEvent(GameEventType::EnemyKilled, enemyName, 1));
        }

        return true;
    }

    // 2. Enemy Turn Initialization (State pattern onTurnStart)
    std::cout << "\n--- Enemy's Turn ---\n";
    enemy->onTurnStart();

    if (!enemy->isAlive()) {
        std::cout << "\n*** VICTORY! Enemy succumbed to state condition! ***\n";
        m_player.addXp(enemy->getXpReward());

        if (m_eventBus) {
            m_eventBus->publish(GameEvent(GameEventType::EnemyKilled, enemyName, 1));
        }

        return true;
    }

    if (enemy->canAct()) {
        enemy->attack(m_player);
        if (!m_player.isAlive()) {
            std::cout << "\n*** DEFEAT! You have been slain in combat... ***\n";
        }
    } else {
        std::cout << "[Turn Skipped] " << enemy->getName() 
                  << " cannot act this turn due to condition [" << enemy->getStateName() << "]!\n";
    }

    return true;
}

std::string AttackCommand::description() const {
    if (m_room.hasEnemy()) {
        return "Attacked " + m_room.getEnemy()->getName();
    }
    return "Attempted attack (No enemy present)";
}

// ==================== UseItemCommand ====================
UseItemCommand::UseItemCommand(Player& player, std::string itemName)
    : m_player(player), m_itemName(std::move(itemName)) {}

bool UseItemCommand::execute() {
    std::cout << "\n[Command: Use Item] Attempting to use '" << m_itemName << "'...\n";
    return m_player.getInventory().useItem(m_itemName, m_player);
}

std::string UseItemCommand::description() const {
    return "Used item '" + m_itemName + "'";
}

// ==================== EquipCommand ====================
EquipCommand::EquipCommand(Player& player, std::string weaponName)
    : m_player(player), m_weaponName(std::move(weaponName)) {}

bool EquipCommand::execute() {
    std::cout << "\n[Command: Equip Weapon] Attempting to equip '" << m_weaponName << "'...\n";
    return m_player.getInventory().useItem(m_weaponName, m_player);
}

std::string EquipCommand::description() const {
    return "Equipped weapon '" + m_weaponName + "'";
}

// ==================== MoveCommand ====================
MoveCommand::MoveCommand(Room*& currentRoomRef, std::string directionStr, EventBus* eventBus, const Quest* requiredQuest)
    : m_currentRoomRef(currentRoomRef), m_directionStr(std::move(directionStr)), m_eventBus(eventBus), m_requiredQuest(requiredQuest) {}

bool MoveCommand::execute() {
    Direction dir = stringToDirection(m_directionStr);
    Room* nextRoom = m_currentRoomRef ? m_currentRoomRef->getExit(dir) : nullptr;

    if (!nextRoom) {
        std::cout << "[Command: Move] Cannot move " << directionToString(dir) 
                  << "! No exit exists in that direction.\n";
        return false;
    }

    // Boss Room Quest Gate Check
    if (nextRoom->isBossRoom()) {
        if (m_requiredQuest && m_requiredQuest->getState() != QuestState::Completed) {
            std::cout << "\n========================================================================\n";
            std::cout << "[QUEST GATE] The heavy Boss Room gate is locked by ancient magic!\n";
            std::cout << "You must complete the required objective first: '" << m_requiredQuest->getName() << "'\n";
            std::cout << "Objective: " << m_requiredQuest->getProgressString() << "\n";
            std::cout << "========================================================================\n";
            return false;
        }
    }

    std::cout << "\n[Command: Move] Moving " << directionToString(dir) << "...\n";
    m_currentRoomRef = nextRoom;
    m_currentRoomRef->look();

    // Publish RoomEntered Event
    if (m_eventBus) {
        m_eventBus->publish(GameEvent(GameEventType::RoomEntered, m_currentRoomRef->getDescription(), 1));
    }

    return true;
}

std::string MoveCommand::description() const {
    return "Moved " + m_directionStr;
}

// ==================== FleeCommand ====================
FleeCommand::FleeCommand(Player& player, Room& room)
    : m_player(player), m_room(room) {}

bool FleeCommand::execute() {
    if (!m_room.hasEnemy()) {
        std::cout << "[Flee] No enemies to flee from in this room.\n";
        return false;
    }

    std::cout << "\n[Command: Flee] " << m_player.getName() << " turns and flees from " 
              << m_room.getEnemy()->getName() << "!\n";
    
    int fleeDamage = 3;
    std::cout << m_room.getEnemy()->getName() << " strikes as you retreat, dealing " 
              << fleeDamage << " opportunity damage!\n";
    m_player.takeDamage(fleeDamage);
    return true;
}

std::string FleeCommand::description() const {
    return "Fleed from combat in room";
}

// ==================== LookCommand ====================
LookCommand::LookCommand(const Room& room)
    : m_room(room) {}

bool LookCommand::execute() {
    m_room.look();
    return true;
}

std::string LookCommand::description() const {
    return "Looked around room";
}

// ==================== PickupCommand ====================
PickupCommand::PickupCommand(Player& player, Room& room, std::string itemName, EventBus* eventBus)
    : m_player(player), m_room(room), m_itemName(std::move(itemName)), m_eventBus(eventBus) {}

bool PickupCommand::execute() {
    auto item = m_room.removeItem(m_itemName);
    if (item) {
        std::string actualName = item->getName();
        m_player.getInventory().add(std::move(item));

        // Publish ItemCollected Event over EventBus
        if (m_eventBus) {
            m_eventBus->publish(GameEvent(GameEventType::ItemCollected, actualName, 1));
        }

        return true;
    }
    std::cout << "No item named '" << m_itemName << "' found in room or containers.\n";
    return false;
}

std::string PickupCommand::description() const {
    return "Picked up '" + m_itemName + "'";
}

// ==================== InventoryCommand ====================
InventoryCommand::InventoryCommand(const Player& player)
    : m_player(player) {}

bool InventoryCommand::execute() {
    m_player.getInventory().listItems();
    return true;
}

std::string InventoryCommand::description() const {
    return "Checked inventory";
}

// ==================== ReplayCommand ====================
ReplayCommand::ReplayCommand(const CommandHistory& history)
    : m_history(history) {}

bool ReplayCommand::execute() {
    m_history.replay();
    return false;
}

std::string ReplayCommand::description() const {
    return "Triggered Command History Replay";
}

// ==================== QuestsCommand ====================
QuestsCommand::QuestsCommand(const std::vector<std::unique_ptr<Quest>>& quests)
    : m_quests(quests) {}

bool QuestsCommand::execute() {
    std::cout << "\n====================================================\n";
    std::cout << "                ACTIVE QUEST LOG                    \n";
    std::cout << "====================================================\n";

    if (m_quests.empty()) {
        std::cout << "No active quests recorded.\n";
    } else {
        for (size_t i = 0; i < m_quests.size(); ++i) {
            std::cout << (i + 1) << ". " << m_quests[i]->getProgressString() << "\n";
        }
    }
    std::cout << "====================================================\n";
    return true;
}

std::string QuestsCommand::description() const {
    return "Checked quest log";
}

// ==================== CraftCommand ====================
CraftCommand::CraftCommand(Player& player, CraftingStation& station, std::string recipeName)
    : m_player(player), m_station(station), m_recipeName(std::move(recipeName)) {}

bool CraftCommand::execute() {
    std::cout << "\n[Command: Craft] Attempting to craft '" << m_recipeName << "'...\n";
    std::string error;
    bool success = m_station.craft(m_player.getInventory(), m_recipeName, error);
    if (!success) {
        std::cout << error << "\n";
    }
    return success;
}

std::string CraftCommand::description() const {
    return "Crafted '" + m_recipeName + "'";
}

// ==================== RecipesCommand ====================
RecipesCommand::RecipesCommand(const CraftingStation& station)
    : m_station(station) {}

bool RecipesCommand::execute() {
    m_station.listRecipes();
    return true;
}

std::string RecipesCommand::description() const {
    return "Listed crafting recipes";
}
