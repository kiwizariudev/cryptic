#pragma once
#include "State.hpp"

// ─────────────────────────────────────────────
//  FeatureRow — no reference to CrypticPopup
// ─────────────────────────────────────────────
class FeatureRow : public CCNode {
public:
    std::string            m_label;
    CCMenuItemToggler*     m_tog = nullptr;
    CCMenuItemSpriteExtra* m_btn = nullptr;

private:
    CCLayerColor* m_bg = nullptr;

public:
    static FeatureRow* create(
        const std::string& label,
        const std::string& settingKey,
        bool               isCommand,
        int                keyIdx,
        CCObject*          target,
        SEL_MenuHandler    toggleSel,
        SEL_MenuHandler    cmdSel,
        float              w)
    {
        auto* r = new FeatureRow();
        r->m_label = label;
        if (r->init(label, settingKey, isCommand, keyIdx,
                    target, toggleSel, cmdSel, w)) {
            r->autorelease(); return r;
        }
        CC_SAFE_DELETE(r); return nullptr;
    }

    bool init(
        const std::string& label,
        const std::string& settingKey,
        bool               isCommand,
        int                keyIdx,
        CCObject*          target,
        SEL_MenuHandler    toggleSel,
        SEL_MenuHandler    cmdSel,
        float              w)
    {
        if (!CCNode::init()) return false;
        setContentSize({ w, 24.f });

        m_bg = CCLayerColor::create({ 0,0,0,0 }, w, 24.f);
        m_bg->ignoreAnchorPointForPosition(false);
        m_bg->setAnchorPoint({ 0.f, 0.f });
        addChild(m_bg, 0);

        auto* lbl = CCLabelBMFont::create(label.c_str(), "bigFont.fnt");
        lbl->setScale(0.36f);
        lbl->setAnchorPoint({ 0.f, 0.5f });
        lbl->setColor({ 215, 215, 215 });
        lbl->setPosition({ 8.f, 12.f });
        addChild(lbl, 1);

        auto* menu = CCMenu::create();
        menu->setPosition({ 0.f, 0.f });
        menu->setContentSize({ w, 24.f });
        addChild(menu, 1);

        if (isCommand) {
            auto* spr = ButtonSprite::create(
                "Run", "bigFont.fnt", "GJ_button_04.png", 0.5f
            );
            spr->setScale(0.5f);
            m_btn = CCMenuItemSpriteExtra::create(spr, target, cmdSel);
            m_btn->setAnchorPoint({ 1.f, 0.5f });
            m_btn->setPosition({ w - 6.f, 12.f });
            menu->addChild(m_btn);
        } else {
            bool cur = Mod::get()->getSettingValue<bool>(settingKey.c_str());
            m_tog = CCMenuItemToggler::createWithStandardSprites(
                target, toggleSel, 0.5f
            );
            m_tog->toggle(cur);
            m_tog->setTag(keyIdx);
            m_tog->setAnchorPoint({ 1.f, 0.5f });
            m_tog->setPosition({ w - 7.f, 12.f });
            menu->addChild(m_tog);
        }
        return true;
    }

    void setStripe(bool on) {
        m_bg->setColor({ 30, 30, 42 });
        m_bg->setOpacity(on ? 80 : 0);
    }

    void setHighlight(bool on) {
        m_bg->setColor(on ? ccColor3B{ 45,55,90 } : ccColor3B{ 30,30,42 });
        m_bg->setOpacity(on ? 140 : 0);
    }
};

// ─────────────────────────────────────────────
//  CrypticPopup
// ─────────────────────────────────────────────
class CrypticPopup : public geode::Popup<>, public TextInputDelegate {

    static constexpr int   TABS  = 4;
    static constexpr float ROW_H = 24.f;
    static constexpr float GAP   = 2.f;

    static constexpr const char* KEYS[7] = {
        "show-fps","show-speed","show-attempts",
        "show-best-run","show-cps",
        "noclip","instant-complete"
    };

    int    m_tab = 0;
    float  m_tabW = 0.f;   // width of one tab segment — set in setup()
    float  m_tabBarY = 0.f;

