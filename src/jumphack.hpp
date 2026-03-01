#pragma once
#include "State.hpp"
#include <Geode/modify/PlayerObject.hpp>

// ─────────────────────────────────────────────
//  Jump Hack — configurable mid-air jumps
// ─────────────────────────────────────────────

namespace Cryptic {
    inline int jumpCount = 0;
}

class $modify(CrypticJumpHack, PlayerObject) {

    bool init(int p0, int p1, GJGameLevel* level, bool p3, bool p4) {
        if (!PlayerObject::init(p0, p1, level, p3, p4)) return false;
        Cryptic::jumpCount = 0;
        return true;
    }

    void pushButton(PlayerButton btn) {
        PlayerObject::pushButton(btn);
        if (btn != PlayerButton::Jump) return;
        if (!PlayLayer::get()) return;
        if (!Mod::get()->getSettingValue<bool>("jump-hack")) return;

        int maxJumps = (int)Mod::get()->getSettingValue<int64_t>("jump-hack-count");

        if (!m_isOnGround && !m_isDead && Cryptic::jumpCount < maxJumps) {
            Cryptic::jumpCount++;
            m_yVelocity = m_jumpAccel;
        }
    }

    void collidedWithObject(float dt, GameObject* obj, CCRect rect, bool idk) {
        PlayerObject::collidedWithObject(dt, obj, rect, idk);
        if (m_isOnGround) Cryptic::jumpCount = 0;
    }
};