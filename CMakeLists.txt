cmake_minimum_required(VERSION 3.14)
project(DungeonCrawlerRPG CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES 
    "src/*.cpp"
)

add_executable(dungeon_crawler ${SOURCES})

target_include_directories(dungeon_crawler PUBLIC include)