    CCNode*               m_pages[TABS]   = {};
    CCMenuItemSpriteExtra* m_tabBtns[TABS] = {};
    CCLayerColor*         m_underline     = nullptr;

    std::vector<FeatureRow*> m_allRows;

    // ── Colours ───────────────────────────────
    ccColor3B accent() { return Cryptic::accentColor(); }
    ccColor3B accentDark() {
        auto c = accent();
        return { (GLubyte)(c.r / 4), (GLubyte)(c.g / 4), (GLubyte)(c.b / 4) };
    }

    // ── Row factories ─────────────────────────
    FeatureRow* makeToggle(const char* label, const char* key, int idx, float w) {
        return FeatureRow::create(label, key, false, idx, this,
            menu_selector(CrypticPopup::onToggle), nullptr, w);
    }
    FeatureRow* makeCmd(const char* label, SEL_MenuHandler sel, float w) {
        return FeatureRow::create(label, "", true, -1, this,
            nullptr, sel, w);
    }

    // ── Page builder ──────────────────────────
    CCNode* buildPage(std::vector<FeatureRow*> rows, float w, float h) {
        auto* pg = CCNode::create();
        pg->setContentSize({ w, h });

        // single column — simpler and cleaner
        float y = h - 8.f;
        for (int i = 0; i < (int)rows.size(); i++) {
            y -= ROW_H;
            rows[i]->setPosition({ 4.f, y });
            rows[i]->setStripe(i % 2 == 0);
            pg->addChild(rows[i]);
            m_allRows.push_back(rows[i]);
            y -= GAP;
        }
        return pg;
    }

protected:
    bool setup() override {
        auto sz = m_mainLayer->getContentSize();
        float w = sz.width, h = sz.height;
        float rowW = w - 8.f;

        // ── Dark bg ───────────────────────────
        auto* bg = CCLayerColor::create({ 14,14,20,255 }, w, h);
        bg->ignoreAnchorPointForPosition(false);
        bg->setAnchorPoint({ 0.f,0.f });
        m_mainLayer->addChild(bg, -1);

        // ── Header ────────────────────────────
        float hdrH = 26.f;
        auto* hdrBg = CCLayerColor::create(
            { accentDark().r, accentDark().g, accentDark().b, 255 }, w, hdrH
        );
        hdrBg->ignoreAnchorPointForPosition(false);
        hdrBg->setAnchorPoint({ 0.f, 0.f });
        hdrBg->setPosition({ 0.f, h - hdrH });
        m_mainLayer->addChild(hdrBg, 5);

        // Title label
        auto* titleLbl = CCLabelBMFont::create("cryptic", "bigFont.fnt");
        titleLbl->setScale(0.44f);
        titleLbl->setColor(accent());
        titleLbl->setAnchorPoint({ 0.f, 0.5f });
        titleLbl->setPosition({ 8.f, h - hdrH / 2.f });
        m_mainLayer->addChild(titleLbl, 6);

        // ── Search bar ────────────────────────
        float sW = 120.f, sH = 16.f;
        auto* sBg = CCLayerColor::create({ 25,25,38,220 }, sW, sH);
        sBg->ignoreAnchorPointForPosition(false);
        sBg->setAnchorPoint({ 0.5f, 0.5f });
        sBg->setPosition({ w / 2.f + 30.f, h - hdrH / 2.f });
        m_mainLayer->addChild(sBg, 6);

        auto* search = CCTextInputNode::create(sW - 8.f, sH, "Search...", "bigFont.fnt");
        search->setMaxLabelScale(0.32f);
        search->setDelegate(this);
        search->setPosition({ w / 2.f + 30.f, h - hdrH / 2.f });
        m_mainLayer->addChild(search, 7);

        // ── Close btn ────────────────────────
        if (m_closeBtn) m_closeBtn->setVisible(false);
        auto* xLbl = CCLabelBMFont::create("X", "bigFont.fnt");
        xLbl->setScale(0.4f);
        xLbl->setColor({ 200,60,60 });
        auto* xMenu = CCMenu::create();
        xMenu->setPosition({ w - 10.f, h - hdrH / 2.f });
        xMenu->addChild(CCMenuItemSpriteExtra::create(
            xLbl, this, menu_selector(CrypticPopup::onCloseBtn)
        ));
        m_mainLayer->addChild(xMenu, 10);

        // ── Tab bar ───────────────────────────
        float tabH  = 20.f;
        float tabY  = h - hdrH - tabH;
        m_tabBarY   = tabY;
        m_tabW      = w / TABS;

        auto* tabBg = CCLayerColor::create({ 18,18,28,255 }, w, tabH);
        tabBg->ignoreAnchorPointForPosition(false);
        tabBg->setAnchorPoint({ 0.f, 0.f });
        tabBg->setPosition({ 0.f, tabY });
        m_mainLayer->addChild(tabBg, 5);

        static const char* tabNames[TABS] = { "Global","HUD","Cheats","Commands" };
        auto* tabMenu = CCMenu::create();
        tabMenu->setPosition({ 0.f, 0.f });
        tabMenu->setContentSize({ w, tabH });
        m_mainLayer->addChild(tabMenu, 6);

        for (int i = 0; i < TABS; i++) {
            auto* lbl = CCLabelBMFont::create(tabNames[i], "bigFont.fnt");
            lbl->setScale(0.34f);
            lbl->setColor(i == 0 ? accent() : ccColor3B{ 120,120,120 });

            auto* btn = CCMenuItemSpriteExtra::create(
                lbl, this, menu_selector(CrypticPopup::onTab)
            );
            btn->setTag(i);
            // Position each tab evenly
            btn->setPosition({ m_tabW * i + m_tabW / 2.f, tabY + tabH / 2.f });
            m_tabBtns[i] = btn;
            tabMenu->addChild(btn);
        }

        // Underline — starts at tab 0
        m_underline = CCLayerColor::create(
            { accent().r, accent().g, accent().b, 220 }, m_tabW, 2.f
        );
        m_underline->ignoreAnchorPointForPosition(false);
        m_underline->setAnchorPoint({ 0.f, 0.f });
        m_underline->setPosition({ 0.f, tabY });  // exact bottom of tab bar
        m_mainLayer->addChild(m_underline, 7);

        // ── Pages ─────────────────────────────
        float pageH = tabY - 4.f;

        m_pages[0] = buildPage({
            makeToggle("Show FPS",      "show-fps",      0, rowW),
            makeToggle("Show Speed",    "show-speed",    1, rowW),
            makeToggle("Show Attempts", "show-attempts", 2, rowW),
            makeToggle("Show Best Run", "show-best-run", 3, rowW),
            makeToggle("Show CPS",      "show-cps",      4, rowW),
        }, w, pageH);

        m_pages[1] = buildPage({
            makeToggle("FPS",        "show-fps",      0, rowW),
            makeToggle("Speed",      "show-speed",    1, rowW),
            makeToggle("Attempts",   "show-attempts", 2, rowW),
            makeToggle("Best Run %", "show-best-run", 3, rowW),
            makeToggle("CPS",        "show-cps",      4, rowW),
        }, w, pageH);

        m_pages[2] = buildPage({
            makeToggle("Noclip",           "noclip",           5, rowW),
            makeToggle("Instant Complete", "instant-complete", 6, rowW),
        }, w, pageH);

        m_pages[3] = buildPage({
            makeCmd("Restart Level",   menu_selector(CrypticPopup::cmdRestart),  rowW),
            makeCmd("Toggle Practice", menu_selector(CrypticPopup::cmdPractice), rowW),
            makeCmd("Copy Level ID",   menu_selector(CrypticPopup::cmdCopyID),   rowW),
            makeCmd("Show Best %",     menu_selector(CrypticPopup::cmdBest),     rowW),
        }, w, pageH);

        for (int i = 0; i < TABS; i++) {
            m_pages[i]->setPosition({ 0.f, 2.f });
            m_pages[i]->setVisible(i == 0);
            m_mainLayer->addChild(m_pages[i], 4);
        }

        // Bottom line
        auto* bot = CCLayerColor::create(
            { accent().r, accent().g, accent().b, 140 }, w, 2.f
        );
        bot->setPosition({ 0.f, 0.f });
        m_mainLayer->addChild(bot, 5);

        return true;
    }

