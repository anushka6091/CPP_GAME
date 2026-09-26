#include "Item.h"
#include <sstream>

Item::Item(std::string name, ItemType type, std::string description, int healAmount, int damageBonus)
    : m_name(std::move(name)),
      m_type(type),
      m_description(std::move(description)),
      m_healAmount(healAmount),
      m_damageBonus(damageBonus) {}

std::string Item::describe(int indent) const {
    std::string indentation(indent, ' ');
    std::ostringstream oss;
    oss << indentation << "- [Item: " << m_name << "] (" << itemTypeToString(m_type) << ") - " << m_description;
    
    if (m_type == ItemType::Potion && m_healAmount > 0) {
        oss << " [Heals: +" << m_healAmount << " HP]";
    } else if (m_type == ItemType::Weapon && m_damageBonus > 0) {
        oss << " [Attack Bonus: +" << m_damageBonus << "]";
    }
    return oss.str();
}

std::string Item::itemTypeToString(ItemType type) {
    switch (type) {
        case ItemType::Potion:   return "Potion";
        case ItemType::Weapon:   return "Weapon";
        case ItemType::Key:      return "Key";
        case ItemType::Material: return "Material";
        default:                 return "Unknown";
    }
}
