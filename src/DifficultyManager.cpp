#include "DifficultyManager.h"
#include "Enemy.h"
#include "Boss.h"
#include "Item.h"
#include "Container.h"
#include <algorithm>
#include <iostream>

std::string difficultyToString(DifficultyLevel level) {
    switch (level) {
        case DifficultyLevel::Normal:      return "Normal";
        case DifficultyLevel::Hard:        return "Hard";
        case DifficultyLevel::NewGamePlus: return "NewGamePlus";
        default:                           return "Normal";
    }
}

DifficultyLevel stringToDifficulty(const std::string& str) {
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "hard" || s == "2") return DifficultyLevel::Hard;
    if (s == "newgameplus" || s == "ng+" || s == "ngplus" || s == "3") return DifficultyLevel::NewGamePlus;
    return DifficultyLevel::Normal;
}

double DifficultyManager::getStatMultiplier(DifficultyLevel level) {
    switch (level) {
        case DifficultyLevel::Normal:      return 1.0;
        case DifficultyLevel::Hard:        return 1.5;
        case DifficultyLevel::NewGamePlus: return 2.0;
        default:                           return 1.0;
    }
}

int DifficultyManager::toNumericLevel(DifficultyLevel level) {
    switch (level) {
        case DifficultyLevel::Normal:      return 1;
        case DifficultyLevel::Hard:        return 3;
        case DifficultyLevel::NewGamePlus: return 5;
        default:                           return 1;
    }
}

void DifficultyManager::applyModifiers(Dungeon& dungeon, DifficultyLevel level) {
    if (level == DifficultyLevel::Normal) {
        return; // Standard baseline generation
    }

    double statMult = getStatMultiplier(level);
    std::cout << "[DifficultyManager] Scaling dungeon encounters to [" 
              << difficultyToString(level) << "] (" << statMult << "x enemy stats";
    if (level == DifficultyLevel::NewGamePlus) {
        std::cout << " + Mythic Rarer Loot Pool + Extra Boss Phase Threshold";
    }
    std::cout << ")...\n";

    int roomIndex = 0;
    for (const auto& roomPtr : dungeon.getRooms()) {
        roomIndex++;
        Room* r = roomPtr.get();
        if (!r) continue;

        // 1. Scale Enemy Stats
        if (r->getEnemy()) {
            Enemy* enemy = r->getEnemy();
            enemy->scaleStats(statMult);

            // Configure Boss for NG+ extra phase threshold
            if (Boss* boss = dynamic_cast<Boss*>(enemy)) {
                if (level == DifficultyLevel::NewGamePlus) {
                    boss->enableNewGamePlusMode(true);
                }
            }
        }

        // 2. Enhance Loot Quality & Rarer Loot Pool
        if (level == DifficultyLevel::Hard) {
            // Enhanced consumables in every 3rd intermediate room
            if (roomIndex % 3 == 0 && !r->isBossRoom()) {
                r->addItem(std::make_unique<Item>(
                    "Refined Healing Draught", ItemType::Potion,
                    "An enriched alchemy brew.", 35, 0));
            }
        } else if (level == DifficultyLevel::NewGamePlus) {
            // Rarer loot pool in NG+: Mythic weapons, potions, and dragonite materials
            if (roomIndex == 2 && !r->isBossRoom()) {
                r->addItem(std::make_unique<Item>(
                    "Astral Rune Blade", ItemType::Weapon,
                    "A mythic sword forged from fallen star fragments.", 0, 24));
            } else if (roomIndex == 4 && !r->isBossRoom()) {
                r->addItem(std::make_unique<Item>(
                    "Mythic Dragon Elixir", ItemType::Potion,
                    "Restores 60 HP and revitalizes the hero.", 60, 0));
            } else if (roomIndex == 6 && !r->isBossRoom()) {
                r->addItem(std::make_unique<Item>(
                    "Pure Dragonite Ingot", ItemType::Material,
                    "A legendary iridescent alloy infused with ancient magic.", 0, 0));
            }
        }
    }
}
