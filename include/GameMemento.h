#ifndef GAMEMEMENTO_H
#define GAMEMEMENTO_H

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

/**
 * @brief Serialized representation of a single Item or composite Container.
 */
struct ItemMemento {
    std::string name;
    std::string type; // "Potion", "Weapon", "Key", "Material", "Container"
    std::string description;
    int healAmount = 0;
    int damageBonus = 0;
    std::vector<ItemMemento> containerContents; // Recursive composite contents
};

/**
 * @brief Serialized representation of Player stats, condition, and inventory.
 */
struct PlayerMemento {
    std::string name;
    int health = 0;
    int maxHealth = 0;
    int attackPower = 0;
    int defense = 0;
    int level = 1;
    int xp = 0;
    std::string stateName = "Alive";
    bool hasEquippedWeapon = false;
    ItemMemento equippedWeapon;
    std::vector<ItemMemento> inventoryItems;
};

/**
 * @brief Serialized representation of an Observer Quest's status and progress.
 */
struct QuestMemento {
    std::string name;
    std::string state; // "NotStarted", "InProgress", "Completed"
    int currentProgress = 0;
    int requiredProgress = 0;
};

/**
 * @brief Full snapshot of the game session state.
 * 
 * DESIGN PATTERN: Memento Pattern (Memento Object)
 * WHY: GameMemento captures the complete internal state of Player, Dungeon,
 * Quests, and Engine without violating encapsulation or exposing private
 * fields directly to external serialization or database logic.
 */
class GameMemento {
public:
    std::string playerName;
    int dungeonSeed = 0;
    int difficultyLevel = 1;
    std::string difficultyName = "Normal";
    std::string currentRoomDescription;
    int turnCount = 0;
    int enemiesKilled = 0;
    std::string timestamp;

    PlayerMemento player;
    std::vector<QuestMemento> quests;

    // JSON Serialization / Deserialization
    nlohmann::json toJson() const;
    static GameMemento fromJson(const nlohmann::json& j);
};

#endif // GAMEMEMENTO_H
