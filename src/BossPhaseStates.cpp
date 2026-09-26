#include "BossPhaseStates.h"
#include "Boss.h"
#include "Character.h"
#include "CharacterStates.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

// ==================== Phase 1 ====================

void Phase1State::attack(Boss& self, Character& target) {
    std::cout << self.getName() << " slashes at " << target.getName() 
              << " with razor-sharp obsidian claws!\n";
    target.takeDamage(self.getAttackPower());

    // 35% chance to trigger dragon flame special ability
    if (target.isAlive() && (rand() % 100 < 35)) {
        specialAbility(self, target);
    }
}

void Phase1State::specialAbility(Boss& self, Character& target) {
    std::cout << ">>> SPECIAL ABILITY: " << self.getName() 
              << " exhales a searing wave of Dragon Fire at " << target.getName() << "! <<<\n";
    int fireDamage = self.getAttackPower() + 4;
    target.takeDamage(fireDamage);
}

int Phase1State::modifyIncomingDamage(Boss& self, int rawDamage) const {
    int effective = std::max(1, rawDamage - self.getDefense());
    return effective;
}

// ==================== Phase 2 ====================

Phase2State::Phase2State()
    : m_hasSummonedAlly(false),
      m_allyName("Goblin Assassin Ally"),
      m_allyAttackPower(6) {}

void Phase2State::onEnterPhase(Boss& self) {
    std::cout << "\n============================================================\n";
    std::cout << ">>> [BOSS PHASE TRANSITION] " << self.getName() 
              << " drops below 50% HP! <<<\n";
    std::cout << self.getName() << " roars furiously and summons a " << m_allyName 
              << " into the fight!\n";
    std::cout << self.getName() << " hardens its dragon scales, becoming RESISTANT to normal attacks (-50% damage taken)!\n";
    std::cout << "============================================================\n";
    m_hasSummonedAlly = true;
}

void Phase2State::attack(Boss& self, Character& target) {
    std::cout << self.getName() << " swings a massive spiked tail at " << target.getName() << "!\n";
    target.takeDamage(self.getAttackPower());

    if (target.isAlive() && m_hasSummonedAlly) {
        std::cout << ">>> [ALLY ASSAULT] The summoned " << m_allyName 
                  << " leaps from the shadows, ambushing " << target.getName() << "! <<<\n";
        target.takeDamage(m_allyAttackPower);

        // 45% chance for the goblin ally to coat blade with poison
        if (target.isAlive() && (rand() % 100 < 45)) {
            std::cout << ">>> [ALLY ABILITY] " << m_allyName 
                      << " stabs with a venom-soaked dagger, poisoning " << target.getName() << "! <<<\n";
            target.transitionTo(std::make_unique<PoisonedState>(2, 3));
        }
    }

    // 25% chance for coordinated special ability
    if (target.isAlive() && (rand() % 100 < 25)) {
        specialAbility(self, target);
    }
}

void Phase2State::specialAbility(Boss& self, Character& target) {
    std::cout << ">>> SPECIAL ABILITY: " << self.getName() << " and " << m_allyName 
              << " execute a Coordinated Shadow Pincer! <<<\n";
    int pincerDamage = self.getAttackPower() + m_allyAttackPower;
    target.takeDamage(pincerDamage);
}

int Phase2State::modifyIncomingDamage(Boss& self, int rawDamage) const {
    // Phase 2 resistance: scale armor reduces damage by 50% after base defense
    int mitigated = std::max(1, rawDamage - self.getDefense());
    int resistantDamage = std::max(1, mitigated / 2);
    std::cout << "[Boss Resilience] Hardened dragon scales deflect the blow! (" 
              << mitigated << " -> " << resistantDamage << " damage)\n";
    return resistantDamage;
}

// ==================== Enrage State ====================

void EnrageState::onEnterPhase(Boss& self) {
    std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    std::cout << ">>> [BOSS ENRAGE ACTIVATED] " << self.getName() 
              << " falls below 15% HP! <<<\n";
    std::cout << self.getName() << "'s eyes ignite with crimson inferno! It enters a BLINDING BERSERK FRENZY!\n";
    std::cout << "Its defense crumbles to 0, but its attack power SURGES to catastrophic levels!\n";
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    self.setDefense(0);
}

void EnrageState::attack(Boss& self, Character& target) {
    int enragedAtk = static_cast<int>(self.getAttackPower() * 1.8);
    std::cout << ">>> [BERSERK FRENZY] " << self.getName() 
              << " unleashes a feral, blood-crazed assault on " << target.getName() 
              << " dealing " << enragedAtk << " raw damage! <<<\n";
    target.takeDamage(enragedAtk);

    // 40% chance to trigger infernal cataclysm
    if (target.isAlive() && (rand() % 100 < 40)) {
        specialAbility(self, target);
    }
}

void EnrageState::specialAbility(Boss& self, Character& target) {
    std::cout << ">>> CRITICAL CATACLYSM: " << self.getName() 
              << " detonates an Infernal Shockwave across the chamber! <<<\n";
    int cataclysmDamage = self.getAttackPower() * 2;
    target.takeDamage(cataclysmDamage);
}

int EnrageState::modifyIncomingDamage(Boss& self, int rawDamage) const {
    // In Enrage, boss has zero defense and takes full unmitigated damage
    std::cout << "[Boss Vulnerability] " << self.getName() 
              << " is enraged and completely defenseless (0 Defense), taking FULL damage!\n";
    return std::max(1, rawDamage);
}

// ==================== AscendedState (NG+ Extra Phase) ====================

void AscendedState::onEnterPhase(Boss& self) {
    std::cout << "\n************************************************************\n";
    std::cout << ">>> [NG+ EXTRA BOSS PHASE ACTIVATED] " << self.getName() 
              << " drops below 35% HP in NEW GAME+! <<<\n";
    std::cout << self.getName() << " ascends into an INFERNAL TEMPEST OVERLORD!\n";
    std::cout << "Draconic fire swirls around its impenetrable scale armor, reflecting strikes!\n";
    std::cout << "************************************************************\n";
}

void AscendedState::attack(Boss& self, Character& target) {
    std::cout << ">>> [ASCENDED TEMPEST] " << self.getName() 
              << " breathes an apocalyptic cyclone of Draconic Fire on " << target.getName() << "!\n";
    int ascendedDamage = self.getAttackPower() + 6;
    target.takeDamage(ascendedDamage);

    if (target.isAlive() && (rand() % 100 < 50)) {
        specialAbility(self, target);
    }
}

void AscendedState::specialAbility(Boss& self, Character& target) {
    std::cout << ">>> CRITICAL ASCENDANCE: " << self.getName() 
              << " calls down a Rain of Scorching Meteors! <<<\n";
    int meteorDamage = self.getAttackPower() + 10;
    target.takeDamage(meteorDamage);
    if (target.isAlive()) {
        target.transitionTo(std::make_unique<PoisonedState>(3, 5));
    }
}

int AscendedState::modifyIncomingDamage(Boss& self, int rawDamage) const {
    // Retains hardened defense: 60% damage reduction
    int mitigated = std::max(1, rawDamage - self.getDefense());
    int reduced = std::max(1, (mitigated * 4) / 10);
    std::cout << "[Ascended Barrier] Draconic aura repels the blow! (" 
              << mitigated << " -> " << reduced << " damage)\n";
    return reduced;
}
