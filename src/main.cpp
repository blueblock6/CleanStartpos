#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSettingsLayer.hpp>

using namespace geode::prelude;

LevelSettingsObject* settings;

class $modify(CleanStartpos, LevelSettingsLayer) {
    $override
    bool init(LevelSettingsObject* p0, LevelEditorLayer* p1) {
        if(!LevelSettingsLayer::init(p0, p1)) return false;

        if(p1 != nullptr) return true;

        settings = p0;
        auto layer = static_cast<CCLayer*>(this->getChildren()->objectAtIndex(0));
        auto menu = static_cast<CCMenu*>(layer->getChildren()->objectAtIndex(1));

        auto center = CCDirector::sharedDirector()->getWinSize() / 2;

        // Menu BG
        static_cast<CCNode*>(layer->getChildren()->objectAtIndex(0))->setContentSize(ccp(500, 190));
        // OK
        static_cast<CCNode*>(menu->getChildren()->objectAtIndex(0))->setPositionY(-65);
        // Target Order
        static_cast<CCNode*>(layer->getChildren()->objectAtIndex(4))->setPosition(center + ccp(-100, -35));
        static_cast<CCNode*>(layer->getChildren()->objectAtIndex(5))->setPosition(center + ccp(-100, -65));
        static_cast<CCNode*>(layer->getChildren()->objectAtIndex(5))->setContentSize(ccp(80, 30));
        static_cast<CCNode*>(layer->getChildren()->objectAtIndex(6))->setPosition(center + ccp(-100, -65));
        // Target Channel
        static_cast<CCNode*>(layer->getChildren()->objectAtIndex(7))->setPosition(center + ccp(100, -35));
        static_cast<CCNode*>(layer->getChildren()->objectAtIndex(8))->setPosition(center + ccp(100, -65));
        static_cast<CCNode*>(layer->getChildren()->objectAtIndex(8))->setContentSize(ccp(80, 30));
        static_cast<CCNode*>(layer->getChildren()->objectAtIndex(9))->setPosition(center + ccp(100, -65));
        // Disable
        static_cast<CCNode*>(menu->getChildren()->objectAtIndex(1))->setPosition(ccp(210, -60));
        static_cast<CCNode*>(layer->getChildren()->objectAtIndex(2))->setPosition(center + ccp(210, -37.5f));
        // Reset Camera
        static_cast<CCNode*>(menu->getChildren()->objectAtIndex(2))->setPosition(ccp(210, -20));
        static_cast<CCNode*>(menu->getChildren()->objectAtIndex(2))->setScale(1.143f);
		auto resetText = CCLabelBMFont::create("Reset Camera", "goldFont.fnt");
        resetText->setPosition(center + ccp(210, 2.5f));
        resetText->setScale(0.4f);
        layer->addChild(resetText);

		// Remove unused elements
        layer->getChildren()->removeObjectAtIndex(12);
        layer->getChildren()->removeObjectAtIndex(11);
        layer->getChildren()->removeObjectAtIndex(10);
        layer->getChildren()->removeObjectAtIndex(3);
        menu->getChildren()->removeObjectAtIndex(5);
        menu->getChildren()->removeObjectAtIndex(4);
        menu->getChildren()->removeObjectAtIndex(3);
        
        auto checkOn = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto checkOff = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

        // Properties
        auto reverse = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        reverse->setPosition(ccp(210, 20));
        reverse->setScale(0.8f);
        reverse->setTag(100);
        reverse->toggle(p0->m_reverseGameplay);
        menu->addChild(reverse);
        
        auto reverseText = CCLabelBMFont::create("Reverse", "goldFont.fnt");
        reverseText->setPosition(center + ccp(210, 42.5f));
        reverseText->setScale(0.6f);
        layer->addChild(reverseText);
        
        auto rotate = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        rotate->setPosition(ccp(210, 60));
        rotate->setScale(0.8f);
        rotate->setTag(101);
        rotate->toggle(p0->m_rotateGameplay);
        menu->addChild(rotate);
        
        auto rotateText = CCLabelBMFont::create("Rotate", "goldFont.fnt");
        rotateText->setPosition(center + ccp(210, 82.5f));
        rotateText->setScale(0.6f);
        layer->addChild(rotateText);
        
        auto mirror = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        mirror->setPosition(ccp(-210, -60));
        mirror->setScale(0.8f);
        mirror->setTag(102);
        mirror->toggle(p0->m_mirrorMode);
        menu->addChild(mirror);
        
        auto mirrorText = CCLabelBMFont::create("Mirror", "goldFont.fnt");
        mirrorText->setPosition(center + ccp(-210, -37.5f));
        mirrorText->setScale(0.6f);
        layer->addChild(mirrorText);
        
        auto dual = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        dual->setPosition(ccp(-210, -20));
        dual->setScale(0.8f);
        dual->setTag(103);
        dual->toggle(p0->m_startDual);
        menu->addChild(dual);
        
        auto dualText = CCLabelBMFont::create("Dual", "goldFont.fnt");
        dualText->setPosition(center + ccp(-210, 2.5f));
        dualText->setScale(0.6f);
        layer->addChild(dualText);
        
        auto mini = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        mini->setPosition(ccp(-210, 20));
        mini->setScale(0.8f);
        mini->setTag(104);
        mini->toggle(p0->m_startMini);
        menu->addChild(mini);
        
        auto miniText = CCLabelBMFont::create("Mini", "goldFont.fnt");
        miniText->setPosition(center + ccp(-210, 42.5f));
        miniText->setScale(0.6f);
        layer->addChild(miniText);
        
        auto flip = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        flip->setPosition(ccp(-210, 60));
        flip->setScale(0.8f);
        flip->setTag(105);
        flip->toggle(p0->m_isFlipped);
        menu->addChild(flip);
        
        auto flipText = CCLabelBMFont::create("Flip", "goldFont.fnt");
        flipText->setPosition(center + ccp(-210, 82.5f));
        flipText->setScale(0.6f);
        layer->addChild(flipText);

        // Gamemodes
        auto gamemodes = CCMenu::create();
        gamemodes->setTouchPriority(-510);
        gamemodes->setPositionY(center.height + 65);
        gamemodes->setContentSize(ccp(300, 33.5f));
        gamemodes->setLayout(RowLayout::create());

        auto icon = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_iconBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        icon->setTag(0);
        gamemodes->addChild(icon);

        auto ship = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_shipBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        ship->setTag(1);
        gamemodes->addChild(ship);

        auto ball = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_ballBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        ball->setTag(2);
        gamemodes->addChild(ball);

        auto bird = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_birdBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        bird->setTag(3);
        gamemodes->addChild(bird);

        auto dart = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_dartBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        dart->setTag(4);
        gamemodes->addChild(dart);

        auto robot = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_robotBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        robot->setTag(5);
        gamemodes->addChild(robot);

        auto spider = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_spiderBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        spider->setTag(6);
        gamemodes->addChild(spider);

        auto swing = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_swingBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        swing->setTag(7);
        gamemodes->addChild(swing);

        CCObject* child;
        CCARRAY_FOREACH(gamemodes->getChildren(), child) {
            if(p0->m_startMode != child->getTag()) static_cast<CCMenuItemSpriteExtra*>(child)->setColor({125, 125, 125});
        }

        gamemodes->updateLayout();
        layer->addChild(gamemodes);
        
        // Speeds
        auto speeds = CCMenu::create();
        speeds->setTouchPriority(-512);
        speeds->setPositionY(center.height + 6);
        speeds->setContentSize(ccp(400, 200));
        speeds->setLayout(RowLayout::create());

        auto slowSpeed = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("boost_01_001.png"), this, menu_selector(CleanStartpos::setSpeed));
        slowSpeed->setTag(1);
        speeds->addChild(slowSpeed);

