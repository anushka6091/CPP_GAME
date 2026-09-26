#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include "Inventory.h"
#include "Item.h"
#include <memory>

/**
 * @brief Concrete class representing the Human Hero player.
 * 
 * DESIGN PATTERN: Subtype Polymorphism & Component Integration
 * WHY: Player extends Character and integrates Inventory and equipment logic. 
 * Equipping a weapon dynamically modifies the player's attackPower stats.
 */
class Player : public Character {
private:
    Inventory m_inventory;
    std::unique_ptr<Item> m_equippedWeapon;
    int m_baseAttackPower;

public:
    Player(const std::string& name = "Hero", int health = 30, int attackPower = 8, int defense = 2);

    void attack(Character& target) override;
    void specialAbility(Character* target = nullptr) override;

    // Equipment & Inventory logic
    void equip(std::unique_ptr<Item> weapon);
    void heal(int amount);

    Inventory& getInventory() { return m_inventory; }
    const Inventory& getInventory() const { return m_inventory; }
    Item* getEquippedWeapon() const { return m_equippedWeapon.get(); }
    int getBaseAttackPower() const { return m_baseAttackPower; }
};

#endif // PLAYER_H
