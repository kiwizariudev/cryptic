#pragma once
#include "State.hpp"
#include "OrbFarm.hpp"

// ─────────────────────────────────────────────
//  FeatureRow
// ─────────────────────────────────────────────
class FeatureRow : public CCNode {
public:
    std::string            m_label;
    CCMenuItemToggler*     m_tog = nullptr;
    CCMenuItemSpriteExtra* m_btn = nullptr;

private:
    CCLayerColor*  m_bg  = nullptr;
    CCLabelBMFont* m_lbl = nullptr;

public:
    static FeatureRow* create(
        const std::string& label, const std::string& settingKey,
        bool isCommand, int keyIdx, CCObject* target,
        SEL_MenuHandler toggleSel, SEL_MenuHandler cmdSel, float w)
    {
        auto* r = new FeatureRow();
        r->m_label = label;
        if (r->init(label, settingKey, isCommand, keyIdx, target, toggleSel, cmdSel, w)) {
            r->autorelease(); return r;
        }
        CC_SAFE_DELETE(r); return nullptr;
    }

    bool init(
        const std::string& label, const std::string& settingKey,
        bool isCommand, int keyIdx, CCObject* target,
        SEL_MenuHandler toggleSel, SEL_MenuHandler cmdSel, float w)
    {
        if (!CCNode::init()) return false;
        setContentSize({ w, 28.f });

        m_bg = CCLayerColor::create({ 0,0,0,0 }, w, 28.f);
        m_bg->ignoreAnchorPointForPosition(false);
        m_bg->setAnchorPoint({ 0.f, 0.f });
        addChild(m_bg, 0);

        auto* pip = CCLayerColor::create({ 70,70,100,0 }, 2.f, 14.f);
        pip->ignoreAnchorPointForPosition(false);
        pip->setAnchorPoint({ 0.f, 0.5f });
        pip->setPosition({ 4.f, 14.f });
        pip->setTag(99);
        addChild(pip, 1);

        m_lbl = CCLabelBMFont::create(label.c_str(), "bigFont.fnt");
        m_lbl->setScale(0.38f);
        m_lbl->setAnchorPoint({ 0.f, 0.5f });
        m_lbl->setColor({ 210, 210, 225 });
        m_lbl->setPosition({ 14.f, 14.f });
        addChild(m_lbl, 1);

        auto* menu = CCMenu::create();
        menu->setPosition({ 0.f, 0.f });
        menu->setContentSize({ w, 28.f });
        addChild(menu, 2);

        if (isCommand) {
            auto* spr = ButtonSprite::create("Run", "bigFont.fnt", "GJ_button_04.png", 0.5f);
            spr->setScale(0.46f);
            m_btn = CCMenuItemSpriteExtra::create(spr, target, cmdSel);
            m_btn->setAnchorPoint({ 1.f, 0.5f });
            m_btn->setPosition({ w - 8.f, 14.f });
            menu->addChild(m_btn);
        } else {
            bool cur = Mod::get()->getSettingValue<bool>(settingKey.c_str());
            m_tog = CCMenuItemToggler::createWithStandardSprites(target, toggleSel, 0.54f);
            m_tog->toggle(cur);
            m_tog->setTag(keyIdx);
            m_tog->setAnchorPoint({ 1.f, 0.5f });
            m_tog->setPosition({ w - 8.f, 14.f });
            menu->addChild(m_tog);
        }
        return true;
    }

    void setStripe(bool on) {
        m_bg->setColor({ 20, 20, 32 });
        m_bg->setOpacity(on ? 100 : 0);
        if (auto* p = dynamic_cast<CCLayerColor*>(getChildByTag(99))) p->setOpacity(0);
    }

    void setHighlight(bool on) {
        m_bg->setColor({ 40, 52, 90 });
        m_bg->setOpacity(on ? 180 : 0);
        if (auto* p = dynamic_cast<CCLayerColor*>(getChildByTag(99))) p->setOpacity(on ? 200 : 0);
        if (m_lbl) m_lbl->setColor(on ? ccColor3B{255,255,255} : ccColor3B{210,210,225});
    }
};

