#include "CommandHistory.h"
#include <iostream>

void CommandHistory::record(const std::string& commandDescription) {
    if (!commandDescription.empty()) {
        m_historyLog.push_back(commandDescription);
    }
}

void CommandHistory::replay() const {
    std::cout << "\n====================================================\n";
    std::cout << "          COMMAND HISTORY REPLAY LOG                \n";
    std::cout << "====================================================\n";

    if (m_historyLog.empty()) {
        std::cout << " (No actions recorded in command history yet)\n";
    } else {
        for (size_t i = 0; i < m_historyLog.size(); ++i) {
            std::cout << " [Step " << (i + 1) << "] " << m_historyLog[i] << "\n";
        }
    }
    std::cout << "====================================================\n";
}
