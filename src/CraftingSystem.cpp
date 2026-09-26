#include "CraftingSystem.h"
#include <iostream>
#include <algorithm>

std::string CraftingRecipe::describe() const {
    std::string out = "  [Recipe] " + m_name + " - " + m_description + "\n";
    out += "    Ingredients:\n";
    for (const auto& ing : m_ingredients) {
        out += "      - " + ing.first + " x" + std::to_string(ing.second) + "\n";
    }
    return out;
}

void CraftingStation::addRecipe(CraftingRecipe recipe) {
    m_recipes.push_back(std::move(recipe));
}

bool CraftingStation::craft(Inventory& inv, const std::string& recipeName, std::string& outError) {
    std::string lowerInput = recipeName;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);

    const CraftingRecipe* found = nullptr;
    for (const auto& r : m_recipes) {
        std::string rName = r.getName();
        std::transform(rName.begin(), rName.end(), rName.begin(), ::tolower);
        if (rName == lowerInput || rName.find(lowerInput) != std::string::npos) {
            found = &r;
            break;
        }
    }

    if (!found) {
        outError = "[Crafting] Unknown recipe '" + recipeName + "'. Type 'recipes' to list available recipes.";
        return false;
    }

    // Check all ingredients present
    for (const auto& ing : found->getIngredients()) {
        int have = inv.getItemCount(ing.first);
        if (have < ing.second) {
            outError = "[Crafting] Missing materials for '" + found->getName() + "':\n"
                     + "  Need " + std::to_string(ing.second) + "x " + ing.first
                     + " but only have " + std::to_string(have) + ".";
            return false;
        }
    }

    // Deduct all ingredients
    for (const auto& ing : found->getIngredients()) {
        inv.removeQuantity(ing.first, ing.second);
    }

    // Produce result and add to inventory
    auto result = found->createResult();
    if (result) {
        std::cout << "\n[Crafting Station] >>> Successfully crafted: " << result->getName() << "! <<<\n";
        inv.add(std::move(result));
        return true;
    }

    outError = "[Crafting] Recipe factory failed to create item.";
    return false;
}

void CraftingStation::listRecipes() const {
    std::cout << "\n====================================================\n";
    std::cout << "       CRAFTING STATION - KNOWN RECIPES            \n";
    std::cout << "====================================================\n";
    if (m_recipes.empty()) {
        std::cout << "  No recipes discovered yet.\n";
    } else {
        for (const auto& r : m_recipes) {
            std::cout << r.describe();
        }
    }
    std::cout << "====================================================\n";
}