// ─────────────────────────────────────────────
//  SectionHeader
// ─────────────────────────────────────────────
class SectionHeader : public CCNode {
public:
    static SectionHeader* create(const char* title, ccColor3B col, float w) {
        auto* h = new SectionHeader();
        if (h->init(title, col, w)) { h->autorelease(); return h; }
        CC_SAFE_DELETE(h); return nullptr;
    }
    bool init(const char* title, ccColor3B col, float w) {
        if (!CCNode::init()) return false;
        setContentSize({ w, 22.f });

        auto* bar = CCLayerColor::create({ col.r,col.g,col.b,180 }, w - 8.f, 1.f);
        bar->ignoreAnchorPointForPosition(false);
        bar->setAnchorPoint({ 0.f, 0.5f });
        bar->setPosition({ 4.f, 11.f });
        addChild(bar);

        auto* pill = CCLayerColor::create({ 11,11,17,255 }, 60.f, 14.f);
        pill->ignoreAnchorPointForPosition(false);
        pill->setAnchorPoint({ 0.f, 0.5f });
        pill->setPosition({ 10.f, 11.f });
        addChild(pill, 1);

        auto* lbl = CCLabelBMFont::create(title, "bigFont.fnt");
        lbl->setScale(0.3f);
        lbl->setColor({ col.r, col.g, col.b });
        lbl->setAnchorPoint({ 0.f, 0.5f });
        lbl->setPosition({ 12.f, 11.f });
        addChild(lbl, 2);
        return true;
    }
};

// ─────────────────────────────────────────────
//  SliderRow  (+/- stepper)
// ─────────────────────────────────────────────
class SliderRow : public CCNode {
    CCLabelBMFont* m_valLbl = nullptr;
    float          m_value;
    float          m_min, m_max, m_step;
    std::string    m_key;
    bool           m_isInt;
    std::function<void(float)> m_cb;

public:
    static SliderRow* create(const char* label, const std::string& key,
        float mn, float mx, float step, bool isInt, float w,
        std::function<void(float)> cb = nullptr)
    {
        auto* r = new SliderRow();
        if (r->init(label, key, mn, mx, step, isInt, w, cb)) { r->autorelease(); return r; }
        CC_SAFE_DELETE(r); return nullptr;
    }

    bool init(const char* label, const std::string& key,
        float mn, float mx, float step, bool isInt, float w,
        std::function<void(float)> cb)
    {
        if (!CCNode::init()) return false;
        setContentSize({ w, 28.f });
        m_key = key; m_min = mn; m_max = mx; m_step = step; m_isInt = isInt; m_cb = cb;

        if (isInt) {
            try { m_value = (float)Mod::get()->getSettingValue<int64_t>(key.c_str()); }
            catch(...) { m_value = mn; }
        } else {
            try { m_value = (float)Mod::get()->getSettingValue<double>(key.c_str()); }
            catch(...) { m_value = mn; }
        }

        auto* bg = CCLayerColor::create({ 20,20,32,100 }, w, 28.f);
        bg->ignoreAnchorPointForPosition(false);
        bg->setAnchorPoint({ 0.f, 0.f });
        addChild(bg, 0);

        auto* lbl = CCLabelBMFont::create(label, "bigFont.fnt");
        lbl->setScale(0.36f);
        lbl->setColor({ 190,190,210 });
        lbl->setAnchorPoint({ 0.f, 0.5f });
        lbl->setPosition({ 14.f, 14.f });
        addChild(lbl, 1);

        m_valLbl = CCLabelBMFont::create("", "bigFont.fnt");
        m_valLbl->setScale(0.38f);
        m_valLbl->setColor({ 255,220,100 });
        m_valLbl->setAnchorPoint({ 0.5f, 0.5f });
        m_valLbl->setPosition({ w - 52.f, 14.f });
        addChild(m_valLbl, 1);
        refreshLabel();

        auto* menu = CCMenu::create();
        menu->setPosition({ 0.f, 0.f });
        menu->setContentSize({ w, 28.f });
        addChild(menu, 2);

        auto* minusL = CCLabelBMFont::create("-", "bigFont.fnt");
        minusL->setScale(0.5f); minusL->setColor({ 200,100,100 });
        auto* minusB = CCMenuItemSpriteExtra::create(minusL, this, menu_selector(SliderRow::onMinus));
        minusB->setAnchorPoint({ 1.f, 0.5f });
        minusB->setPosition({ w - 68.f, 14.f });
        menu->addChild(minusB);

        auto* plusL = CCLabelBMFont::create("+", "bigFont.fnt");
        plusL->setScale(0.5f); plusL->setColor({ 100,200,100 });
        auto* plusB = CCMenuItemSpriteExtra::create(plusL, this, menu_selector(SliderRow::onPlus));
        plusB->setAnchorPoint({ 1.f, 0.5f });
        plusB->setPosition({ w - 8.f, 14.f });
        menu->addChild(plusB);

        return true;
    }

