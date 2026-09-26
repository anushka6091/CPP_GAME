#include "DungeonGenerator.h"
#include "Goblin.h"
#include "Skeleton.h"
#include "Item.h"
#include "Container.h"
#include <random>
#include <map>
#include <queue>
#include <vector>
#include <algorithm>
#include <sstream>

std::unique_ptr<Dungeon> StandardDungeonGenerator::generate(int seed, int difficultyLevel) {
    auto dungeon = std::make_unique<Dungeon>();
    std::mt19937 rng(static_cast<unsigned int>(seed));

    int targetRooms = 8 + difficultyLevel * 2;
    std::map<std::pair<int, int>, Room*> grid;
    std::vector<std::pair<int, int>> posList;

    // 1. Create Start Room at (0,0)
    std::pair<int, int> startPos = {0, 0};
    auto startRoomPtr = std::make_unique<Room>(
        "Dungeon Entrance - A cold, damp stone vestibule. Flickering torches cast long shadows on ancient brickwork."
    );
    Room* startRoom = startRoomPtr.get();
    dungeon->addRoom(std::move(startRoomPtr));
    grid[startPos] = startRoom;
    posList.push_back(startPos);
    dungeon->setStartRoom(startRoom);

    // Direction offsets
    const std::vector<std::pair<Direction, std::pair<int, int>>> directions = {
        { Direction::North, {0, 1} },
        { Direction::South, {0, -1} },
        { Direction::East,  {1, 0} },
        { Direction::West,  {-1, 0} }
    };

    // Room descriptions generator bank
    std::vector<std::string> roomDescs = {
        "A crumbling guard post filled with rusted armor and broken spears.",
        "An abandoned alchemy laboratory smelling faintly of sulfur and dried herbs.",
        "A subterranean cavern with water dripping steadily from stalactites.",
        "A forgotten library lined with rotting bookshelves and dusty scrolls.",
        "A dark corridor with moss-covered stone walls and creaking floorboards.",
        "A vaulted shrine dedicated to forgotten gods with a cracked marble altar.",
        "A narrow stone passage echoing with ominous distant whispers.",
        "A flooded chamber with waist-deep murky water and slippery cobblestones."
    };

    // 2. Procedurally grow connected grid graph
    while (static_cast<int>(grid.size()) < targetRooms) {
        // Pick a random existing position
        std::uniform_int_distribution<size_t> posDist(0, posList.size() - 1);
        std::pair<int, int> currPos = posList[posDist(rng)];

        // Pick a random direction
        std::uniform_int_distribution<size_t> dirDist(0, directions.size() - 1);
        const auto& dirInfo = directions[dirDist(rng)];
        Direction dir = dirInfo.first;
        std::pair<int, int> neighborPos = { currPos.first + dirInfo.second.first, currPos.second + dirInfo.second.second };

        // If neighbor position is unvisited, expand
        if (grid.find(neighborPos) == grid.end()) {
            std::uniform_int_distribution<size_t> descDist(0, roomDescs.size() - 1);
            std::string desc = roomDescs[descDist(rng)];

            auto newRoomPtr = std::make_unique<Room>(desc);
            Room* newRoom = newRoomPtr.get();

            // Bi-directional connection
            Room* currRoom = grid[currPos];
            currRoom->setExit(dir, newRoom);
            newRoom->setExit(getOppositeDirection(dir), currRoom);

            grid[neighborPos] = newRoom;
            posList.push_back(neighborPos);
            dungeon->addRoom(std::move(newRoomPtr));
        }
    }

    // 3. Perform BFS to compute distance (depth) from Start Room and designate deepest room as Boss Room
    std::queue<Room*> q;
    std::map<Room*, int> distanceMap;

    q.push(startRoom);
    distanceMap[startRoom] = 0;

    Room* deepestRoom = startRoom;
    int maxDistance = 0;

    while (!q.empty()) {
        Room* curr = q.front();
        q.pop();

        int currDist = distanceMap[curr];
        if (currDist > maxDistance) {
            maxDistance = currDist;
            deepestRoom = curr;
        }

        for (const auto& exitPair : curr->getExits()) {
            Room* neighbor = exitPair.second;
            if (neighbor && distanceMap.find(neighbor) == distanceMap.end()) {
                distanceMap[neighbor] = currDist + 1;
                q.push(neighbor);
            }
        }
    }

    // Designate Boss Room
    dungeon->setBossRoom(deepestRoom);

    // 4. Populate Boss Room
    int bossHp = 30 + difficultyLevel * 10;
    int bossAtk = 10 + difficultyLevel * 3;
    auto bossEnemy = std::make_unique<Skeleton>("Dungeon Dragon Overlord", bossHp, bossAtk, 3);
    deepestRoom->setEnemy(std::move(bossEnemy));

    auto bossChest = std::make_unique<Container>("Royal Treasure Chest", "A massive gold-trimmed chest overflowing with ancient relics.");
    bossChest->add(std::make_unique<Item>("Dragon Slaying Sword", ItemType::Weapon, "A legendary blade glowing with magical fire.", 0, 15 + difficultyLevel * 3));
    bossChest->add(std::make_unique<Item>("Elixir of Immortality", ItemType::Potion, "A rare shimmering potion.", 50, 0));
    deepestRoom->addItem(std::move(bossChest));

    // 5. Populate intermediate rooms with scaling enemies & composite items
    std::uniform_int_distribution<int> chanceDist(1, 100);
    bool goldenKeyPlaced = false;

    for (const auto& roomPair : grid) {
        Room* r = roomPair.second;
        if (r == startRoom || r == deepestRoom) continue;

        // Guarantee Golden Key quest item in the first non-boss intermediate room
        if (!goldenKeyPlaced) {
            r->addItem(std::make_unique<Item>("Golden Key", ItemType::Key, "A gleaming golden key decorated with ancient runes.", 0, 0));
            goldenKeyPlaced = true;
        }

        // Enemy spawn check (70% chance)
        if (chanceDist(rng) <= 70) {
            int hp = 15 + difficultyLevel * 4;
            int atk = 4 + difficultyLevel * 2;

            if (chanceDist(rng) <= 50) {
                r->setEnemy(std::make_unique<Goblin>("Vicious Goblin", hp, atk, 1));
            } else {
                r->setEnemy(std::make_unique<Skeleton>("Ancient Skeleton Warrior", hp + 5, atk + 1, 2));
            }
        }

        // Loot spawn check (60% chance)
        if (chanceDist(rng) <= 60) {
            if (chanceDist(rng) <= 50) {
                r->addItem(std::make_unique<Item>("Health Potion", ItemType::Potion, "Restorative potion.", 15 + difficultyLevel * 5, 0));
            } else {
                auto chest = std::make_unique<Container>("Wooden Chest", "An old wooden container.");
                chest->add(std::make_unique<Item>("Steel Dagger", ItemType::Weapon, "A sharp dagger.", 0, 4 + difficultyLevel * 2));
                r->addItem(std::move(chest));
            }
        }
    }


    return dungeon;
}
