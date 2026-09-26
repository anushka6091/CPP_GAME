#include "Inventory.h"
#include "Item.h"
#include "Container.h"
#include "Player.h"
#include <iostream>
#include <algorithm>

void Inventory::add(std::unique_ptr<GameObject> item) {
    if (item) {
        std::cout << "[Inventory] Added '" << item->getName() << "' to inventory.\n";
        m_items.push_back(std::move(item));
    }
}

std::unique_ptr<GameObject> Inventory::remove(const std::string& name) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (auto it = m_items.begin(); it != m_items.end(); ++it) {
        std::string itemName = (*it)->getName();
        std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);

        if (itemName == lowerName) {
            std::unique_ptr<GameObject> found = std::move(*it);
            m_items.erase(it);
            return found;
        }
    }
    return nullptr;
}

void Inventory::listItems() const {
    std::cout << "\n=== INVENTORY CONTENTS ===\n";
    if (m_items.empty()) {
        std::cout << " (Inventory is empty)\n";
        return;
    }

    for (const auto& item : m_items) {
        std::cout << item->describe(2) << "\n";
    }
    std::cout << "==========================\n";
}

GameObject* Inventory::getItem(const std::string& name) const {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (const auto& item : m_items) {
        std::string itemName = item->getName();
        std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);

        if (itemName == lowerName) {
            return item.get();
        }
    }
    return nullptr;
}

bool Inventory::useItem(const std::string& name, Player& player) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (auto it = m_items.begin(); it != m_items.end(); ++it) {
        std::string itemName = (*it)->getName();
        std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);

        if (itemName == lowerName) {
            // Check if it's an Item leaf
            if (Item* item = dynamic_cast<Item*>((*it).get())) {
                if (item->getType() == ItemType::Potion) {
                    int heal = item->getHealAmount();
                    player.heal(heal);
                    std::cout << "[Inventory] Used potion '" << item->getName() 
                              << "' restoring " << heal << " HP!\n";
                    m_items.erase(it);
                    return true;
                } else if (item->getType() == ItemType::Weapon) {
                    std::unique_ptr<GameObject> obj = std::move(*it);
                    m_items.erase(it);
                    
                    // Downcast back to Item for equip
                    std::unique_ptr<Item> weapon(static_cast<Item*>(obj.release()));
                    player.equip(std::move(weapon));
                    return true;
                } else {
                    std::cout << "[Inventory] Item '" << item->getName() 
                              << "' (" << Item::itemTypeToString(item->getType()) 
                              << ") cannot be consumed directly.\n";
                    return false;
                }
            } else if (Container* container = dynamic_cast<Container*>((*it).get())) {
                std::cout << "[Inventory] Inspecting Container:\n";
                std::cout << container->describe(2) << "\n";
                return true;
            }
        }
    }

    std::cout << "[Inventory] No item named '" << name << "' found in inventory.\n";
    return false;
}

int Inventory::getItemCount(const std::string& name) const {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    int count = 0;
    for (const auto& item : m_items) {
        std::string itemName = item->getName();
        std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);

        if (itemName == lowerName || itemName.find(lowerName) != std::string::npos) {
            count++;
        }
    }
    return count;
}

bool Inventory::removeQuantity(const std::string& name, int count) {
    if (getItemCount(name) < count) {
        return false;
    }

    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    int removed = 0;
    for (auto it = m_items.begin(); it != m_items.end() && removed < count; ) {
        std::string itemName = (*it)->getName();
        std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);

        if (itemName == lowerName || itemName.find(lowerName) != std::string::npos) {
            it = m_items.erase(it);
            removed++;
        } else {
            ++it;
        }
    }
    return removed == count;
}