    void refreshLabel() {
        if (m_isInt) m_valLbl->setString(fmt::format("{}", (int)m_value).c_str());
        else         m_valLbl->setString(fmt::format("{:.2f}", m_value).c_str());
    }

    void save() {
        if (m_isInt) Mod::get()->setSettingValue<int64_t>(m_key.c_str(), (int64_t)m_value);
        else         Mod::get()->setSettingValue<double>(m_key.c_str(), (double)m_value);
        if (m_cb) m_cb(m_value);
    }

    void onMinus(CCObject*) { m_value = std::max(m_min, m_value - m_step); refreshLabel(); save(); }
    void onPlus(CCObject*)  { m_value = std::min(m_max, m_value + m_step); refreshLabel(); save(); }
};

// ─────────────────────────────────────────────
//  ColorPickRow  (inline RGB stepper)
// ─────────────────────────────────────────────
class ColorPickRow : public CCNode {
    CCLayerColor* m_preview = nullptr;
    int m_r, m_g, m_b;

public:
    static ColorPickRow* create(float w) {
        auto* r = new ColorPickRow();
        if (r->init(w)) { r->autorelease(); return r; }
        CC_SAFE_DELETE(r); return nullptr;
    }

    bool init(float w) {
        if (!CCNode::init()) return false;
        setContentSize({ w, 60.f });

        auto c = Cryptic::accentColor();
        m_r = c.r; m_g = c.g; m_b = c.b;

        auto* bg = CCLayerColor::create({ 14,14,24,120 }, w, 60.f);
        bg->ignoreAnchorPointForPosition(false);
        bg->setAnchorPoint({ 0.f, 0.f });
        addChild(bg, 0);

        auto* titleL = CCLabelBMFont::create("Accent Color", "bigFont.fnt");
        titleL->setScale(0.32f);
        titleL->setColor({ 160,160,180 });
        titleL->setAnchorPoint({ 0.f, 0.5f });
        titleL->setPosition({ 14.f, 50.f });
        addChild(titleL, 1);

        m_preview = CCLayerColor::create({ (GLubyte)m_r,(GLubyte)m_g,(GLubyte)m_b,255 }, 22.f, 22.f);
        m_preview->ignoreAnchorPointForPosition(false);
        m_preview->setAnchorPoint({ 1.f, 0.5f });
        m_preview->setPosition({ w - 8.f, 30.f });
        addChild(m_preview, 1);

        buildCh("R", 0, w, { 220,80,80  }, 42.f);
        buildCh("G", 1, w, { 80,220,80  }, 28.f);
        buildCh("B", 2, w, { 80,140,220 }, 14.f);

        return true;
    }

