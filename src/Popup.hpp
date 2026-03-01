#pragma once
#include "State.hpp"

// ─────────────────────────────────────────────
//  FeatureRow
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
        setContentSize({ w, 26.f });

        m_bg = CCLayerColor::create({ 0,0,0,0 }, w, 26.f);
        m_bg->ignoreAnchorPointForPosition(false);
        m_bg->setAnchorPoint({ 0.f, 0.f });
        addChild(m_bg, 0);

        // Tag icon per type
        const char* icon = isCommand ? "▶" : "•";
        auto* iconLbl = CCLabelBMFont::create(icon, "chatFont.fnt");
        iconLbl->setScale(0.4f);
        iconLbl->setAnchorPoint({ 0.f, 0.5f });
        iconLbl->setPosition({ 6.f, 13.f });
        iconLbl->setColor({ 100, 100, 120 });
        addChild(iconLbl, 1);

        auto* lbl = CCLabelBMFont::create(label.c_str(), "bigFont.fnt");
        lbl->setScale(0.37f);
        lbl->setAnchorPoint({ 0.f, 0.5f });
        lbl->setColor({ 220, 220, 230 });
        lbl->setPosition({ 20.f, 13.f });
        addChild(lbl, 1);

        auto* menu = CCMenu::create();
        menu->setPosition({ 0.f, 0.f });
        menu->setContentSize({ w, 26.f });
        addChild(menu, 1);

        if (isCommand) {
            auto* spr = ButtonSprite::create(
                "Run", "bigFont.fnt", "GJ_button_04.png", 0.5f
            );
            spr->setScale(0.48f);
            m_btn = CCMenuItemSpriteExtra::create(spr, target, cmdSel);
            m_btn->setAnchorPoint({ 1.f, 0.5f });
            m_btn->setPosition({ w - 8.f, 13.f });
            menu->addChild(m_btn);
        } else {
            bool cur = Mod::get()->getSettingValue<bool>(settingKey.c_str());
            m_tog = CCMenuItemToggler::createWithStandardSprites(
                target, toggleSel, 0.52f
            );
            m_tog->toggle(cur);
            m_tog->setTag(keyIdx);
            m_tog->setAnchorPoint({ 1.f, 0.5f });
            m_tog->setPosition({ w - 8.f, 13.f });
            menu->addChild(m_tog);
        }
        return true;
    }

    void setStripe(bool on) {
        m_bg->setColor({ 22, 22, 34 });
        m_bg->setOpacity(on ? 90 : 0);
    }

    void setHighlight(bool on) {
        m_bg->setColor(on ? ccColor3B{ 50, 60, 100 } : ccColor3B{ 22, 22, 34 });
        m_bg->setOpacity(on ? 160 : 0);
    }
};

// ─────────────────────────────────────────────
//  Section header row
// ─────────────────────────────────────────────
class SectionHeader : public CCNode {
public:
    static SectionHeader* create(const char* title, ccColor3B accent, float w) {
        auto* h = new SectionHeader();
        if (h->init(title, accent, w)) { h->autorelease(); return h; }
        CC_SAFE_DELETE(h); return nullptr;
    }
    bool init(const char* title, ccColor3B accent, float w) {
        if (!CCNode::init()) return false;
        setContentSize({ w, 20.f });

        // accent line left
        auto* line = CCLayerColor::create({ accent.r, accent.g, accent.b, 180 }, 3.f, 14.f);
        line->ignoreAnchorPointForPosition(false);
        line->setAnchorPoint({ 0.f, 0.5f });
        line->setPosition({ 6.f, 10.f });
        addChild(line);

        auto* lbl = CCLabelBMFont::create(title, "bigFont.fnt");
        lbl->setScale(0.32f);
        lbl->setColor({ accent.r, accent.g, accent.b });
        lbl->setAnchorPoint({ 0.f, 0.5f });
        lbl->setPosition({ 14.f, 10.f });
        addChild(lbl);

        // thin separator line
        auto* sep = CCLayerColor::create({ 60, 60, 80, 120 }, w - 20.f, 1.f);
        sep->ignoreAnchorPointForPosition(false);
        sep->setAnchorPoint({ 0.f, 0.5f });
        sep->setPosition({ 14.f + lbl->getContentSize().width * 0.32f + 6.f, 10.f });
        addChild(sep);

        return true;
    }
};

