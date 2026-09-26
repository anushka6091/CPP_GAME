#include "Player.h"
#include "GameMemento.h"
#include "Container.h"
#include "CharacterStates.h"
#include <iostream>
#include <algorithm>

static ItemMemento serializeGameObject(const GameObject* obj) {
    ItemMemento mem;
    if (!obj) return mem;

    if (const Item* item = dynamic_cast<const Item*>(obj)) {
        mem.name = item->getName();
        mem.type = Item::itemTypeToString(item->getType());
        mem.description = item->getDescription();
        mem.healAmount = item->getHealAmount();
        mem.damageBonus = item->getDamageBonus();
    } else if (const Container* container = dynamic_cast<const Container*>(obj)) {
        mem.name = container->getName();
        mem.type = "Container";
        mem.description = "";
        for (const auto& child : container->getContents()) {
            mem.containerContents.push_back(serializeGameObject(child.get()));
        }
    }
    return mem;
}

static std::unique_ptr<GameObject> deserializeGameObject(const ItemMemento& mem) {
    if (mem.type == "Container") {
        auto container = std::make_unique<Container>(mem.name, mem.description);
        for (const auto& childMem : mem.containerContents) {
            container->add(deserializeGameObject(childMem));
        }
        return container;
    }

    ItemType type = ItemType::Material;
    if (mem.type == "Potion") type = ItemType::Potion;
    else if (mem.type == "Weapon") type = ItemType::Weapon;
    else if (mem.type == "Key") type = ItemType::Key;

    return std::make_unique<Item>(mem.name, type, mem.description, mem.healAmount, mem.damageBonus);
}

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

PlayerMemento Player::saveState() const {
    PlayerMemento pm;
    pm.name = m_name;
    pm.health = m_health;
    pm.maxHealth = m_maxHealth;
    pm.attackPower = m_attackPower;
    pm.defense = m_defense;
    pm.level = m_level;
    pm.xp = m_xp;
    pm.stateName = getStateName();

    if (m_equippedWeapon) {
        pm.hasEquippedWeapon = true;
        pm.equippedWeapon = serializeGameObject(m_equippedWeapon.get());
    } else {
        pm.hasEquippedWeapon = false;
    }

    for (const auto& item : m_inventory.getItems()) {
        pm.inventoryItems.push_back(serializeGameObject(item.get()));
    }
    return pm;
}

void Player::restoreState(const PlayerMemento& pm) {
    m_name = pm.name;
    m_health = pm.health;
    m_maxHealth = pm.maxHealth;
    m_defense = pm.defense;
    m_level = pm.level;
    m_xp = pm.xp;

    // Restore condition state
    if (pm.stateName == "Dead") {
        m_state = std::make_unique<DeadState>();
    } else if (pm.stateName == "Poisoned") {
        m_state = std::make_unique<PoisonedState>(3, 4);
    } else if (pm.stateName == "Stunned") {
        m_state = std::make_unique<StunnedState>(1);
    } else {
        m_state = std::make_unique<AliveState>();
    }

    // Restore equipped weapon
    if (pm.hasEquippedWeapon) {
        auto weaponObj = deserializeGameObject(pm.equippedWeapon);
        if (auto* item = dynamic_cast<Item*>(weaponObj.get())) {
            m_equippedWeapon = std::unique_ptr<Item>(static_cast<Item*>(weaponObj.release()));
            m_baseAttackPower = pm.attackPower - m_equippedWeapon->getDamageBonus();
            m_attackPower = pm.attackPower;
        }
    } else {
        m_equippedWeapon = nullptr;
        m_baseAttackPower = pm.attackPower;
        m_attackPower = pm.attackPower;
    }

    // Restore inventory
    m_inventory.clear();
    for (const auto& itemMem : pm.inventoryItems) {
        m_inventory.add(deserializeGameObject(itemMem));
    }
}

