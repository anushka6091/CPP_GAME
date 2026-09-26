#include "GameEngine.h"
#include <iostream>
#include <string>

// External function from TestSolvability.cpp
int testSolvabilitySuite();

int main(int argc, char* argv[]) {
    // If run with --test flag or by default run solvability check suite first
    bool runTestsOnly = (argc > 1 && std::string(argv[1]) == "--test");

    if (runTestsOnly) {
        return testSolvabilitySuite();
    }

    // Run automated solvability suite verification first
    testSolvabilitySuite();

    // Launch interactive game engine
    GameEngine engine;
    engine.initialize();
    engine.start();
    return 0;
}