        auto normalSpeed = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("boost_02_001.png"), this, menu_selector(CleanStartpos::setSpeed));
        normalSpeed->setTag(0);
        speeds->addChild(normalSpeed);

        auto doubleSpeed = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("boost_03_001.png"), this, menu_selector(CleanStartpos::setSpeed));
        doubleSpeed->setTag(2);
        speeds->addChild(doubleSpeed);

        auto tripleSpeed = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("boost_04_001.png"), this, menu_selector(CleanStartpos::setSpeed));
        tripleSpeed->setTag(3);
        speeds->addChild(tripleSpeed);

        auto quadSpeed = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("boost_05_001.png"), this, menu_selector(CleanStartpos::setSpeed));
        quadSpeed->setTag(4);
        speeds->addChild(quadSpeed);

        CCARRAY_FOREACH(speeds->getChildren(), child) {
            if(static_cast<int>(p0->m_startSpeed) != child->getTag()) static_cast<CCMenuItemSpriteExtra*>(child)->setColor({125, 125, 125});
        }
        
        speeds->updateLayout();
        layer->addChild(speeds);

        return true;
    }

    void setProperty(CCObject* sender) {
        auto state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
        switch(sender->getTag()) {
            case 100:
                settings->m_reverseGameplay = state;
                break;
            case 101:
                settings->m_rotateGameplay = state;
                break;
            case 102:
                settings->m_mirrorMode = state;
                break;
            case 103:
                settings->m_startDual = state;
                break;
            case 104:
                settings->m_startMini = state;
                break;
            case 105:
                settings->m_isFlipped = state;
                break;
        }
    }

    void setMode(CCObject* sender) {
        settings->m_startMode = sender->getTag();
        CCObject* child;
        CCARRAY_FOREACH(static_cast<CCMenuItemSpriteExtra*>(sender)->getParent()->getChildren(), child) {
            static_cast<CCMenuItemSpriteExtra*>(child)->setColor({125, 125, 125});
        }
        static_cast<CCMenuItemSpriteExtra*>(sender)->setColor({255, 255, 255});
    }

    void setSpeed(CCObject* sender) {
		settings->m_startSpeed = static_cast<Speed>(sender->getTag());
        CCObject* child;
        CCARRAY_FOREACH(static_cast<CCMenuItemSpriteExtra*>(sender)->getParent()->getChildren(), child) {
            static_cast<CCMenuItemSpriteExtra*>(child)->setColor({125, 125, 125});
        }
        static_cast<CCMenuItemSpriteExtra*>(sender)->setColor({255, 255, 255});
    }
};