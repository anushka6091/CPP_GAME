#ifndef COMMANDHISTORY_H
#define COMMANDHISTORY_H

#include <vector>
#include <string>

/**
 * @brief Class that logs executed commands and provides action replay capabilities.
 * 
 * DESIGN PATTERN: Memento / Audit Log component for Command Pattern
 * WHY: CommandHistory records description logs for every executed command. 
 * Replaying actions is "almost for free" because each action is already self-contained 
 * and described as a discrete command object.
 */
class CommandHistory {
private:
    std::vector<std::string> m_historyLog;

public:
    CommandHistory() = default;

    /**
     * @brief Records a command's description into the historical log.
     */
    void record(const std::string& commandDescription);

    /**
     * @brief Re-prints the full chronological sequence of actions taken so far.
     */
    void replay() const;

    /**
     * @brief Returns the raw vector of recorded command descriptions.
     */
    const std::vector<std::string>& getHistory() const { return m_historyLog; }

    /**
     * @brief Returns total number of recorded commands.
     */
    size_t size() const { return m_historyLog.size(); }

    /**
     * @brief Clears history log.
     */
    void clear() { m_historyLog.clear(); }
};

#endif // COMMANDHISTORY_H
