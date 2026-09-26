#ifndef QUEST_H
#define QUEST_H

#include "GameEvent.h"
#include <string>

/**
 * @brief States a Quest can exist in throughout its lifecycle.
 */
enum class QuestState {
    NotStarted,
    InProgress,
    Completed
};

/**
 * @brief Abstract Base Observer class for Quests.
 * 
 * DESIGN PATTERN: Observer Pattern (Concrete Observer Interface)
 * WHY: Quest defines a standard interface for reacting to published GameEvents asynchronously
 * without hardcoding quest tracking logic directly inside combat or movement systems.
 */
class Quest {
protected:
    std::string m_name;
    std::string m_description;
    QuestState m_state;

public:
    Quest(std::string name, std::string description)
        : m_name(std::move(name)), m_description(std::move(description)), m_state(QuestState::InProgress) {}

    virtual ~Quest() = default;

    /**
     * @brief Reaction handler invoked by EventBus whenever a GameEvent is published.
     */
    virtual void onEvent(const GameEvent& event) = 0;

    /**
     * @brief Checks if the quest completion conditions have been satisfied.
     */
    virtual bool isComplete() const = 0;

    /**
     * @brief Formatted progress string for UI/console display.
     */
    virtual std::string getProgressString() const = 0;

    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }
    QuestState getState() const { return m_state; }
    void setState(QuestState state) { m_state = state; }
    
    std::string getStateString() const {
        switch (m_state) {
            case QuestState::NotStarted: return "Not Started";
            case QuestState::InProgress: return "In Progress";
            case QuestState::Completed:  return "COMPLETED";
        }
        return "Unknown";
    }
};

#endif // QUEST_H
