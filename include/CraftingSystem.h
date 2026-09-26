#ifndef CRAFTINGSYSTEM_H
#define CRAFTINGSYSTEM_H

#include "Item.h"
#include "Inventory.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

/**
 * @brief Struct/Class defining a crafting recipe.
 * 
 * DESIGN PATTERN: Factory Method / Strategy for Item Creation
 * WHY: CraftingRecipe encapsulates ingredient requirements and a factory method (std::function)
 * for instantiating the crafted Item result.
 */
class CraftingRecipe {
private:
    std::string m_name;
    std::string m_description;
    std::vector<std::pair<std::string, int>> m_ingredients; // (ingredient name, count)
    std::function<std::unique_ptr<Item>()> m_resultFactory;

public:
    CraftingRecipe(
        std::string name, 
        std::string description, 
        std::vector<std::pair<std::string, int>> ingredients, 
        std::function<std::unique_ptr<Item>()> resultFactory
    ) : m_name(std::move(name)), 
        m_description(std::move(description)), 
        m_ingredients(std::move(ingredients)), 
        m_resultFactory(std::move(resultFactory)) {}

    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }
    const std::vector<std::pair<std::string, int>>& getIngredients() const { return m_ingredients; }

    /**
     * @brief Produces a new unique_ptr<Item> according to the recipe factory.
     */
    std::unique_ptr<Item> createResult() const {
        if (m_resultFactory) {
            return m_resultFactory();
        }
        return nullptr;
    }

    std::string describe() const;
};

/**
 * @brief Manager holding known CraftingRecipes and executing inventory craft operations.
 */
class CraftingStation {
private:
    std::vector<CraftingRecipe> m_recipes;

public:
    CraftingStation() = default;

    void addRecipe(CraftingRecipe recipe);

    /**
     * @brief Attempts to craft a recipe by name using materials in the provided inventory.
     * @param inv Player's Inventory.
     * @param recipeName Target recipe name.
     * @param outError Error message output if crafting fails.
     * @return true if craft succeeds; false otherwise.
     */
    bool craft(Inventory& inv, const std::string& recipeName, std::string& outError);

    void listRecipes() const;
    const std::vector<CraftingRecipe>& getRecipes() const { return m_recipes; }
};

#endif // CRAFTINGSYSTEM_H