    void buildCh(const char* ch, int idx, float w, ccColor3B col, float y) {
        auto* lbl = CCLabelBMFont::create(ch, "bigFont.fnt");
        lbl->setScale(0.3f); lbl->setColor(col);
        lbl->setAnchorPoint({ 0.f, 0.5f });
        lbl->setPosition({ 14.f, y });
        addChild(lbl, 1);

        auto* vl = CCLabelBMFont::create("", "bigFont.fnt");
        vl->setScale(0.32f); vl->setColor({ 220,220,220 });
        vl->setAnchorPoint({ 0.5f, 0.5f });
        vl->setPosition({ w - 72.f, y });
        vl->setTag(200 + idx);
        addChild(vl, 1);
        refreshCh(idx);

        auto* menu = CCMenu::create();
        menu->setPosition({ 0.f, 0.f });
        menu->setContentSize({ w, 14.f });
        addChild(menu, 2);

        auto* mL = CCLabelBMFont::create("-", "bigFont.fnt");
        mL->setScale(0.44f); mL->setColor({ 200,100,100 });
        auto* mB = CCMenuItemSpriteExtra::create(mL, this, menu_selector(ColorPickRow::onMinus));
        mB->setTag(idx); mB->setAnchorPoint({ 1.f,0.5f }); mB->setPosition({ w - 86.f, y });
        menu->addChild(mB);

        auto* pL = CCLabelBMFont::create("+", "bigFont.fnt");
        pL->setScale(0.44f); pL->setColor({ 100,200,100 });
        auto* pB = CCMenuItemSpriteExtra::create(pL, this, menu_selector(ColorPickRow::onPlus));
        pB->setTag(idx); pB->setAnchorPoint({ 1.f,0.5f }); pB->setPosition({ w - 36.f, y });
        menu->addChild(pB);
    }

    void refreshCh(int idx) {
        int v = idx==0 ? m_r : idx==1 ? m_g : m_b;
        if (auto* l = dynamic_cast<CCLabelBMFont*>(getChildByTag(200+idx)))
            l->setString(fmt::format("{}", v).c_str());
    }

    void applyColor() {
        m_preview->setColor({ (GLubyte)m_r,(GLubyte)m_g,(GLubyte)m_b });
        Mod::get()->setSettingValue("accent-color",
            ccColor3B{ (GLubyte)m_r,(GLubyte)m_g,(GLubyte)m_b });
    }

    void onMinus(CCObject* s) {
        int idx = static_cast<CCNode*>(s)->getTag();
        int& ch = idx==0?m_r:idx==1?m_g:m_b;
        ch = std::max(0, ch - 10);
        refreshCh(idx); applyColor();
    }

    void onPlus(CCObject* s) {
        int idx = static_cast<CCNode*>(s)->getTag();
        int& ch = idx==0?m_r:idx==1?m_g:m_b;
        ch = std::min(255, ch + 10);
        refreshCh(idx); applyColor();
    }
};

// ─────────────────────────────────────────────
//  CrypticPopup
// ─────────────────────────────────────────────
class CrypticPopup : public geode::Popup<> {

