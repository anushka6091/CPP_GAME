#include "GameMemento.h"

// ==================== JSON Helper Functions ====================

static nlohmann::json itemToJson(const ItemMemento& item) {
    nlohmann::json j;
    j["name"] = item.name;
    j["type"] = item.type;
    j["description"] = item.description;
    j["healAmount"] = item.healAmount;
    j["damageBonus"] = item.damageBonus;

    if (item.type == "Container" && !item.containerContents.empty()) {
        nlohmann::json children = nlohmann::json::array();
        for (const auto& child : item.containerContents) {
            children.push_back(itemToJson(child));
        }
        j["containerContents"] = children;
    }
    return j;
}

static ItemMemento itemFromJson(const nlohmann::json& j) {
    ItemMemento item;
    item.name = j.value("name", "Unknown Item");
    item.type = j.value("type", "Material");
    item.description = j.value("description", "");
    item.healAmount = j.value("healAmount", 0);
    item.damageBonus = j.value("damageBonus", 0);

    if (j.contains("containerContents") && j["containerContents"].is_array()) {
        for (const auto& childJson : j["containerContents"]) {
            item.containerContents.push_back(itemFromJson(childJson));
        }
    }
    return item;
}

static nlohmann::json playerToJson(const PlayerMemento& player) {
    nlohmann::json j;
    j["name"] = player.name;
    j["health"] = player.health;
    j["maxHealth"] = player.maxHealth;
    j["attackPower"] = player.attackPower;
    j["defense"] = player.defense;
    j["level"] = player.level;
    j["xp"] = player.xp;
    j["stateName"] = player.stateName;
    j["hasEquippedWeapon"] = player.hasEquippedWeapon;

    if (player.hasEquippedWeapon) {
        j["equippedWeapon"] = itemToJson(player.equippedWeapon);
    }

    nlohmann::json invArray = nlohmann::json::array();
    for (const auto& item : player.inventoryItems) {
        invArray.push_back(itemToJson(item));
    }
    j["inventoryItems"] = invArray;

    return j;
}

static PlayerMemento playerFromJson(const nlohmann::json& j) {
    PlayerMemento player;
    player.name = j.value("name", "Hero");
    player.health = j.value("health", 30);
    player.maxHealth = j.value("maxHealth", 30);
    player.attackPower = j.value("attackPower", 8);
    player.defense = j.value("defense", 2);
    player.level = j.value("level", 1);
    player.xp = j.value("xp", 0);
    player.stateName = j.value("stateName", "Alive");
    player.hasEquippedWeapon = j.value("hasEquippedWeapon", false);

    if (player.hasEquippedWeapon && j.contains("equippedWeapon")) {
        player.equippedWeapon = itemFromJson(j["equippedWeapon"]);
    }

    if (j.contains("inventoryItems") && j["inventoryItems"].is_array()) {
        for (const auto& itemJson : j["inventoryItems"]) {
            player.inventoryItems.push_back(itemFromJson(itemJson));
        }
    }
    return player;
}

static nlohmann::json questToJson(const QuestMemento& quest) {
    nlohmann::json j;
    j["name"] = quest.name;
    j["state"] = quest.state;
    j["currentProgress"] = quest.currentProgress;
    j["requiredProgress"] = quest.requiredProgress;
    return j;
}

static QuestMemento questFromJson(const nlohmann::json& j) {
    QuestMemento quest;
    quest.name = j.value("name", "");
    quest.state = j.value("state", "InProgress");
    quest.currentProgress = j.value("currentProgress", 0);
    quest.requiredProgress = j.value("requiredProgress", 0);
    return quest;
}

// ==================== GameMemento Methods ====================

nlohmann::json GameMemento::toJson() const {
    nlohmann::json j;
    j["playerName"] = playerName;
    j["dungeonSeed"] = dungeonSeed;
    j["difficultyLevel"] = difficultyLevel;
    j["difficultyName"] = difficultyName;
    j["currentRoomDescription"] = currentRoomDescription;
    j["turnCount"] = turnCount;
    j["enemiesKilled"] = enemiesKilled;
    j["timestamp"] = timestamp;

    j["player"] = playerToJson(player);

    nlohmann::json questArray = nlohmann::json::array();
    for (const auto& q : quests) {
        questArray.push_back(questToJson(q));
    }
    j["quests"] = questArray;

    return j;
}

GameMemento GameMemento::fromJson(const nlohmann::json& j) {
    GameMemento memento;
    memento.playerName = j.value("playerName", "Hero");
    memento.dungeonSeed = j.value("dungeonSeed", 42);
    memento.difficultyLevel = j.value("difficultyLevel", 1);
    memento.difficultyName = j.value("difficultyName", "Normal");
    memento.currentRoomDescription = j.value("currentRoomDescription", "");
    memento.turnCount = j.value("turnCount", 0);
    memento.enemiesKilled = j.value("enemiesKilled", 0);
    memento.timestamp = j.value("timestamp", "");

    if (j.contains("player")) {
        memento.player = playerFromJson(j["player"]);
    }

    if (j.contains("quests") && j["quests"].is_array()) {
        for (const auto& qJson : j["quests"]) {
            memento.quests.push_back(questFromJson(qJson));
        }
    }

    return memento;
}
