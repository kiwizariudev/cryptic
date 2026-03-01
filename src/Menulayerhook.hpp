#pragma once
#include "Popup.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

// ─────────────────────────────────────────────
//  Shared button builder
//  Uses a simple GJ_optionsBtn sprite — small,
//  reliable, no lag, tinted with accent color.
// ─────────────────────────────────────────────
static CCMenuItemSpriteExtra* makeCrypticBtn(CCObject* target, SEL_MenuHandler sel) {
    auto* spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    if (spr) spr->setColor(Cryptic::accentColor());

    return CCMenuItemSpriteExtra::create(
        spr ? (CCNode*)spr
            : (CCNode*)CCLabelBMFont::create("C", "bigFont.fnt"),
        target, sel
    );
}

// ─────────────────────────────────────────────
//  MenuLayer hook
// ─────────────────────────────────────────────
class $modify(CrypticMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto* btn = makeCrypticBtn(this, menu_selector(CrypticMenuLayer::onOpen));
        btn->setID("cryptic-btn"_spr);

        if (auto* menu = getChildByID("bottom-menu")) {
            menu->addChild(btn);
            menu->updateLayout();
        }
        return true;
    }

    void onOpen(CCObject*) { CrypticPopup::create()->show(); }
};

// ─────────────────────────────────────────────
//  PauseLayer hook
// ─────────────────────────────────────────────
class $modify(CrypticPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto* btn = makeCrypticBtn(this, menu_selector(CrypticPauseLayer::onOpen));
        btn->setID("cryptic-pause-btn"_spr);

        auto* menu = CCMenu::create();
        auto  ws   = CCDirector::sharedDirector()->getWinSize();
        menu->setPosition({ 50.f, ws.height / 2.f + 90.f });
        menu->addChild(btn);
        this->addChild(menu, 10);
    }

    void onOpen(CCObject*) { CrypticPopup::create()->show(); }
};