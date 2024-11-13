#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSettingsLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

LevelSettingsObject* settings;

#define addButton(sprite, tag, callback, condition)                                                                     \
    btn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName(sprite), menu, menu_selector(callback));    \
    btn->setTag(tag);                                                                                                   \
    if(condition != tag) btn->setColor({127, 127, 127});                                                              \
    menu->addChild(btn);

#define addToggle(name, tag, condition)                                                                         \
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
        m_mainLayer->getChildByType(0)->setContentSize({450, 190}); // BG
        m_buttonMenu->getChildByType(0)->setPositionY(-60); // OK
        m_mainLayer->getChildByType(4)->setPositionX(center.width - 80); // Target Order Label
        m_mainLayer->getChildByType(5)->setPositionX(center.width - 80); // Target Order BG
        m_mainLayer->getChildByType(5)->setContentWidth(80); // Target Order BG
        m_mainLayer->getChildByType(6)->setPositionX(center.width - 80); // Target Order Input
        m_mainLayer->getChildByType(7)->setPositionX(center.width + 80); // Target Channel Label
        m_mainLayer->getChildByType(8)->setPositionX(center.width + 80); // Target Channel BG
        m_mainLayer->getChildByType(8)->setContentWidth(80); // Target Channel BG
        m_mainLayer->getChildByType(9)->setPositionX(center.width + 80); // Target Channel Input

        // Hide Stuff
        m_mainLayer->getChildByType(2)->setVisible(false); // Disable Label
        m_buttonMenu->getChildByType(1)->setVisible(false); // Disable Toggle
        m_mainLayer->getChildByType(3)->setVisible(false); // Reset Camera Label
        m_buttonMenu->getChildByType(2)->setVisible(false); // Reset Camera Toggle
        m_mainLayer->getChildByType(10)->setVisible(false); // Speed Label
        m_buttonMenu->getChildByType(3)->setVisible(false); // Speed Button
        m_mainLayer->getChildByType(11)->setVisible(false); // Mode Label
        m_buttonMenu->getChildByType(4)->setVisible(false); // Mode Button
        m_mainLayer->getChildByType(12)->setVisible(false); // Options Label
        m_buttonMenu->getChildByType(5)->setVisible(false); // Options Button

        CCMenuItemSpriteExtra* btn;
        auto menu = CCMenu::create();
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

        CCLabelBMFont* label;
        float start = 80;
        auto checkOn = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        checkOn->setScale(0.8f);
        auto checkOff = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        checkOff->setScale(0.8f);
        CCMenuItemToggler* toggle;

        menu = CCMenu::create();
        menu->setID("left-properties-menu"_spr);
        menu->setContentSize({75, 160});

        addToggle("Flip", 100, p0->m_isFlipped);
        addToggle("Mini", 101, p0->m_startMini);
        addToggle("Dual", 102, p0->m_startDual);
        addToggle("Mirror", 103, p0->m_mirrorMode);

        menu->setPosition(center + ccp(-180, 0));
        m_mainLayer->addChild(menu);

        menu = CCMenu::create();
        menu->setID("left-properties-menu"_spr);
        menu->setContentSize({75, 160});
        start = 80;
        
        addToggle("Rotate", 104, p0->m_rotateGameplay);
        addToggle("Reverse", 105, p0->m_reverseGameplay);
        addToggle("Reset Camera", 106, p0->m_resetCamera);
        addToggle("Disable", 107, p0->m_disableStartPos);

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
        if(m_startPosObject->m_startSettings->m_mirrorMode) toggleFlipped(true, true);
    }
};