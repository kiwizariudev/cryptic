#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace Cryptic {

    // ── Settings readers ──────────────────────
    inline ccColor3B accentColor() {
        return Mod::get()->getSettingValue<ccColor3B>("accent-color");
    }
    inline bool showFPS()      { return Mod::get()->getSettingValue<bool>("show-fps");      }
    inline bool showSpeed()    { return Mod::get()->getSettingValue<bool>("show-speed");    }
    inline bool showAttempts() { return Mod::get()->getSettingValue<bool>("show-attempts"); }
    inline bool showBestRun()  { return Mod::get()->getSettingValue<bool>("show-best-run"); }
    inline bool showCPS()      { return Mod::get()->getSettingValue<bool>("show-cps");      }
    inline bool noclip()       { return Mod::get()->getSettingValue<bool>("noclip");        }
    inline bool instantComplete() { return Mod::get()->getSettingValue<bool>("instant-complete"); }

    // ── Session state (reset on level enter) ──
    inline float  sessionBest  = 0.f;   // best % this run
    inline int    noclipDeaths = 0;     // deaths skipped by noclip

    // ── CPS tracking ──────────────────────────
    inline std::vector<float> clickTimes;  // timestamps of recent clicks
    inline float currentCPS = 0.f;

    inline void recordClick() {
        float now = CCDirector::sharedDirector()->getTotalFrames()
                    / CCDirector::sharedDirector()->getAnimationInterval()
                    / 60.f; // rough seconds
        clickTimes.push_back(now);
        // Keep only last 1 second
        clickTimes.erase(
            std::remove_if(clickTimes.begin(), clickTimes.end(),
                [now](float t){ return now - t > 1.f; }),
            clickTimes.end()
        );
        currentCPS = (float)clickTimes.size();
    }

    inline void resetSession() {
        sessionBest  = 0.f;
        noclipDeaths = 0;
        clickTimes.clear();
        currentCPS   = 0.f;
    }
}