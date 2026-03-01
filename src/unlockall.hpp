#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>
using namespace geode::prelude;

// ─────────────────────────────────────────────
//  Unlock All Icons
// ─────────────────────────────────────────────
class $modify(CrypticGSM, GameStatsManager) {
    bool isItemUnlocked(UnlockType type, int key) {
        if (Mod::get()->getSettingValue<bool>("unlock-all"))
            return true;
        return GameStatsManager::isItemUnlocked(type, key);
    }
};

// ─────────────────────────────────────────────
//  Unlock All RobTop Levels
// ─────────────────────────────────────────────
#include <Geode/modify/LevelPage.hpp>

class $modify(CrypticLevelPage, LevelPage) {
    bool init(GJGameLevel* level) {
        if (!LevelPage::init(level)) return false;
        if (Mod::get()->getSettingValue<bool>("unlock-levels") && m_level) {
            m_level->m_requiredCoins = 0;
            m_level->m_levelNotDownloaded = false;
        }
        return true;
    }
};

// ─────────────────────────────────────────────
//  Show 100% on All Levels
// ─────────────────────────────────────────────
#include <Geode/modify/LevelCell.hpp>

class $modify(CrypticLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
        if (Mod::get()->getSettingValue<bool>("fake-100") && level) {
            level->m_normalPercent = 100;
            level->m_practicePercent = 100;
        }
    }
};