#include "SaveManager.h"
#include "sqlite3.h"
#include <iostream>
#include <iomanip>
#include <sstream>

SaveManager::SaveManager(std::string dbPath)
    : m_db(nullptr), m_dbPath(std::move(dbPath)) {
    int rc = sqlite3_open(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "[SaveManager] Error opening SQLite database: " 
                  << (m_db ? sqlite3_errmsg(m_db) : "Unknown error") << "\n";
        m_db = nullptr;
    } else {
        initDatabase();
    }
}

SaveManager::~SaveManager() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

SaveManager::SaveManager(SaveManager&& other) noexcept
    : m_db(other.m_db), m_dbPath(std::move(other.m_dbPath)) {
    other.m_db = nullptr;
}

SaveManager& SaveManager::operator=(SaveManager&& other) noexcept {
    if (this != &other) {
        if (m_db) {
            sqlite3_close(m_db);
        }
        m_db = other.m_db;
        m_dbPath = std::move(other.m_dbPath);
        other.m_db = nullptr;
    }
    return *this;
}

bool SaveManager::executeQuery(const std::string& sql) {
    if (!m_db) return false;
    char* err = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "[SaveManager] SQL Error: " << (err ? err : "Unknown") << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}

bool SaveManager::initDatabase() {
    const std::string createSavesTable = 
        "CREATE TABLE IF NOT EXISTS saves ("
        "  player_name TEXT PRIMARY KEY,"
        "  json_blob TEXT NOT NULL,"
        "  timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    const std::string createLeaderboardTable = 
        "CREATE TABLE IF NOT EXISTS leaderboard ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  player_name TEXT NOT NULL,"
        "  difficulty TEXT NOT NULL DEFAULT 'Normal',"
        "  dungeon_seed INTEGER NOT NULL,"
        "  turns_taken INTEGER NOT NULL,"
        "  enemies_killed INTEGER NOT NULL,"
        "  completed_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    bool ok = executeQuery(createSavesTable) && executeQuery(createLeaderboardTable);
    // Backward compatibility migration if table already existed without difficulty column
    sqlite3_exec(m_db, "ALTER TABLE leaderboard ADD COLUMN difficulty TEXT NOT NULL DEFAULT 'Normal';", nullptr, nullptr, nullptr);
    return ok;
}

bool SaveManager::saveGame(const std::string& playerName, const GameMemento& memento) {
    if (!m_db) {
        std::cerr << "[SaveManager] Database not open!\n";
        return false;
    }

    std::string jsonStr = memento.toJson().dump(2);

    const char* sql = "INSERT OR REPLACE INTO saves (player_name, json_blob, timestamp) "
                      "VALUES (?, ?, CURRENT_TIMESTAMP);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[SaveManager] Failed to prepare save statement: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, playerName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, jsonStr.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "[SaveManager] Error executing save: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    std::cout << "[SaveManager] Successfully saved session for player '" << playerName 
              << "' into SQLite database (" << m_dbPath << ").\n";
    return true;
}

bool SaveManager::loadGame(const std::string& playerName, GameMemento& outMemento) {
    if (!m_db) {
        std::cerr << "[SaveManager] Database not open!\n";
        return false;
    }

    const char* sql = "SELECT json_blob, timestamp FROM saves WHERE player_name = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[SaveManager] Failed to prepare load statement: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, playerName.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char* rawJson = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* rawTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        std::string jsonStr = rawJson ? rawJson : "{}";
        std::string timeStr = rawTime ? rawTime : "";

        sqlite3_finalize(stmt);

        try {
            nlohmann::json j = nlohmann::json::parse(jsonStr);
            outMemento = GameMemento::fromJson(j);
            outMemento.timestamp = timeStr;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "[SaveManager] JSON Deserialization error: " << e.what() << "\n";
            return false;
        }
    }

    sqlite3_finalize(stmt);
    std::cout << "[SaveManager] No saved session found for player '" << playerName << "'.\n";
    return false;
}

