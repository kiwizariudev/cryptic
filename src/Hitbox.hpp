#pragma once
#include "State.hpp"
#include <Geode/modify/PlayLayer.hpp>

// ─────────────────────────────────────────────
//  Hitbox / Hitbox On Death / Layout Mode
//  NOTE: remove destroyPlayer + postUpdate from
//  PlayLayerHook.hpp to avoid duplicate hooks!
// ─────────────────────────────────────────────

namespace Cryptic {
    inline bool hitboxFlash = false;
}

class $modify(CrypticHitboxLayer, PlayLayer) {

    bool init(GJGameLevel* level, bool useReplay, bool dontCreate) {
        if (!PlayLayer::init(level, useReplay, dontCreate)) return false;
        Cryptic::resetSession();
        Cryptic::hitboxFlash = false;
        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        // Hitbox
        bool showHitbox = Mod::get()->getSettingValue<bool>("show-hitbox");
        GameManager::get()->m_showHitboxes = showHitbox || Cryptic::hitboxFlash;

        // Layout mode
        bool layout = Mod::get()->getSettingValue<bool>("layout-mode");
        if (m_background)   m_background->setVisible(!layout);
        if (m_groundLayer)  m_groundLayer->setVisible(!layout);
        if (m_groundLayer2) m_groundLayer2->setVisible(!layout);

        // Instant complete
        if (Mod::get()->getSettingValue<bool>("instant-complete") && !m_hasCompletedLevel)
            this->levelComplete();
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        // Noclip
        if (Mod::get()->getSettingValue<bool>("noclip")) {
            Cryptic::noclipDeaths++;
            return;
        }

        PlayLayer::destroyPlayer(player, object);

        // Hitbox on death flash
        if (Mod::get()->getSettingValue<bool>("hitbox-on-death")) {
            Cryptic::hitboxFlash = true;
            this->scheduleOnce([](float) {
                Cryptic::hitboxFlash = false;
            }, 2.f, "cryptic_hitbox_flash");
        }
    }

    void onQuit() {
        PlayLayer::onQuit();
        GameManager::get()->m_showHitboxes = false;
        Cryptic::hitboxFlash = false;
    }
};