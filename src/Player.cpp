#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player(const std::string& name, int health, int attackPower, int defense)
    : Character(name, health, attackPower, defense, 1, 0),
      m_baseAttackPower(attackPower) {}

void Player::attack(Character& target) {
    if (m_equippedWeapon) {
        std::cout << m_name << " strikes " << target.getName() 
                  << " with " << m_equippedWeapon->getName() << "!\n";
    } else {
        std::cout << m_name << " strikes " << target.getName() << " with bare fists!\n";
    }
    target.takeDamage(m_attackPower);
}

void Player::specialAbility(Character* target) {
    std::cout << m_name << " focuses energy for a powerful strike!\n";
    if (target) {
        int bonusDamage = m_attackPower + 5;
        std::cout << m_name << " deals " << bonusDamage << " heavy damage to " << target->getName() << "!\n";
        target->takeDamage(bonusDamage);
    }
}

void Player::equip(std::unique_ptr<Item> weapon) {
    if (!weapon || weapon->getType() != ItemType::Weapon) {
        std::cout << "[Player] Cannot equip non-weapon object!\n";
        return;
    }

    // If already holding a weapon, return the current weapon to inventory
    if (m_equippedWeapon) {
        std::cout << "[Player] Unequipping " << m_equippedWeapon->getName() 
                  << " and placing it back into inventory.\n";
        std::string oldName = m_equippedWeapon->getName();
        m_inventory.add(std::move(m_equippedWeapon));
    }

    m_equippedWeapon = std::move(weapon);
    m_attackPower = m_baseAttackPower + m_equippedWeapon->getDamageBonus();

    std::cout << "[Player] Equipped '" << m_equippedWeapon->getName() 
              << "' (+ " << m_equippedWeapon->getDamageBonus() << " Damage). "
              << "Total Attack Power is now " << m_attackPower << "!\n";
}

void Player::heal(int amount) {
    int oldHealth = m_health;
    m_health = std::min(m_maxHealth, m_health + amount);
    int healedAmount = m_health - oldHealth;
    std::cout << "[Player] Healed for " << healedAmount << " HP. Current Health: " 
              << m_health << "/" << m_maxHealth << "\n";
}