bool SaveManager::recordLeaderboardEntry(
    const std::string& playerName, 
    int dungeonSeed, 
    int turnsTaken, 
    int enemiesKilled,
    const std::string& difficulty
) {
    if (!m_db) return false;

    const char* sql = "INSERT INTO leaderboard (player_name, difficulty, dungeon_seed, turns_taken, enemies_killed, completed_at) "
                      "VALUES (?, ?, ?, ?, ?, CURRENT_TIMESTAMP);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[SaveManager] Failed to prepare leaderboard insert: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, playerName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, difficulty.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, dungeonSeed);
    sqlite3_bind_int(stmt, 4, turnsTaken);
    sqlite3_bind_int(stmt, 5, enemiesKilled);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "[SaveManager] Error recording leaderboard entry: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    std::cout << "[SaveManager] Recorded victory for '" << playerName 
              << "' on the Leaderboard [" << difficulty << "] (Turns: " << turnsTaken << ", Kills: " << enemiesKilled << ")!\n";
    return true;
}

std::vector<LeaderboardEntry> SaveManager::getLeaderboard(
    const std::string& sortBy, 
    const std::string& difficultyFilter
) const {
    std::vector<LeaderboardEntry> entries;
    if (!m_db) return entries;

    bool filter = (!difficultyFilter.empty() && difficultyFilter != "All" && difficultyFilter != "all");

    std::string sql = "SELECT player_name, difficulty, dungeon_seed, turns_taken, enemies_killed, completed_at FROM leaderboard ";
    if (filter) {
        sql += "WHERE LOWER(difficulty) = LOWER(?) ";
    }

    if (sortBy == "kills") {
        sql += "ORDER BY enemies_killed DESC, turns_taken ASC LIMIT 10;";
    } else {
        sql += "ORDER BY turns_taken ASC, enemies_killed DESC LIMIT 10;";
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[SaveManager] Error fetching leaderboard: " << sqlite3_errmsg(m_db) << "\n";
        return entries;
    }

    if (filter) {
        sqlite3_bind_text(stmt, 1, difficultyFilter.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        LeaderboardEntry entry;
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* diff = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        entry.playerName = name ? name : "Unknown";
        entry.difficulty = diff ? diff : "Normal";
        entry.dungeonSeed = sqlite3_column_int(stmt, 2);
        entry.turnsTaken = sqlite3_column_int(stmt, 3);
        entry.enemiesKilled = sqlite3_column_int(stmt, 4);
        const char* time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        entry.completedAt = time ? time : "";

        entries.push_back(std::move(entry));
    }

    sqlite3_finalize(stmt);
    return entries;
}

void SaveManager::printLeaderboard(
    const std::string& sortBy, 
    const std::string& difficultyFilter
) const {
    auto entries = getLeaderboard(sortBy, difficultyFilter);

    std::cout << "\n========================================================================================================\n";
    std::cout << "                                  DUNGEON HALL OF FAME (LEADERBOARD)                                    \n";
    std::cout << "       Sorted by: " 
              << (sortBy == "kills" ? "Enemies Slain" : "Fewest Turns Taken (Fastest Clear)")
              << " | Filter: [" << (difficultyFilter.empty() ? "All" : difficultyFilter) << "]\n";
    std::cout << "========================================================================================================\n";
    std::cout << std::left 
              << std::setw(6)  << "Rank" 
              << std::setw(20) << "Hero Name" 
              << std::setw(14) << "Difficulty"
              << std::setw(14) << "Turns Taken" 
              << std::setw(16) << "Enemies Slain" 
              << std::setw(12) << "Seed" 
              << std::setw(20) << "Completed At" << "\n";
    std::cout << "--------------------------------------------------------------------------------------------------------\n";

    if (entries.empty()) {
        std::cout << "  (No dungeon conquerors recorded for this criteria yet!)\n";
    } else {
        for (size_t i = 0; i < entries.size(); ++i) {
            const auto& e = entries[i];
            std::cout << std::left 
                      << std::setw(6)  << ("#" + std::to_string(i + 1))
                      << std::setw(20) << e.playerName
                      << std::setw(14) << e.difficulty
                      << std::setw(14) << e.turnsTaken
                      << std::setw(16) << e.enemiesKilled
                      << std::setw(12) << e.dungeonSeed
                      << std::setw(20) << e.completedAt << "\n";
        }
    }
    std::cout << "========================================================================================================\n";
}
