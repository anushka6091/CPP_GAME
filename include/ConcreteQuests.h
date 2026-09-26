#ifndef CONCRETEQUESTS_H
#define CONCRETEQUESTS_H

#include "Quest.h"
#include <string>

/**
 * @brief Concrete Quest observer tracking target enemy kills.
 */
class KillCountQuest : public Quest {
private:
    std::string m_targetEnemySubstring;
    int m_requiredKills;
    int m_currentKills;

public:
    KillCountQuest(std::string name, std::string description, std::string targetEnemySubstring, int requiredKills);

    void onEvent(const GameEvent& event) override;
    bool isComplete() const override;
    std::string getProgressString() const override;

    int getCurrentKills() const { return m_currentKills; }
    int getRequiredKills() const { return m_requiredKills; }
};

/**
 * @brief Concrete Quest observer tracking specific item pickups.
 */
class ItemCollectionQuest : public Quest {
private:
    std::string m_targetItemSubstring;
    int m_requiredAmount;
    int m_currentAmount;

public:
    ItemCollectionQuest(std::string name, std::string description, std::string targetItemSubstring, int requiredAmount = 1);

    void onEvent(const GameEvent& event) override;
    bool isComplete() const override;
    std::string getProgressString() const override;

    int getCurrentAmount() const { return m_currentAmount; }
    int getRequiredAmount() const { return m_requiredAmount; }
};

#endif // CONCRETEQUESTS_H