    static constexpr int   TABS = 5;
    static constexpr float GAP  = 1.f;

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
        "jump-hack",        // 13
        "slow-mode",        // 14
    };
    static constexpr int KEY_COUNT = 15;

    int   m_tab  = 0;
    float m_tabW = 0.f, m_tabBarY = 0.f;

    CCNode*                m_pages[TABS]   = {};
    CCMenuItemSpriteExtra* m_tabBtns[TABS] = {};
    CCLayerColor*          m_underline     = nullptr;
    CCTextInputNode*       m_searchInput   = nullptr;
    std::string            m_lastSearch;

    std::vector<FeatureRow*> m_allRows;

    ccColor3B accent() { return Cryptic::accentColor(); }
    ccColor3B accentDark() {
        auto c = accent();
        return { (GLubyte)(c.r/5),(GLubyte)(c.g/5),(GLubyte)(c.b/5) };
    }

    FeatureRow* mkTog(const char* lbl, const char* key, int idx, float w) {
        return FeatureRow::create(lbl, key, false, idx, this,
            menu_selector(CrypticPopup::onToggle), nullptr, w);
    }
    FeatureRow* mkCmd(const char* lbl, SEL_MenuHandler sel, float w) {
        return FeatureRow::create(lbl, "", true, -1, this, nullptr, sel, w);
    }

    CCNode* buildPage(std::vector<CCNode*> items, float w, float h) {
        auto* pg = CCNode::create();
        pg->setContentSize({ w, h });
        float y = h - 4.f;
        int stripe = 0;
        for (auto* item : items) {
            y -= item->getContentSize().height;
            item->setPosition({ 4.f, y });
            if (auto* row = dynamic_cast<FeatureRow*>(item)) {
                row->setStripe(stripe++ % 2 == 0);
                m_allRows.push_back(row);
            }
            pg->addChild(item);
            y -= GAP;
        }
        return pg;
    }