    // ── TextInputDelegate — WORKING search ────
    void textChanged(CCTextInputNode* input) override {
        std::string raw   = input->getString();
        std::string lower = raw;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        bool searching = !lower.empty();

        // When searching: show all pages so all rows are searchable
        for (int i = 0; i < TABS; i++)
            m_pages[i]->setVisible(searching || i == m_tab);

        for (auto* row : m_allRows) {
            std::string lbl = row->m_label;
            std::transform(lbl.begin(), lbl.end(), lbl.begin(), ::tolower);
            bool match = !searching || (lbl.find(lower) != std::string::npos);
            row->setVisible(match);
            if (match) row->setHighlight(searching);
            else       row->setStripe(false);
        }
    }

    // ── Tab ───────────────────────────────────
    void onTab(CCObject* s) {
        int tab = static_cast<CCNode*>(s)->getTag();
        if (tab == m_tab) return;
        m_tab = tab;

        for (int i = 0; i < TABS; i++) {
            m_pages[i]->setVisible(i == tab);
            if (auto* l = dynamic_cast<CCLabelBMFont*>(
                    m_tabBtns[i]->getNormalImage()))
                l->setColor(i == tab ? accent() : ccColor3B{ 120,120,120 });
        }

        // Move underline to correct tab position
        m_underline->setPositionX(tab * m_tabW);
    }

