#ifndef ITEM_H
#define ITEM_H

#include "GameObject.h"
#include <string>

/**
 * @brief Enum defining the specific functional category of an Item.
 */
enum class ItemType {
    Potion,
    Weapon,
    Key,
    Material
};

/**
 * @brief Leaf class in the Composite pattern representing individual, non-container game items.
 * 
 * DESIGN PATTERN: Composite Pattern (Leaf)
 * WHY: Item is an atomic GameObject. It cannot contain other GameObjects. 
 * Its describe() implementation formats and returns its own attributes and stats.
 */
class Item : public GameObject {
private:
    std::string m_name;
    std::string m_description;
    ItemType m_type;
    int m_healAmount;
    int m_damageBonus;

public:
    Item(std::string name, ItemType type, std::string description, int healAmount = 0, int damageBonus = 0);

    // GameObject Interface Implementation
    std::string getName() const override { return m_name; }
    std::string describe(int indent = 0) const override;

    // Getters
    ItemType getType() const { return m_type; }
    std::string getDescription() const { return m_description; }
    int getHealAmount() const { return m_healAmount; }
    int getDamageBonus() const { return m_damageBonus; }

    // Helper to get string name of ItemType
    static std::string itemTypeToString(ItemType type);
};

#endif // ITEM_H
