#ifndef DUNGEONGENERATOR_H
#define DUNGEONGENERATOR_H

#include "Dungeon.h"
#include <memory>

/**
 * @brief Abstract Base Class for procedural dungeon generators.
 * 
 * DESIGN PATTERN: Abstract Factory / Strategy for Procedural Generation
 * WHY: DungeonGenerator defines a contract allowing different generation algorithms 
 * (StandardDungeonGenerator, BSP Dungeons, Maze Dungeons) to be swapped cleanly.
 */
class DungeonGenerator {
public:
    virtual ~DungeonGenerator() = default;

    /**
     * @brief Generates a procedural dungeon level with guaranteed solvability.
     * @param seed Random number generator seed.
     * @param difficultyLevel Scaled difficulty integer (1 to 5).
     * @return std::unique_ptr<Dungeon> Fully constructed, populated, and connected dungeon.
     */
    virtual std::unique_ptr<Dungeon> generate(int seed, int difficultyLevel) = 0;
};

/**
 * @brief Concrete Generator creating grid-graph dungeons with guaranteed BFS solvability.
 */
class StandardDungeonGenerator : public DungeonGenerator {
public:
    StandardDungeonGenerator() = default;

    std::unique_ptr<Dungeon> generate(int seed, int difficultyLevel) override;
};

#endif // DUNGEONGENERATOR_H