// ─────────────────────────────────────────────
//  CrypticPopup
// ─────────────────────────────────────────────
class CrypticPopup : public geode::Popup<>, public TextInputDelegate {

    static constexpr int   TABS  = 4;
    static constexpr float ROW_H = 26.f;
    static constexpr float SEC_H = 20.f;
    static constexpr float GAP   = 2.f;

    // All setting keys — order matters (matches tag idx)
    static constexpr const char* KEYS[] = {
        "show-fps",         // 0
        "show-speed",       // 1
        "show-attempts",    // 2
        "show-best-run",    // 3
        "show-cps",         // 4
        "noclip",           // 5
        "instant-complete", // 6
        "unlock-all",       // 7
        "unlock-levels",    // 8
        "fake-100",         // 9
        "show-hitbox",      // 10
        "hitbox-on-death",  // 11
        "layout-mode",      // 12
    };
    static constexpr int KEY_COUNT = 13;

    int    m_tab    = 0;
    float  m_tabW   = 0.f;
    float  m_tabBarY = 0.f;

    CCNode*                m_pages[TABS]   = {};
    CCMenuItemSpriteExtra* m_tabBtns[TABS] = {};
    CCLayerColor*          m_underline     = nullptr;
    CCTextInputNode*       m_searchInput   = nullptr;

    std::vector<FeatureRow*> m_allRows;

