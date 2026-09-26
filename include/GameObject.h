#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

/**
 * @brief Abstract Base Class representing any object in the game world (Item, Container, etc.).
 * 
 * DESIGN PATTERN: Composite Pattern (Component Interface)
 * WHY: GameObject establishes a common contract for both leaf nodes (Item) and 
 * composite nodes (Container). This allows client code (Inventory, Room, GameEngine) 
 * to manipulate items and containers uniformly without needing to check concrete types.
 */
class GameObject {
public:
    virtual ~GameObject() = default;

    /**
     * @brief Pure virtual method returning the object's display name.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Pure virtual method returning a human-readable description of the object.
     * @param indent Indentation level for nested output formatting (Composite pattern).
     */
    virtual std::string describe(int indent = 0) const = 0;
};

#endif // GAMEOBJECT_H
