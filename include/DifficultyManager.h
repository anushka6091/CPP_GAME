#ifndef DIFFICULTYMANAGER_H
#define DIFFICULTYMANAGER_H

#include "Dungeon.h"
#include <string>

/**
 * @brief Game difficulty tiers.
 */
enum class DifficultyLevel {
    Normal,
    Hard,
    NewGamePlus
};

std::string difficultyToString(DifficultyLevel level);
DifficultyLevel stringToDifficulty(const std::string& str);

/**
 * @brief Utility / Strategy manager applying difficulty scaling and loot enhancements.
 * 
 * DESIGN PATTERN: Strategy / Manager for World Scaling
 * WHY: DifficultyManager encapsulates difficulty mathematics and rules (stat scaling,
 * loot quality boosts, and NG+ boss extra phase thresholds) into a dedicated class,
 * avoiding scattered if-else statements across Dungeon and Monster logic.
 */
class DifficultyManager {
public:
    DifficultyManager() = delete;

    /**
     * @brief Applies stat multipliers and enhanced loot drops to all rooms in the dungeon.
     * - Hard: 1.5x enemy health/attack, upgraded potions/weapons.
     * - NewGamePlus: 2.0x enemy stats, rarer mythic loot pool, extra boss phase threshold.
     */
    static void applyModifiers(Dungeon& dungeon, DifficultyLevel level);

    /**
     * @brief Returns stat scaling factor for the given difficulty.
     */
    static double getStatMultiplier(DifficultyLevel level);

    /**
     * @brief Converts DifficultyLevel enum to standard procedural difficulty integer (1 to 5).
     */
    static int toNumericLevel(DifficultyLevel level);
};

#endif // DIFFICULTYMANAGER_H