    // ── Colours ───────────────────────────────
    ccColor3B accent() { return Cryptic::accentColor(); }
    ccColor3B accentDark() {
        auto c = accent();
        return { (GLubyte)(c.r / 5), (GLubyte)(c.g / 5), (GLubyte)(c.b / 5) };
    }
    ccColor3B accentMid() {
        auto c = accent();
        return { (GLubyte)(c.r / 2), (GLubyte)(c.g / 2), (GLubyte)(c.b / 2) };
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
    // Accepts mixed CCNode* (FeatureRow or SectionHeader)
    CCNode* buildPage(std::vector<CCNode*> items, float w, float h) {
        auto* pg = CCNode::create();
        pg->setContentSize({ w, h });

        float y = h - 6.f;
        int stripeIdx = 0;
        for (auto* item : items) {
            float itemH = item->getContentSize().height;
            y -= itemH;
            item->setPosition({ 4.f, y });

            auto* row = dynamic_cast<FeatureRow*>(item);
            if (row) {
                row->setStripe(stripeIdx % 2 == 0);
                m_allRows.push_back(row);
                stripeIdx++;
            }
            pg->addChild(item);
            y -= GAP;
        }
        return pg;
    }

    // Helper to cast FeatureRow* vector to CCNode* vector
    CCNode* buildPageRows(std::vector<FeatureRow*> rows, float w, float h) {
        std::vector<CCNode*> items(rows.begin(), rows.end());
        return buildPage(items, w, h);
    }

protected:
    bool setup() override {
        auto sz = m_mainLayer->getContentSize();
        float w = sz.width, h = sz.height;
        float rowW = w - 10.f;

        // ── Dark bg with subtle gradient feel ─
        auto* bg = CCLayerColor::create({ 12, 12, 18, 255 }, w, h);
        bg->ignoreAnchorPointForPosition(false);
        bg->setAnchorPoint({ 0.f, 0.f });
        m_mainLayer->addChild(bg, -1);

        // subtle top glow
        auto* glow = CCLayerColor::create(
            { accentDark().r, accentDark().g, accentDark().b, 60 }, w, 60.f
        );
        glow->ignoreAnchorPointForPosition(false);
        glow->setAnchorPoint({ 0.f, 0.f });
        glow->setPosition({ 0.f, h - 60.f });
        m_mainLayer->addChild(glow, -1);

        // ── Header bar ────────────────────────
        float hdrH = 28.f;
        auto* hdrBg = CCLayerColor::create(
            { accentDark().r, accentDark().g, accentDark().b, 255 }, w, hdrH
        );
        hdrBg->ignoreAnchorPointForPosition(false);
        hdrBg->setAnchorPoint({ 0.f, 0.f });
        hdrBg->setPosition({ 0.f, h - hdrH });
        m_mainLayer->addChild(hdrBg, 5);

        // accent bottom border on header
        auto* hdrLine = CCLayerColor::create(
            { accent().r, accent().g, accent().b, 200 }, w, 2.f
        );
        hdrLine->ignoreAnchorPointForPosition(false);
        hdrLine->setAnchorPoint({ 0.f, 0.f });
        hdrLine->setPosition({ 0.f, h - hdrH - 2.f });
        m_mainLayer->addChild(hdrLine, 6);

        // Title
        auto* titleLbl = CCLabelBMFont::create("cryptic", "bigFont.fnt");
        titleLbl->setScale(0.46f);
        titleLbl->setColor(accent());
        titleLbl->setAnchorPoint({ 0.f, 0.5f });
        titleLbl->setPosition({ 10.f, h - hdrH / 2.f });
        m_mainLayer->addChild(titleLbl, 6);

        // Version tag
        auto* verLbl = CCLabelBMFont::create("v1.0", "chatFont.fnt");
        verLbl->setScale(0.5f);
        verLbl->setColor({ 80, 80, 100 });
        verLbl->setAnchorPoint({ 0.f, 0.5f });
        verLbl->setPosition({ 10.f + titleLbl->getContentSize().width * 0.46f + 4.f, h - hdrH / 2.f - 2.f });
        m_mainLayer->addChild(verLbl, 6);

        // ── Search bar ────────────────────────
        float sW = 110.f, sH = 18.f;

        // search bg with rounded feel
        auto* sBg = CCLayerColor::create({ 20, 20, 32, 240 }, sW + 4.f, sH);
        sBg->ignoreAnchorPointForPosition(false);
        sBg->setAnchorPoint({ 1.f, 0.5f });
        sBg->setPosition({ w - 24.f, h - hdrH / 2.f });
        m_mainLayer->addChild(sBg, 6);

        // search icon
        auto* sIcon = CCLabelBMFont::create("?", "chatFont.fnt");
        sIcon->setScale(0.5f);
        sIcon->setColor({ 100, 100, 130 });
        sIcon->setAnchorPoint({ 0.f, 0.5f });
        sIcon->setPosition({ w - 24.f - sW - 2.f, h - hdrH / 2.f });
        m_mainLayer->addChild(sIcon, 7);

        m_searchInput = CCTextInputNode::create(sW - 4.f, sH - 4.f, "search...", "chatFont.fnt");
        m_searchInput->setMaxLabelScale(0.45f);
        m_searchInput->setDelegate(this);
        m_searchInput->setAnchorPoint({ 1.f, 0.5f });
        m_searchInput->setPosition({ w - 26.f, h - hdrH / 2.f });
        m_mainLayer->addChild(m_searchInput, 7);

        // ── Close btn ─────────────────────────
        if (m_closeBtn) m_closeBtn->setVisible(false);
        auto* xLbl = CCLabelBMFont::create("X", "bigFont.fnt");
        xLbl->setScale(0.38f);
        xLbl->setColor({ 210, 60, 60 });
        auto* xMenu = CCMenu::create();
        xMenu->setPosition({ w - 8.f, h - hdrH / 2.f });
        xMenu->addChild(CCMenuItemSpriteExtra::create(
            xLbl, this, menu_selector(CrypticPopup::onCloseBtn)
        ));
        m_mainLayer->addChild(xMenu, 10);

        // ── Tab bar ───────────────────────────
        float tabH = 22.f;
        float tabY = h - hdrH - 2.f - tabH;
        m_tabBarY  = tabY;
        m_tabW     = w / TABS;

        auto* tabBg = CCLayerColor::create({ 16, 16, 24, 255 }, w, tabH);
        tabBg->ignoreAnchorPointForPosition(false);
        tabBg->setAnchorPoint({ 0.f, 0.f });
        tabBg->setPosition({ 0.f, tabY });
        m_mainLayer->addChild(tabBg, 5);

        // vertical dividers between tabs
        for (int i = 1; i < TABS; i++) {
            auto* div = CCLayerColor::create({ 40, 40, 60, 150 }, 1.f, tabH - 6.f);
            div->ignoreAnchorPointForPosition(false);
            div->setAnchorPoint({ 0.f, 0.5f });
            div->setPosition({ m_tabW * i, tabY + tabH / 2.f });
            m_mainLayer->addChild(div, 6);
        }

        static const char* tabNames[TABS] = { "Global", "HUD", "Cheats", "Commands" };
        auto* tabMenu = CCMenu::create();
        tabMenu->setPosition({ 0.f, 0.f });
        tabMenu->setContentSize({ w, tabH });
        m_mainLayer->addChild(tabMenu, 6);

        for (int i = 0; i < TABS; i++) {
            auto* lbl = CCLabelBMFont::create(tabNames[i], "bigFont.fnt");
            lbl->setScale(0.34f);
            lbl->setColor(i == 0 ? accent() : ccColor3B{ 110, 110, 130 });

            auto* btn = CCMenuItemSpriteExtra::create(
                lbl, this, menu_selector(CrypticPopup::onTab)
            );
            btn->setTag(i);
            btn->setPosition({ m_tabW * i + m_tabW / 2.f, tabY + tabH / 2.f });
            m_tabBtns[i] = btn;
            tabMenu->addChild(btn);
        }

        // Underline indicator
        m_underline = CCLayerColor::create(
            { accent().r, accent().g, accent().b, 230 }, m_tabW, 2.f
        );
        m_underline->ignoreAnchorPointForPosition(false);
        m_underline->setAnchorPoint({ 0.f, 0.f });
        m_underline->setPosition({ 0.f, tabY });
        m_mainLayer->addChild(m_underline, 7);

        // ── Pages ─────────────────────────────
        float pageH = tabY - 4.f;

        // Tab 0 — Global (previously mislabeled, now actually global/unlocks)
        m_pages[0] = buildPage({
            (CCNode*)SectionHeader::create("Unlocks", accent(), rowW),
            (CCNode*)makeToggle("Unlock All Icons",    "unlock-all",     7, rowW),
            (CCNode*)makeToggle("Unlock All Levels",   "unlock-levels",  8, rowW),
            (CCNode*)makeToggle("100% All Levels",     "fake-100",       9, rowW),
        }, w, pageH);

        // Tab 1 — HUD
        m_pages[1] = buildPage({
            (CCNode*)SectionHeader::create("HUD Display", accent(), rowW),
            (CCNode*)makeToggle("FPS",        "show-fps",      0, rowW),
            (CCNode*)makeToggle("Speed",      "show-speed",    1, rowW),
            (CCNode*)makeToggle("Attempts",   "show-attempts", 2, rowW),
            (CCNode*)makeToggle("Best Run %", "show-best-run", 3, rowW),
            (CCNode*)makeToggle("CPS",        "show-cps",      4, rowW),
        }, w, pageH);

        // Tab 2 — Cheats
        m_pages[2] = buildPage({
            (CCNode*)SectionHeader::create("Gameplay", accent(), rowW),
            (CCNode*)makeToggle("Noclip",           "noclip",           5, rowW),
            (CCNode*)makeToggle("Instant Complete", "instant-complete", 6, rowW),
            (CCNode*)SectionHeader::create("Visual", accent(), rowW),
            (CCNode*)makeToggle("Show Hitbox",      "show-hitbox",      10, rowW),
            (CCNode*)makeToggle("Hitbox On Death",  "hitbox-on-death",  11, rowW),
            (CCNode*)makeToggle("Layout Mode",      "layout-mode",      12, rowW),
        }, w, pageH);

        // Tab 3 — Commands
        m_pages[3] = buildPage({
            (CCNode*)SectionHeader::create("Level", accent(), rowW),
            (CCNode*)makeCmd("Restart Level",    menu_selector(CrypticPopup::cmdRestart),  rowW),
            (CCNode*)makeCmd("Toggle Practice",  menu_selector(CrypticPopup::cmdPractice), rowW),
            (CCNode*)makeCmd("Copy Level ID",    menu_selector(CrypticPopup::cmdCopyID),   rowW),
            (CCNode*)makeCmd("Show Best %",      menu_selector(CrypticPopup::cmdBest),     rowW),
            (CCNode*)SectionHeader::create("Cheats", accent(), rowW),
            (CCNode*)makeCmd("Show Hitbox",      menu_selector(CrypticPopup::cmdHitbox),   rowW),
            (CCNode*)makeCmd("Hitbox On Death",  menu_selector(CrypticPopup::cmdHitboxDeath), rowW),
            (CCNode*)makeCmd("Layout Mode",      menu_selector(CrypticPopup::cmdLayout),   rowW),
        }, w, pageH);

        for (int i = 0; i < TABS; i++) {
            m_pages[i]->setPosition({ 0.f, 2.f });
            m_pages[i]->setVisible(i == 0);
            m_mainLayer->addChild(m_pages[i], 4);
        }

        // Bottom accent line
        auto* bot = CCLayerColor::create(
            { accent().r, accent().g, accent().b, 160 }, w, 2.f
        );
        bot->setPosition({ 0.f, 0.f });
        m_mainLayer->addChild(bot, 5);

        return true;
    }

    // ── Search — FIXED ────────────────────────
    // Hides rows that don't match, shows all pages while searching
    void textChanged(CCTextInputNode* input) override {
        std::string raw = input->getString();
        std::string lower = raw;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        bool searching = !lower.empty();

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

    // ── Tab switch ────────────────────────────
    void onTab(CCObject* s) {
        int tab = static_cast<CCNode*>(s)->getTag();
        if (tab == m_tab) return;
        m_tab = tab;

        // clear search when switching tabs
        if (m_searchInput) m_searchInput->setString("");

        for (int i = 0; i < TABS; i++) {
            m_pages[i]->setVisible(i == tab);
            if (auto* l = dynamic_cast<CCLabelBMFont*>(m_tabBtns[i]->getNormalImage()))
                l->setColor(i == tab ? accent() : ccColor3B{ 110, 110, 130 });
        }
        m_underline->setPositionX(tab * m_tabW);
    }

    void onCloseBtn(CCObject*) { onClose(nullptr); }

    // ── Toggle ────────────────────────────────
    void onToggle(CCObject* s) {
        int idx = static_cast<CCNode*>(s)->getTag();
        if (idx < 0 || idx >= KEY_COUNT) return;
        bool cur = Mod::get()->getSettingValue<bool>(KEYS[idx]);
        Mod::get()->setSettingValue(KEYS[idx], !cur);

        // Notifications for important toggles
        struct { int idx; const char* on; const char* off; } notifs[] = {
            { 5,  "Noclip ON",           "Noclip OFF"           },
            { 6,  "Instant Complete ON", "Instant Complete OFF" },
            { 10, "Show Hitbox ON",      "Show Hitbox OFF"      },
            { 11, "Hitbox On Death ON",  "Hitbox On Death OFF"  },
            { 12, "Layout Mode ON",      "Layout Mode OFF"      },
        };
        for (auto& n : notifs) {
            if (idx == n.idx) {
                Notification::create(!cur ? n.on : n.off,
                    !cur ? NotificationIcon::Success : NotificationIcon::Error, 1.2f)->show();
                break;
            }
        }
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

    void cmdHitbox(CCObject*) {
        bool cur = Mod::get()->getSettingValue<bool>("show-hitbox");
        Mod::get()->setSettingValue("show-hitbox", !cur);
        Notification::create(!cur ? "Hitbox ON" : "Hitbox OFF",
            !cur ? NotificationIcon::Success : NotificationIcon::Error, 1.2f)->show();
    }

    void cmdHitboxDeath(CCObject*) {
        bool cur = Mod::get()->getSettingValue<bool>("hitbox-on-death");
        Mod::get()->setSettingValue("hitbox-on-death", !cur);
        Notification::create(!cur ? "Hitbox On Death ON" : "Hitbox On Death OFF",
            !cur ? NotificationIcon::Success : NotificationIcon::Error, 1.2f)->show();
    }

    void cmdLayout(CCObject*) {
        bool cur = Mod::get()->getSettingValue<bool>("layout-mode");
        Mod::get()->setSettingValue("layout-mode", !cur);
        Notification::create(!cur ? "Layout Mode ON" : "Layout Mode OFF",
            !cur ? NotificationIcon::Success : NotificationIcon::Error, 1.2f)->show();
    }

public:
    static CrypticPopup* create() {
        auto* p = new CrypticPopup();
        if (p->initAnchored(340.f, 290.f)) { p->autorelease(); return p; }
        CC_SAFE_DELETE(p); return nullptr;
    }
};