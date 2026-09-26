#ifndef EVENTBUS_H
#define EVENTBUS_H

#include "Quest.h"
#include "GameEvent.h"
#include <vector>

/**
 * @brief Central Event Bus dispatching GameEvents to subscribed Quest observers.
 * 
 * DESIGN PATTERN: Observer Pattern (Subject / Event Publisher)
 * WHY: EventBus decouples event producers (combat engine, movement system, inventory)
 * from event consumers (Quests), allowing new quests to be added without modifying game logic.
 */
class EventBus {
private:
    std::vector<Quest*> m_subscribers;

public:
    EventBus() = default;

    /**
     * @brief Registers a Quest observer to receive game event notifications.
     */
    void subscribe(Quest* quest);

    /**
     * @brief Removes a Quest observer from receiving notifications.
     */
    void unsubscribe(Quest* quest);

    /**
     * @brief Publishes a GameEvent to all subscribed active Quest observers.
     */
    void publish(const GameEvent& event);

    /**
     * @brief Returns current subscriber count.
     */
    size_t getSubscriberCount() const { return m_subscribers.size(); }
};

#endif // EVENTBUS_H