protected:
    bool setup() override {
        auto sz = m_mainLayer->getContentSize();
        float w = sz.width, h = sz.height, rW = w - 10.f;

        // BG
        auto* bg = CCLayerColor::create({ 11,11,17,255 }, w, h);
        bg->ignoreAnchorPointForPosition(false); bg->setAnchorPoint({ 0.f,0.f });
        m_mainLayer->addChild(bg, -1);

        auto* glow = CCLayerColor::create(
            { accentDark().r,accentDark().g,accentDark().b,80 }, w, 55.f);
        glow->ignoreAnchorPointForPosition(false); glow->setAnchorPoint({ 0.f,0.f });
        glow->setPosition({ 0.f, h-55.f });
        m_mainLayer->addChild(glow, -1);

        // Header
        float hH = 30.f;
        auto* hBg = CCLayerColor::create(
            { accentDark().r,accentDark().g,accentDark().b,255 }, w, hH);
        hBg->ignoreAnchorPointForPosition(false); hBg->setAnchorPoint({ 0.f,0.f });
        hBg->setPosition({ 0.f, h-hH });
        m_mainLayer->addChild(hBg, 5);

        auto* hLine = CCLayerColor::create({ accent().r,accent().g,accent().b,220 }, w, 2.f);
        hLine->ignoreAnchorPointForPosition(false); hLine->setAnchorPoint({ 0.f,0.f });
        hLine->setPosition({ 0.f, h-hH-2.f });
        m_mainLayer->addChild(hLine, 6);

        auto* title = CCLabelBMFont::create("cryptic", "bigFont.fnt");
        title->setScale(0.48f); title->setColor(accent());
        title->setAnchorPoint({ 0.f,0.5f }); title->setPosition({ 10.f, h-hH/2.f });
        m_mainLayer->addChild(title, 6);

        // Search
        auto* sBg = CCLayerColor::create({ 18,18,28,230 }, 98.f, 20.f);
        sBg->ignoreAnchorPointForPosition(false); sBg->setAnchorPoint({ 1.f,0.5f });
        sBg->setPosition({ w-22.f, h-hH/2.f });
        m_mainLayer->addChild(sBg, 6);

        m_searchInput = CCTextInputNode::create(90.f, 18.f, "search...", "chatFont.fnt");
        m_searchInput->setMaxLabelScale(0.46f);
        m_searchInput->setAnchorPoint({ 1.f,0.5f });
        m_searchInput->setPosition({ w-24.f, h-hH/2.f });
        m_mainLayer->addChild(m_searchInput, 7);

        // Close
        if (m_closeBtn) m_closeBtn->setVisible(false);
        auto* xL = CCLabelBMFont::create("X", "bigFont.fnt");
        xL->setScale(0.38f); xL->setColor({ 210,55,55 });
        auto* xM = CCMenu::create();
        xM->setPosition({ w-8.f, h-hH/2.f });
        xM->addChild(CCMenuItemSpriteExtra::create(
            xL, this, menu_selector(CrypticPopup::onCloseBtn)));
        m_mainLayer->addChild(xM, 10);

        // Tab bar
        float tH = 22.f, tY = h-hH-2.f-tH;
        m_tabBarY = tY; m_tabW = w / TABS;

        auto* tBg = CCLayerColor::create({ 14,14,22,255 }, w, tH);
        tBg->ignoreAnchorPointForPosition(false); tBg->setAnchorPoint({ 0.f,0.f });
        tBg->setPosition({ 0.f, tY });
        m_mainLayer->addChild(tBg, 5);

        for (int i = 1; i < TABS; i++) {
            auto* div = CCLayerColor::create({ 35,35,55,160 }, 1.f, tH-8.f);
            div->ignoreAnchorPointForPosition(false); div->setAnchorPoint({ 0.5f,0.5f });
            div->setPosition({ m_tabW*i, tY+tH/2.f });
            m_mainLayer->addChild(div, 6);
        }

        static const char* tNames[TABS] = { "Global","HUD","Cheats","Commands","Style" };
        auto* tMenu = CCMenu::create();
        tMenu->setPosition({ 0.f,0.f }); tMenu->setContentSize({ w, tH });
        m_mainLayer->addChild(tMenu, 6);

        for (int i = 0; i < TABS; i++) {
            auto* lbl = CCLabelBMFont::create(tNames[i], "bigFont.fnt");
            lbl->setScale(0.30f);
            lbl->setColor(i == 0 ? accent() : ccColor3B{ 105,105,125 });
            auto* btn = CCMenuItemSpriteExtra::create(
                lbl, this, menu_selector(CrypticPopup::onTab));
            btn->setTag(i);
            btn->setPosition({ m_tabW*i + m_tabW/2.f, tY+tH/2.f });
            m_tabBtns[i] = btn;
            tMenu->addChild(btn);
        }

        m_underline = CCLayerColor::create(
            { accent().r,accent().g,accent().b,240 }, m_tabW, 2.f);
        m_underline->ignoreAnchorPointForPosition(false);
        m_underline->setAnchorPoint({ 0.f,0.f });
        m_underline->setPosition({ 0.f, tY });
        m_mainLayer->addChild(m_underline, 7);

        float pH = tY - 4.f;

        // Page 0 — Global
        m_pages[0] = buildPage({
            (CCNode*)SectionHeader::create("Unlocks", accent(), rW),
            (CCNode*)mkTog("Unlock All Icons",  "unlock-all",    7, rW),
            (CCNode*)mkTog("Unlock All Levels", "unlock-levels", 8, rW),
            (CCNode*)mkTog("100% All Levels",   "fake-100",      9, rW),
        }, w, pH);

        // Page 1 — HUD
        m_pages[1] = buildPage({
            (CCNode*)SectionHeader::create("HUD Display", accent(), rW),
            (CCNode*)mkTog("FPS",        "show-fps",      0, rW),
            (CCNode*)mkTog("Speed",      "show-speed",    1, rW),
            (CCNode*)mkTog("Attempts",   "show-attempts", 2, rW),
            (CCNode*)mkTog("Best Run %", "show-best-run", 3, rW),
            (CCNode*)mkTog("CPS",        "show-cps",      4, rW),
        }, w, pH);

        // Page 2 — Cheats
        m_pages[2] = buildPage({
            (CCNode*)SectionHeader::create("Gameplay", accent(), rW),
            (CCNode*)mkTog("Noclip",           "noclip",           5, rW),
            (CCNode*)mkTog("Instant Complete", "instant-complete", 6, rW),
            (CCNode*)mkTog("Jump Hack",        "jump-hack",        13, rW),
            (CCNode*)mkTog("Slow Mode",        "slow-mode",        14, rW),
            (CCNode*)SectionHeader::create("Visual", accent(), rW),
            (CCNode*)mkTog("Show Hitbox",      "show-hitbox",      10, rW),
            (CCNode*)mkTog("Hitbox On Death",  "hitbox-on-death",  11, rW),
            (CCNode*)mkTog("Layout Mode",      "layout-mode",      12, rW),
        }, w, pH);

        // Page 3 — Commands
        m_pages[3] = buildPage({
            (CCNode*)SectionHeader::create("Level", accent(), rW),
            (CCNode*)mkCmd("Restart Level",    menu_selector(CrypticPopup::cmdRestart),     rW),
            (CCNode*)mkCmd("Toggle Practice",  menu_selector(CrypticPopup::cmdPractice),    rW),
            (CCNode*)mkCmd("Copy Level ID",    menu_selector(CrypticPopup::cmdCopyID),      rW),
            (CCNode*)mkCmd("Show Best %",      menu_selector(CrypticPopup::cmdBest),        rW),
            (CCNode*)SectionHeader::create("Visual", accent(), rW),
            (CCNode*)mkCmd("Toggle Hitbox",    menu_selector(CrypticPopup::cmdHitbox),      rW),
            (CCNode*)mkCmd("Hitbox On Death",  menu_selector(CrypticPopup::cmdHitboxDeath), rW),
            (CCNode*)mkCmd("Layout Mode",      menu_selector(CrypticPopup::cmdLayout),      rW),
            (CCNode*)SectionHeader::create("Orb Farm", accent(), rW),
            (CCNode*)mkCmd("Start Farm",       menu_selector(CrypticPopup::cmdFarmStart),   rW),
            (CCNode*)mkCmd("Stop Farm",        menu_selector(CrypticPopup::cmdFarmStop),    rW),
            (CCNode*)mkCmd("Farm Status",      menu_selector(CrypticPopup::cmdFarmStatus),  rW),
        }, w, pH);

        // Page 4 — Style
        {
            auto* pg = CCNode::create();
            pg->setContentSize({ w, pH });
            float y = pH - 4.f;

            auto place = [&](CCNode* n) {
                y -= n->getContentSize().height;
                n->setPosition({ 4.f, y });
                pg->addChild(n);
                y -= GAP;
            };

            place(ColorPickRow::create(rW));

            place(SectionHeader::create("Slow Mode Speed", accent(), rW));
            place(SliderRow::create("Speed", "slow-mode-speed",
                0.1f, 1.0f, 0.1f, false, rW,
                [](float v) {
                    if (Mod::get()->getSettingValue<bool>("slow-mode"))
                        CCDirector::get()->getScheduler()->setTimeScale(v);
                }));

            place(SectionHeader::create("Jump Hack", accent(), rW));
            place(SliderRow::create("Extra Jumps", "jump-hack-count",
                1.f, 10.f, 1.f, true, rW, nullptr));

            place(SectionHeader::create("Orb Farm", accent(), rW));
            place(SliderRow::create("Orb Target", "orb-farm-target",
                1000.f, 100000.f, 1000.f, true, rW, nullptr));

            m_pages[4] = pg;
        }

        for (int i = 0; i < TABS; i++) {
            m_pages[i]->setPosition({ 0.f, 2.f });
            m_pages[i]->setVisible(i == 0);
            m_mainLayer->addChild(m_pages[i], 4);
        }

        auto* bot = CCLayerColor::create(
            { accent().r,accent().g,accent().b,170 }, w, 2.f);
        bot->setPosition({ 0.f, 0.f });
        m_mainLayer->addChild(bot, 5);

        this->schedule(schedule_selector(CrypticPopup::pollSearch), 0.05f);
        return true;
    }

    // ── Search poll ───────────────────────────
    void pollSearch(float) {
        if (!m_searchInput) return;
        std::string raw = m_searchInput->getString();
        if (raw == m_lastSearch) return;
        m_lastSearch = raw;

        std::string lower = raw;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        bool s = !lower.empty();

        for (int i = 0; i < TABS; i++)
            m_pages[i]->setVisible(s || i == m_tab);

        for (auto* row : m_allRows) {
            std::string lbl = row->m_label;
            std::transform(lbl.begin(), lbl.end(), lbl.begin(), ::tolower);
            bool match = !s || lbl.find(lower) != std::string::npos;
            row->setVisible(match);
            row->setHighlight(match && s);
            if (!match) row->setStripe(false);
        }
    }

    // ── Tab ───────────────────────────────────
    void onTab(CCObject* s) {
        int tab = static_cast<CCNode*>(s)->getTag();
        if (tab == m_tab) return;
        m_tab = tab;
        if (m_searchInput) m_searchInput->setString("");
        m_lastSearch = "";

        int stripe = 0;
        for (auto* row : m_allRows) {
            row->setVisible(true);
            row->setHighlight(false);
            row->setStripe(stripe++ % 2 == 0);
        }
        for (int i = 0; i < TABS; i++) {
            m_pages[i]->setVisible(i == tab);
            if (auto* l = dynamic_cast<CCLabelBMFont*>(m_tabBtns[i]->getNormalImage()))
                l->setColor(i == tab ? accent() : ccColor3B{105,105,125});
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

        if (idx == 14) {
            if (!cur) {
                float spd = 0.5f;
                try { spd = (float)Mod::get()->getSettingValue<double>("slow-mode-speed"); }
                catch(...) {}
                CCDirector::get()->getScheduler()->setTimeScale(spd);
            } else {
                CCDirector::get()->getScheduler()->setTimeScale(1.f);
            }
        }

        struct { int i; const char* on; const char* off; } msgs[] = {
            {  5, "Noclip ON",           "Noclip OFF"           },
            {  6, "Instant Complete ON", "Instant Complete OFF" },
            { 10, "Show Hitbox ON",      "Show Hitbox OFF"      },
            { 11, "Hitbox On Death ON",  "Hitbox On Death OFF"  },
            { 12, "Layout Mode ON",      "Layout Mode OFF"      },
            { 13, "Jump Hack ON",        "Jump Hack OFF"        },
            { 14, "Slow Mode ON",        "Slow Mode OFF"        },
        };
        for (auto& m : msgs)
            if (idx == m.i) {
                Notification::create(!cur ? m.on : m.off,
                    !cur ? NotificationIcon::Success : NotificationIcon::Error, 1.2f)->show();
                break;
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
        Notification::create(fmt::format("Copied ID: {}", (int)pl->m_level->m_levelID),
            NotificationIcon::Success, 2.f)->show();
    }

    void cmdBest(CCObject*) {
        if (!checkLevel()) return;
        auto* pl = PlayLayer::get();
        if (!pl->m_level) return;
        FLAlertLayer::create(nullptr, "Best %",
            fmt::format("All-time: <cy>{:.0f}%</c>\nSession: <cg>{:.1f}%</c>",
                (float)pl->m_level->m_normalPercent, Cryptic::sessionBest).c_str(),
            "OK", nullptr)->show();
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

    void cmdFarmStart(CCObject*) {
        Cryptic::startOrbFarm((int)Mod::get()->getSettingValue<int64_t>("orb-farm-target"));
    }

    void cmdFarmStop(CCObject*) {
        Cryptic::stopOrbFarm();
    }

    void cmdFarmStatus(CCObject*) {
        FLAlertLayer::create(nullptr, "Orb Farm",
            Cryptic::orbFarmStatus().c_str(), "OK", nullptr)->show();
    }

public:
    static CrypticPopup* create() {
        auto* p = new CrypticPopup();
        if (p->initAnchored(340.f, 300.f)) { p->autorelease(); return p; }
        CC_SAFE_DELETE(p); return nullptr;
    }
};