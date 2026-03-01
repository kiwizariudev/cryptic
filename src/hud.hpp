#pragma once
#include "State.hpp"

// ─────────────────────────────────────────────
//  CrypticHUD
//  Fixed overlay — bottom-left of screen.
//  Rows: FPS · Speed · Attempts · Best Run · CPS
//  + Noclip death counter when noclip is on.
// ─────────────────────────────────────────────
class CrypticHUD : public CCNode {

    static constexpr float ROW_H  = 14.f;
    static constexpr float PAD    = 6.f;
    static constexpr float BG_W   = 116.f;
    static constexpr float ICON_W = 36.f;

    struct Row {
        CCLabelBMFont* key = nullptr;
        CCLabelBMFont* val = nullptr;
    };

    // Rows in display order
    enum RowID { R_FPS, R_SPD, R_ATT, R_BEST, R_CPS, R_NC, R_COUNT };
    Row           m_rows[R_COUNT];
    CCLayerColor* m_bg = nullptr;

    // FPS smoothing
    float m_fpsAccum  = 0.f;
    int   m_fpsCnt    = 0;
    float m_fps       = 60.f;

public:
    static CrypticHUD* create() {
        auto* h = new CrypticHUD();
        if (h->init()) { h->autorelease(); return h; }
        CC_SAFE_DELETE(h);
        return nullptr;
    }

    bool init() {
        if (!CCNode::init()) return false;

        m_bg = CCLayerColor::create({ 0, 0, 0, 160 }, BG_W, 10.f);
        m_bg->ignoreAnchorPointForPosition(false);
        m_bg->setAnchorPoint({ 0.f, 0.f });
        addChild(m_bg, 0);

        static const char* keys[R_COUNT] = {
            "FPS", "SPD", "ATT", "BEST", "CPS", "NC"
        };
        static const ccColor3B keyColors[R_COUNT] = {
            {160,160,160},{160,160,160},{160,160,160},
            {160,160,160},{160,160,160},{255,80,80}
        };

        for (int i = 0; i < R_COUNT; i++) {
            auto& r = m_rows[i];

            r.key = CCLabelBMFont::create(keys[i], "bigFont.fnt");
            r.key->setScale(0.28f);
            r.key->setAnchorPoint({ 0.f, 0.5f });
            r.key->setColor(keyColors[i]);
            addChild(r.key, 1);

            r.val = CCLabelBMFont::create("--", "bigFont.fnt");
            r.val->setScale(0.28f);
            r.val->setAnchorPoint({ 0.f, 0.5f });
            addChild(r.val, 1);
        }

        scheduleUpdate();
        return true;
    }

    void update(float dt) override {
        // ── FPS ───────────────────────────────
        m_fpsAccum += dt; m_fpsCnt++;
        if (m_fpsCnt >= 20) {
            m_fps = m_fpsCnt / m_fpsAccum;
            m_fpsAccum = 0.f; m_fpsCnt = 0;
        }

        // ── PlayLayer data ────────────────────
        float percent  = 0.f;
        int   attempts = 0;
        float speed    = CCDirector::sharedDirector()
                             ->getScheduler()->getTimeScale();

        if (auto* pl = PlayLayer::get()) {
            percent  = pl->getCurrentPercent();
            attempts = pl->m_attempts;

            // Update session best
            if (percent > Cryptic::sessionBest)
                Cryptic::sessionBest = percent;
        }

        // ── Visibility ────────────────────────
        bool vis[R_COUNT] = {
            Cryptic::showFPS(),
            Cryptic::showSpeed(),
            Cryptic::showAttempts(),
            Cryptic::showBestRun(),
            Cryptic::showCPS(),
            Cryptic::noclip()   // NC row only when noclip on
        };

        // ── Values ────────────────────────────
        auto accent = Cryptic::accentColor();

        std::string vals[R_COUNT] = {
            fmt::format("{:.0f}", m_fps),
            fmt::format("{:.2f}x", speed),
            fmt::format("{}", attempts),
            fmt::format("{:.1f}%", Cryptic::sessionBest),
            fmt::format("{:.0f}", Cryptic::currentCPS),
            fmt::format("x{}", Cryptic::noclipDeaths)
        };

        // ── Layout (bottom-up) ────────────────
        int activeCount = 0;
        for (int i = 0; i < R_COUNT; i++)
            if (vis[i]) activeCount++;

        float totalH = activeCount * ROW_H + PAD * 2.f;
        m_bg->setContentSize({ BG_W, totalH });
        this->setVisible(activeCount > 0);

        float rowY = PAD + ROW_H * 0.5f;
        for (int i = R_COUNT - 1; i >= 0; i--) {
            m_rows[i].key->setVisible(vis[i]);
            m_rows[i].val->setVisible(vis[i]);
            if (!vis[i]) continue;

            m_rows[i].key->setPosition({ PAD, rowY });
            m_rows[i].val->setString(vals[i].c_str());
            m_rows[i].val->setColor(i == R_NC ? ccColor3B{255,80,80} : accent);
            m_rows[i].val->setPosition({ PAD + ICON_W, rowY });

            rowY += ROW_H;
        }
    }
};