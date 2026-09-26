#include "EventBus.h"
#include <algorithm>
#include <iostream>

void EventBus::subscribe(Quest* quest) {
    if (quest && std::find(m_subscribers.begin(), m_subscribers.end(), quest) == m_subscribers.end()) {
        m_subscribers.push_back(quest);
    }
}

void EventBus::unsubscribe(Quest* quest) {
    m_subscribers.erase(
        std::remove(m_subscribers.begin(), m_subscribers.end(), quest),
        m_subscribers.end()
    );
}

void EventBus::publish(const GameEvent& event) {
    for (auto* quest : m_subscribers) {
        if (quest && quest->getState() != QuestState::Completed) {
            quest->onEvent(event);
        }
    }
}
