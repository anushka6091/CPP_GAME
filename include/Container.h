#ifndef CONTAINER_H
#define CONTAINER_H

#include "GameObject.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @brief Composite class in the Composite pattern representing objects that can store other GameObjects.
 * 
 * DESIGN PATTERN: Composite Pattern (Composite Node)
 * WHY: Container extends GameObject and holds a collection of std::unique_ptr<GameObject>. 
 * Because it stores pointers to the abstract base class, a Container can hold both Leaf items (Item) 
 * AND other Composite objects (Container), enabling arbitrary recursive nesting.
 */
class Container : public GameObject {
private:
    std::string m_name;
    std::string m_description;
    std::vector<std::unique_ptr<GameObject>> m_contents;

public:
    Container(std::string name, std::string description = "");

    // GameObject Interface Implementation
    std::string getName() const override { return m_name; }
    std::string describe(int indent = 0) const override;

    // Composite Methods
    void add(std::unique_ptr<GameObject> item);
    std::unique_ptr<GameObject> remove(const std::string& name);
    
    // Check if container is empty
    bool isEmpty() const { return m_contents.empty(); }

    // Direct access to contents
    const std::vector<std::unique_ptr<GameObject>>& getContents() const { return m_contents; }
    std::vector<std::unique_ptr<GameObject>>& getContents() { return m_contents; }
};

#endif // CONTAINER_H