    void onCloseBtn(CCObject*) { onClose(nullptr); }

    // ── Toggle ────────────────────────────────
    void onToggle(CCObject* s) {
        int idx = static_cast<CCNode*>(s)->getTag();
        if (idx < 0 || idx > 6) return;
        bool cur = Mod::get()->getSettingValue<bool>(KEYS[idx]);
        Mod::get()->setSettingValue(KEYS[idx], !cur);
        if (idx == 5)
            Notification::create(!cur ? "Noclip ON" : "Noclip OFF",
                !cur ? NotificationIcon::Success : NotificationIcon::Error, 1.2f)->show();
        if (idx == 6)
            Notification::create(!cur ? "Instant Complete ON" : "Instant Complete OFF",
                !cur ? NotificationIcon::Success : NotificationIcon::Error, 1.2f)->show();
    }

    // ── Commands ──────────────────────────────
    bool checkLevel() {
        if (!PlayLayer::get()) {
            Notification::create("Not in a level!", NotificationIcon::Error, 1.5f)->show();
            return false;
        }
        return true;
    }
    void cmdRestart(CCObject*) {
        if (!checkLevel()) return;
        onClose(nullptr);
        PlayLayer::get()->resetLevelFromStart();
    }
    void cmdPractice(CCObject*) {
        if (!checkLevel()) return;
        auto* pl = PlayLayer::get();
        bool nxt = !pl->m_isPracticeMode;
        pl->togglePracticeMode(nxt);
        Notification::create(nxt ? "Practice ON" : "Practice OFF",
            nxt ? NotificationIcon::Success : NotificationIcon::Error, 1.5f)->show();
    }
    void cmdCopyID(CCObject*) {
        if (!checkLevel()) return;
        auto* pl = PlayLayer::get();
        if (!pl->m_level) return;
        clipboard::write(fmt::format("{}", (int)pl->m_level->m_levelID));
        Notification::create(
            fmt::format("Copied ID: {}", (int)pl->m_level->m_levelID),
            NotificationIcon::Success, 2.f)->show();
    }
    void cmdBest(CCObject*) {
        if (!checkLevel()) return;
        auto* pl = PlayLayer::get();
        if (!pl->m_level) return;
        FLAlertLayer::create("Best %",
            fmt::format("All-time: <cy>{:.0f}%</c>\nSession: <cg>{:.1f}%</c>",
                (float)pl->m_level->m_normalPercent,
                Cryptic::sessionBest).c_str(), "OK")->show();
    }

public:
    static CrypticPopup* create() {
        auto* p = new CrypticPopup();
        if (p->initAnchored(340.f, 280.f)) { p->autorelease(); return p; }
        CC_SAFE_DELETE(p); return nullptr;
    }
};