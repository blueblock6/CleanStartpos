#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSettingsLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

LevelSettingsObject* settings;

#define addButton(sprite, tag, callback, condition)                             \
    btnSpr = CCSprite::createWithSpriteFrameName(sprite);                       \
    btn = CCMenuItemSpriteExtra::create(btnSpr, menu, menu_selector(callback)); \
    btn->setTag(tag);                                                           \
    if(condition != tag) btn->setColor({127, 127, 127});                      \
    menu->addChild(btn);

#define addProperty(name, tag, condition)                                                                       \
    label = CCLabelBMFont::create(name, "goldFont.fnt");                                                        \
    label->limitLabelWidth(75, 0.7f, 0.1f);                                                                     \
    label->setPositionY(start);                                                                                 \
    menu->addChild(label);                                                                                      \
    toggle = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::toggleProperty));  \
    toggle->setTag(tag);                                                                                        \
    toggle->toggle(condition);                                                                                  \
    toggle->setPositionY(start - 22);                                                                           \
    menu->addChild(toggle);                                                                                     \
    start -= 40;

class $modify(CleanStartpos, LevelSettingsLayer) {
    $override
    bool init(LevelSettingsObject* p0, LevelEditorLayer* p1) {
        if(!LevelSettingsLayer::init(p0, p1)) return false;

        if(p1 != nullptr) return true;

        settings = p0;
        auto center = CCDirector::sharedDirector()->getWinSize() / 2;

        // Move Stuff
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(0))->setContentSize({450, 190}); // BG
        static_cast<CCNode*>(m_buttonMenu->getChildren()->objectAtIndex(0))->setPositionY(-60); // OK
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(4))->setPositionX(center.width - 80); // Target Order Label
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(5))->setPositionX(center.width - 80); // Target Order BG
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(5))->setContentWidth(80); // Target Order BG
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(6))->setPositionX(center.width - 80); // Target Order Input
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(7))->setPositionX(center.width + 80); // Target Channel Label
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(8))->setPositionX(center.width + 80); // Target Channel BG
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(8))->setContentWidth(80); // Target Channel BG
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(9))->setPositionX(center.width + 80); // Target Channel Input

        // Hide Stuff
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(2))->setVisible(false); // Disable Label
        static_cast<CCNode*>(m_buttonMenu->getChildren()->objectAtIndex(1))->setVisible(false); // Disable Toggle
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(3))->setVisible(false); // Reset Camera Label
        static_cast<CCNode*>(m_buttonMenu->getChildren()->objectAtIndex(2))->setVisible(false); // Reset Camera Toggle
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(10))->setVisible(false); // Speed Label
        static_cast<CCNode*>(m_buttonMenu->getChildren()->objectAtIndex(3))->setVisible(false); // Speed Button
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(11))->setVisible(false); // Mode Label
        static_cast<CCNode*>(m_buttonMenu->getChildren()->objectAtIndex(4))->setVisible(false); // Mode Button
        static_cast<CCNode*>(m_mainLayer->getChildren()->objectAtIndex(12))->setVisible(false); // Options Label
        static_cast<CCNode*>(m_buttonMenu->getChildren()->objectAtIndex(5))->setVisible(false); // Options Button

        CCSprite* btnSpr;
        CCMenuItemSpriteExtra* btn;
        CCMenu* menu;
        CCLabelBMFont* label;
        float start;
        auto checkOn = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        checkOn->setScale(0.8f);
        auto checkOff = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        checkOff->setScale(0.8f);
        CCMenuItemToggler* toggle;

        menu = CCMenu::create();
        menu->setID("select-mode-menu"_spr);
        menu->setContentWidth(300.f);
        menu->setLayout(RowLayout::create());

        addButton("gj_iconBtn_off_001.png", 0, CleanStartpos::onCMode, p0->m_startMode);
        addButton("gj_shipBtn_off_001.png", 1, CleanStartpos::onCMode, p0->m_startMode);
        addButton("gj_ballBtn_off_001.png", 2, CleanStartpos::onCMode, p0->m_startMode);
        addButton("gj_birdBtn_off_001.png", 3, CleanStartpos::onCMode, p0->m_startMode);
        addButton("gj_dartBtn_off_001.png", 4, CleanStartpos::onCMode, p0->m_startMode);
        addButton("gj_robotBtn_off_001.png", 5, CleanStartpos::onCMode, p0->m_startMode);
        addButton("gj_spiderBtn_off_001.png", 6, CleanStartpos::onCMode, p0->m_startMode);
        addButton("gj_swingBtn_off_001.png", 7, CleanStartpos::onCMode, p0->m_startMode);

        menu->updateLayout(false);
        menu->setPosition(center + ccp(0, 70));
        m_mainLayer->addChild(menu);

        menu = CCMenu::create();
        menu->setID("select-speed-menu"_spr);
        menu->setContentWidth(300.f);
        menu->setLayout(RowLayout::create());

        addButton("boost_01_001.png", 1, CleanStartpos::onCSpeed, static_cast<int>(p0->m_startSpeed));
        addButton("boost_02_001.png", 0, CleanStartpos::onCSpeed, static_cast<int>(p0->m_startSpeed));
        addButton("boost_03_001.png", 2, CleanStartpos::onCSpeed, static_cast<int>(p0->m_startSpeed));
        addButton("boost_04_001.png", 3, CleanStartpos::onCSpeed, static_cast<int>(p0->m_startSpeed));
        addButton("boost_05_001.png", 4, CleanStartpos::onCSpeed, static_cast<int>(p0->m_startSpeed));

        menu->updateLayout(false);
        menu->setPosition(center + ccp(0, 10));
        m_mainLayer->addChild(menu);

        menu = CCMenu::create();
        menu->setID("left-properties-menu"_spr);
        menu->setContentSize({75, 160});
        start = 80;

        addProperty("Flip", 100, p0->m_isFlipped);
        addProperty("Mini", 101, p0->m_startMini);
        addProperty("Dual", 102, p0->m_startDual);
        addProperty("Mirror", 103, p0->m_mirrorMode);

        menu->setPosition(center + ccp(-180, 0));
        m_mainLayer->addChild(menu);

        menu = CCMenu::create();
        menu->setID("left-properties-menu"_spr);
        menu->setContentSize({75, 160});
        start = 80;
        
        addProperty("Rotate", 104, p0->m_rotateGameplay);
        addProperty("Reverse", 105, p0->m_reverseGameplay);
        addProperty("Reset Camera", 106, p0->m_resetCamera);
        addProperty("Disable", 107, p0->m_disableStartPos);

        menu->setPosition(center + ccp(180, 0));
        m_mainLayer->addChild(menu);

        handleTouchPriority(this);

        return true;
    }

    void onCMode(CCObject* sender) {
        settings->m_startMode = sender->getTag();
        CCObject* child;
        CCARRAY_FOREACH(static_cast<CCMenuItemSpriteExtra*>(sender)->getParent()->getChildren(), child) {
            static_cast<CCMenuItemSpriteExtra*>(child)->setColor({125, 125, 125});
        }
        static_cast<CCMenuItemSpriteExtra*>(sender)->setColor({255, 255, 255});
    }

    void onCSpeed(CCObject* sender) {
		settings->m_startSpeed = static_cast<Speed>(sender->getTag());
        CCObject* child;
        CCARRAY_FOREACH(static_cast<CCMenuItemSpriteExtra*>(sender)->getParent()->getChildren(), child) {
            static_cast<CCMenuItemSpriteExtra*>(child)->setColor({125, 125, 125});
        }
        static_cast<CCMenuItemSpriteExtra*>(sender)->setColor({255, 255, 255});
    }

    void toggleProperty(CCObject* sender) {
        auto state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
        switch(sender->getTag()) {
            case 100:
                settings->m_isFlipped = state;
                break;
            case 101:
                settings->m_startMini = state;
                break;
            case 102:
                settings->m_startDual = state;
                break;
            case 103:
                settings->m_mirrorMode = state;
                break;
            case 104:
                settings->m_rotateGameplay = state;
                break;
            case 105:
                settings->m_reverseGameplay = state;
                break;
            case 106:
                settings->m_resetCamera = state;
                break;
            case 107:
                settings->m_disableStartPos = state;
        }
    }
};

class $modify(PlayLayer) {
    $override
    void resetLevel() {
        PlayLayer::resetLevel();
        if(!m_startPosObject) return;
        toggleFlipped(m_startPosObject->m_startSettings->m_mirrorMode, true);
    }
};