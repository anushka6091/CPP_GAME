#include "Container.h"
#include <sstream>
#include <algorithm>

Container::Container(std::string name, std::string description)
    : m_name(std::move(name)), m_description(std::move(description)) {}

void Container::add(std::unique_ptr<GameObject> item) {
    if (item) {
        m_contents.push_back(std::move(item));
    }
}

std::unique_ptr<GameObject> Container::remove(const std::string& name) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    // First check top-level contents
    for (auto it = m_contents.begin(); it != m_contents.end(); ++it) {
        std::string objName = (*it)->getName();
        std::transform(objName.begin(), objName.end(), objName.begin(), ::tolower);
        
        if (objName == lowerName) {
            std::unique_ptr<GameObject> found = std::move(*it);
            m_contents.erase(it);
            return found;
        }
    }

    // Next check recursively inside nested containers if not found at top level
    for (auto& child : m_contents) {
        if (auto* childContainer = dynamic_cast<Container*>(child.get())) {
            auto nestedFound = childContainer->remove(name);
            if (nestedFound) {
                return nestedFound;
            }
        }
    }

    return nullptr;
}

std::string Container::describe(int indent) const {
    std::string indentation(indent, ' ');
    std::ostringstream oss;
    oss << indentation << "+ [Container: " << m_name << "]";
    if (!m_description.empty()) {
        oss << " - " << m_description;
    }
    
    if (m_contents.empty()) {
        oss << " (Empty)";
    } else {
        oss << " (Contains " << m_contents.size() << " item(s)):\n";
        for (size_t i = 0; i < m_contents.size(); ++i) {
            oss << m_contents[i]->describe(indent + 2);
            if (i + 1 < m_contents.size()) {
                oss << "\n";
            }
        }
    }
    return oss.str();
}
