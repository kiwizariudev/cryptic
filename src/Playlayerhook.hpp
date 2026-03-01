#pragma once
#include "HUD.hpp"
#include "Popup.hpp"
#include <Geode/modify/PlayLayer.hpp>

// ─────────────────────────────────────────────
//  PlayLayer hook
//  - Attaches HUD (bottom-left)
//  - Noclip via destroyPlayer
//  - Instant complete via postUpdate
//  - CPS tracking via pushButton
//  - Session best tracked in HUD
//  - M key opens popup
// ─────────────────────────────────────────────
class $modify(CrypticPlayLayer, PlayLayer) {

    bool init(GJGameLevel* level, bool useReplay, bool dontCreate) {
        if (!PlayLayer::init(level, useReplay, dontCreate)) return false;

        Cryptic::resetSession();

        // Attach HUD — bottom-left, above the ground line
        auto* hud = CrypticHUD::create();
        hud->setID("cryptic-hud"_spr);
        hud->setZOrder(999);
        hud->setPosition({ 6.f, 46.f }); // 46 = just above the ground bar
        this->addChild(hud, 999);

        this->setKeyboardEnabled(true);
        return true;
    }

    // ── Noclip ────────────────────────────────
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (Cryptic::noclip()) {
            Cryptic::noclipDeaths++;
            return;
        }
        PlayLayer::destroyPlayer(player, object);
    }

    // ── Instant complete ──────────────────────
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        if (Cryptic::instantComplete() && !m_hasCompletedLevel) {
            this->levelComplete();
        }
    }

    // ── M key → open popup ────────────────────
    void keyDown(enumKeyCodes key) {
        PlayLayer::keyDown(key);
        if (key == enumKeyCodes::KEY_M) {
            auto* scene = CCDirector::sharedDirector()->getRunningScene();
            if (!scene->getChildByTag(7777)) {
                auto* popup = CrypticPopup::create();
                popup->setTag(7777);
                scene->addChild(popup, 999);
            }
        }
    }
};

// ─────────────────────────────────────────────
//  PlayerObject hook — CPS tracking
// ─────────────────────────────────────────────
#include <Geode/modify/PlayerObject.hpp>

class $modify(CrypticPlayer, PlayerObject) {
    void pushButton(PlayerButton btn) {
        PlayerObject::pushButton(btn);
        // Only track player 1 jumps while in a level
        if (PlayLayer::get() && btn == PlayerButton::Jump)
            Cryptic::recordClick();
    }
};