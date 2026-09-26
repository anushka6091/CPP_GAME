cmake_minimum_required(VERSION 3.14)
project(DungeonCrawlerRPG CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. nlohmann/json dependency via CMake FetchContent
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# 2. SQLite3 dependency (find_package or FetchContent for SQLiteCpp wrapper)
find_package(SQLite3 QUIET)
if(NOT SQLite3_FOUND)
    # Fallback to FetchContent for SQLiteCpp or bundled amalgamation
    FetchContent_Declare(
        SQLiteCpp
        GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
        GIT_TAG        3.3.1
    )
    # If network/git is unavailable, use local bundled amalgamation
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite-amalgamation-3450200/sqlite3.c")
        add_library(sqlite3_bundled STATIC third_party/sqlite-amalgamation-3450200/sqlite3.c)
        target_include_directories(sqlite3_bundled PUBLIC include third_party/sqlite-amalgamation-3450200)
    endif()
endif()

include_directories(include)

file(GLOB SOURCES 
    "src/*.cpp"
)

add_executable(dungeon_crawler ${SOURCES})

target_include_directories(dungeon_crawler PUBLIC include)
target_link_libraries(dungeon_crawler PRIVATE nlohmann_json::nlohmann_json)

if(TARGET SQLite::SQLite3)
    target_link_libraries(dungeon_crawler PRIVATE SQLite::SQLite3)
elseif(TARGET sqlite3_bundled)
    target_link_libraries(dungeon_crawler PRIVATE sqlite3_bundled)
else()
    target_link_libraries(dungeon_crawler PRIVATE sqlite3)
endif()
