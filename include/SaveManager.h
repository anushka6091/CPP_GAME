#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include "GameMemento.h"
#include <string>
#include <vector>

// Forward declaration of sqlite3 struct
struct sqlite3;

/**
 * @brief Leaderboard entry record.
 */
struct LeaderboardEntry {
    std::string playerName;
    std::string difficulty = "Normal";
    int dungeonSeed = 0;
    int turnsTaken = 0;
    int enemiesKilled = 0;
    std::string completedAt;
};

/**
 * @brief Manages SQLite persistence for GameMemento save slots and dungeon leaderboards.
 * 
 * DESIGN PATTERN: Caretaker (Memento Pattern) & Repository Pattern
 * WHY: SaveManager acts as the Caretaker for GameMemento objects. It handles
 * database persistence, SQL queries, and JSON serialization without inspecting
 * or altering the internal contents of the mementos.
 */
class SaveManager {
private:
    sqlite3* m_db;
    std::string m_dbPath;

    bool executeQuery(const std::string& sql);

public:
    explicit SaveManager(std::string dbPath = "saves.db");
    ~SaveManager();

    // Non-copyable due to raw sqlite3 pointer
    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    // Moveable
    SaveManager(SaveManager&& other) noexcept;
    SaveManager& operator=(SaveManager&& other) noexcept;

    /**
     * @brief Initializes the SQLite database schema if tables do not exist.
     */
    bool initDatabase();

    /**
     * @brief Serializes a GameMemento to JSON and saves/updates it in SQLite.
     * @param playerName The key identifier for the save slot.
     * @param memento The opaque game state snapshot.
     */
    bool saveGame(const std::string& playerName, const GameMemento& memento);

    /**
     * @brief Loads and deserializes a GameMemento for the specified player from SQLite.
     * @param playerName Name of the saved player.
     * @param outMemento Output memento populated on success.
     * @return true if save slot found and loaded; false otherwise.
     */
    bool loadGame(const std::string& playerName, GameMemento& outMemento);

    /**
     * @brief Records a dungeon completion record in the leaderboard table.
     */
    bool recordLeaderboardEntry(
        const std::string& playerName, 
        int dungeonSeed, 
        int turnsTaken, 
        int enemiesKilled,
        const std::string& difficulty = "Normal"
    );

    /**
     * @brief Retrieves top leaderboard records sorted by "turns" or "kills", optionally filtered by difficulty.
     */
    std::vector<LeaderboardEntry> getLeaderboard(
        const std::string& sortBy = "turns", 
        const std::string& difficultyFilter = "All"
    ) const;

    /**
     * @brief Formats and prints the leaderboard table to console.
     */
    void printLeaderboard(
        const std::string& sortBy = "turns", 
        const std::string& difficultyFilter = "All"
    ) const;

    bool isOpen() const { return m_db != nullptr; }
};

#endif // SAVEMANAGER_H
