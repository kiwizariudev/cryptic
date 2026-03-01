#pragma once
#include "State.hpp"
#include <Geode/modify/PlayLayer.hpp>

// ─────────────────────────────────────────────
//  Orb Farmer
//  - Auto completes insane levels for orbs
//  - Stops at target orb count
// ─────────────────────────────────────────────

namespace Cryptic {
    inline bool  orbFarmActive  = false;
    inline int   orbFarmTarget  = 50000;
    inline int   orbFarmCount   = 0;
    inline int   orbFarmLevels  = 0;

    inline std::string orbFarmStatus() {
        if (!orbFarmActive) return "Orb farm is OFF";
        int current = GameStatsManager::get()->getStat("6");
        return fmt::format("Farming... {}/{} orbs | {} levels done",
            current, orbFarmTarget, orbFarmLevels);
    }

    inline void stopOrbFarm() {
        orbFarmActive = false;
        Notification::create(
            fmt::format("Farm stopped. {} orbs over {} levels.",
                orbFarmCount, orbFarmLevels),
            NotificationIcon::Warning, 3.f)->show();
    }

    inline void startOrbFarm(int targetOrbs) {
        if (!PlayLayer::get()) {
            Notification::create("Enter an insane level first!",
                NotificationIcon::Error, 2.f)->show();
            return;
        }

        static const std::vector<int> INSANE_IDS = {
            13, 14, 17, 18, 19, 20, 21
        };

        int lid = (int)PlayLayer::get()->m_level->m_levelID;
        bool isInsane = std::find(INSANE_IDS.begin(), INSANE_IDS.end(), lid) != INSANE_IDS.end();

        if (!isInsane) {
            Notification::create("Must be in an insane difficulty level!",
                NotificationIcon::Error, 2.5f)->show();
            return;
        }

        orbFarmTarget  = targetOrbs;
        orbFarmCount   = 0;
        orbFarmLevels  = 0;
        orbFarmActive  = true;

        int current = GameStatsManager::get()->getStat("6");
        Notification::create(
            fmt::format("Farm started! Target: {} (have: {})", targetOrbs, current),
            NotificationIcon::Success, 3.f)->show();

        PlayLayer::get()->resetLevelFromStart();
    }
}

// ─────────────────────────────────────────────
//  PlayLayer hook
// ─────────────────────────────────────────────
static const std::vector<int> INSANE_LEVEL_IDS = { 13, 14, 17, 18, 19, 20, 21 };

class $modify(CrypticOrbFarm, PlayLayer) {

    // ── Named method for scheduleOnce ─────────
    void doFarmRestart(float) {
        if (!Cryptic::orbFarmActive) return;
        this->resetLevelFromStart();
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!Cryptic::orbFarmActive) return;
        if (m_hasCompletedLevel) return;
        if (!m_level) return;

        int lid = (int)m_level->m_levelID;
        bool isTarget = std::find(
            INSANE_LEVEL_IDS.begin(), INSANE_LEVEL_IDS.end(), lid
        ) != INSANE_LEVEL_IDS.end();
        if (!isTarget) return;

        int current = GameStatsManager::get()->getStat("6");
        if (current >= Cryptic::orbFarmTarget) {
            Cryptic::orbFarmActive = false;
            Notification::create(
                fmt::format("Farm done! {} orbs, {} levels.",
                    Cryptic::orbFarmCount, Cryptic::orbFarmLevels),
                NotificationIcon::Success, 4.f)->show();
            return;
        }

        this->levelComplete();
    }

    void levelComplete() {
        PlayLayer::levelComplete();

        if (!Cryptic::orbFarmActive) return;
        if (!m_level) return;

        Cryptic::orbFarmCount  += m_level->m_orbCompletion;
        Cryptic::orbFarmLevels += 1;

        int current = GameStatsManager::get()->getStat("6");
        if (current >= Cryptic::orbFarmTarget) {
            Cryptic::orbFarmActive = false;
            Notification::create(
                fmt::format("Target reached! {} orbs total.", current),
                NotificationIcon::Success, 3.f)->show();
            return;
        }

        Notification::create(
            fmt::format("Orbs: {} / {} | Lvls: {}",
                current, Cryptic::orbFarmTarget, Cryptic::orbFarmLevels),
            NotificationIcon::Loading, 2.f)->show();

        this->scheduleOnce(
            schedule_selector(CrypticOrbFarm::doFarmRestart), 1.2f
        );
    }
};