#include "ConcreteQuests.h"
#include <iostream>
#include <algorithm>

// Case-insensitive substring search helper
static bool containsIgnoreCase(const std::string& str, const std::string& sub) {
    auto it = std::search(
        str.begin(), str.end(),
        sub.begin(), sub.end(),
        [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
    );
    return it != str.end();
}

// ==================== KillCountQuest ====================
KillCountQuest::KillCountQuest(std::string name, std::string description, std::string targetEnemySubstring, int requiredKills)
    : Quest(std::move(name), std::move(description)),
      m_targetEnemySubstring(std::move(targetEnemySubstring)),
      m_requiredKills(requiredKills),
      m_currentKills(0) {}

void KillCountQuest::onEvent(const GameEvent& event) {
    if (m_state == QuestState::Completed) return;

    if (event.type == GameEventType::EnemyKilled && containsIgnoreCase(event.targetName, m_targetEnemySubstring)) {
        m_currentKills += event.count;
        std::cout << "\n>>> [QUEST UPDATE] '" << m_name << "': Defeated " 
                  << event.targetName << " (" << m_currentKills << "/" << m_requiredKills << ") <<<\n";

        if (isComplete()) {
            m_state = QuestState::Completed;
            std::cout << ">>> *** QUEST COMPLETED: " << m_name << "! *** <<<\n";
        }
    }
}

bool KillCountQuest::isComplete() const {
    return m_currentKills >= m_requiredKills;
}

std::string KillCountQuest::getProgressString() const {
    return "[" + getStateString() + "] " + m_name + " - " + m_description + 
           " (" + std::to_string(m_currentKills) + "/" + std::to_string(m_requiredKills) + " Defeated)";
}

// ==================== ItemCollectionQuest ====================
ItemCollectionQuest::ItemCollectionQuest(std::string name, std::string description, std::string targetItemSubstring, int requiredAmount)
    : Quest(std::move(name), std::move(description)),
      m_targetItemSubstring(std::move(targetItemSubstring)),
      m_requiredAmount(requiredAmount),
      m_currentAmount(0) {}

void ItemCollectionQuest::onEvent(const GameEvent& event) {
    if (m_state == QuestState::Completed) return;

    if (event.type == GameEventType::ItemCollected && containsIgnoreCase(event.targetName, m_targetItemSubstring)) {
        m_currentAmount += event.count;
        std::cout << "\n>>> [QUEST UPDATE] '" << m_name << "': Collected " 
                  << event.targetName << " (" << m_currentAmount << "/" << m_requiredAmount << ") <<<\n";

        if (isComplete()) {
            m_state = QuestState::Completed;
            std::cout << ">>> *** QUEST COMPLETED: " << m_name << "! *** <<<\n";
        }
    }
}

bool ItemCollectionQuest::isComplete() const {
    return m_currentAmount >= m_requiredAmount;
}

std::string ItemCollectionQuest::getProgressString() const {
    return "[" + getStateString() + "] " + m_name + " - " + m_description + 
           " (" + std::to_string(m_currentAmount) + "/" + std::to_string(m_requiredAmount) + " Collected)";
}
