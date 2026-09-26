#ifndef INVENTORY_H
#define INVENTORY_H

#include "GameObject.h"
#include <vector>
#include <memory>
#include <string>

// Forward declaration
class Player;

/**
 * @brief Class wrapping player storage for GameObjects.
 * 
 * DESIGN PATTERN: Wrapper / Facade for Item Collection
 * WHY: Inventory encapsulates storage management (add, remove, list, use) 
 * using std::vector<std::unique_ptr<GameObject>>. It decouples storage implementation 
 * details from the Player class.
 */
class Inventory {
private:
    std::vector<std::unique_ptr<GameObject>> m_items;

public:
    Inventory() = default;

    void add(std::unique_ptr<GameObject> item);
    std::unique_ptr<GameObject> remove(const std::string& name);
    void listItems() const;
    bool useItem(const std::string& name, Player& player);

    GameObject* getItem(const std::string& name) const;
    int getItemCount(const std::string& name) const;
    bool removeQuantity(const std::string& name, int count);

    bool isEmpty() const { return m_items.empty(); }
    size_t size() const { return m_items.size(); }

    const std::vector<std::unique_ptr<GameObject>>& getItems() const { return m_items; }
};


#endif // INVENTORY_H
