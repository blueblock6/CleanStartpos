#include <Geode/modify/LevelSettingsLayer.hpp>

using namespace geode::prelude;

class $modify(CleanStartpos, LevelSettingsLayer) {

    static void onModify(auto& self) {
        self.setHookPriority("CleanStartpos::init", 999);
    }

    void setProperty(CCObject* sender) {
        auto settings = as<LevelSettingsObject*>(as<CCMenu*>(as<CCMenuItemSpriteExtra*>(sender)->getParent())->getUserObject());
        auto state = !as<CCMenuItemToggler*>(sender)->isToggled();
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
        as<LevelSettingsObject*>(as<CCMenu*>(as<CCMenuItemSpriteExtra*>(sender)->getParent())->getUserObject())->m_startMode = sender->getTag() - 200;
        for(int i = 200; i < 208; i++) {
            as<CCMenuItemSpriteExtra*>(as<CCMenuItemSpriteExtra*>(sender)->getParent()->getChildByTag(i))->setColor({125, 125, 125});
        }
        as<CCMenuItemSpriteExtra*>(sender)->setColor({255, 255, 255});
    }

    void setSpeed(CCObject* sender) {
		as<LevelSettingsObject*>(as<CCMenu*>(as<CCMenuItemSpriteExtra*>(sender)->getParent())->getUserObject())->m_startSpeed = as<Speed>(sender->getTag() - 300);
        for(int i = 300; i < 305; i++) {
            as<CCMenuItemSpriteExtra*>(as<CCMenuItemSpriteExtra*>(sender)->getParent()->getChildByTag(i))->setColor({125, 125, 125});
        }
        as<CCMenuItemSpriteExtra*>(sender)->setColor({255, 255, 255});
    }
	
    $override
    bool init(LevelSettingsObject* p0, LevelEditorLayer* p1) {
        if(!LevelSettingsLayer::init(p0, p1)) return false;

        auto layer = as<CCLayer*>(this->getChildren()->objectAtIndex(0));

        if(layer->getChildrenCount() > 13) return true;

        auto menu = as<CCMenu*>(layer->getChildren()->objectAtIndex(1));
        menu->setUserObject(p0);

        layer->setPosition(CCDirector::sharedDirector()->getWinSize() / 2);
        menu->setPosition(ccp(0, 0));

        // Menu bg
        as<CCNode*>(layer->getChildren()->objectAtIndex(0))->setContentSize(ccp(500, 190));
        as<CCNode*>(layer->getChildren()->objectAtIndex(0))->setPosition(ccp(0, 0));
        // OK
        as<CCNode*>(menu->getChildren()->objectAtIndex(0))->setPositionY(-60);
        // Target Order
        as<CCNode*>(layer->getChildren()->objectAtIndex(4))->setPosition(ccp(-100, -30));
        as<CCNode*>(layer->getChildren()->objectAtIndex(5))->setPosition(ccp(-100, -60));
        as<CCNode*>(layer->getChildren()->objectAtIndex(5))->setContentSize(ccp(80, 30));
        as<CCNode*>(layer->getChildren()->objectAtIndex(6))->setPosition(ccp(-100, -60));
        // Target Channel
        as<CCNode*>(layer->getChildren()->objectAtIndex(7))->setPosition(ccp(100, -30));
        as<CCNode*>(layer->getChildren()->objectAtIndex(8))->setPosition(ccp(100, -60));
        as<CCNode*>(layer->getChildren()->objectAtIndex(8))->setContentSize(ccp(80, 30));
        as<CCNode*>(layer->getChildren()->objectAtIndex(9))->setPosition(ccp(100, -60));
        // Disable
        as<CCNode*>(menu->getChildren()->objectAtIndex(1))->setPosition(ccp(210, -60));
        as<CCNode*>(layer->getChildren()->objectAtIndex(2))->setPosition(ccp(210, -37.5));
        // Reset Camera
        as<CCNode*>(menu->getChildren()->objectAtIndex(2))->setPosition(ccp(210, -20));
        as<CCNode*>(menu->getChildren()->objectAtIndex(2))->setScale(1.143);
		auto resetText = CCLabelBMFont::create("Rst Cam", "goldFont.fnt");
        resetText->setPosition(ccp(210, 2.5));
        resetText->setScale(0.6);
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
        reverse->setScale(0.8);
        reverse->setTag(100);
        reverse->toggle(p0->m_reverseGameplay);
        menu->addChild(reverse);
        
        auto reverseText = CCLabelBMFont::create("Reverse", "goldFont.fnt");
        reverseText->setPosition(ccp(210, 42.5));
        reverseText->setScale(0.6);
        layer->addChild(reverseText);
        
        auto rotate = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        rotate->setPosition(ccp(210, 60));
        rotate->setScale(0.8);
        rotate->setTag(101);
        rotate->toggle(p0->m_rotateGameplay);
        menu->addChild(rotate);
        
        auto rotateText = CCLabelBMFont::create("Rotate", "goldFont.fnt");
        rotateText->setPosition(ccp(210, 82.5));
        rotateText->setScale(0.6);
        layer->addChild(rotateText);
        
        auto mirror = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        mirror->setPosition(ccp(-210, -60));
        mirror->setScale(0.8);
        mirror->setTag(102);
        mirror->toggle(p0->m_mirrorMode);
        menu->addChild(mirror);
        
        auto mirrorText = CCLabelBMFont::create("Mirror", "goldFont.fnt");
        mirrorText->setPosition(ccp(-210, -37.5));
        mirrorText->setScale(0.6);
        layer->addChild(mirrorText);
        
        auto dual = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        dual->setPosition(ccp(-210, -20));
        dual->setScale(0.8);
        dual->setTag(103);
        dual->toggle(p0->m_startDual);
        menu->addChild(dual);
        
        auto dualText = CCLabelBMFont::create("Dual", "goldFont.fnt");
        dualText->setPosition(ccp(-210, 2.5));
        dualText->setScale(0.6);
        layer->addChild(dualText);
        
        auto mini = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        mini->setPosition(ccp(-210, 20));
        mini->setScale(0.8);
        mini->setTag(104);
        mini->toggle(p0->m_startMini);
        menu->addChild(mini);
        
        auto miniText = CCLabelBMFont::create("Mini", "goldFont.fnt");
        miniText->setPosition(ccp(-210, 42.5));
        miniText->setScale(0.6);
        layer->addChild(miniText);
        
        auto flip = CCMenuItemToggler::create(checkOn, checkOff, menu, menu_selector(CleanStartpos::setProperty));
        flip->setPosition(ccp(-210, 60));
        flip->setScale(0.8);
        flip->setTag(105);
        flip->toggle(p0->m_isFlipped);
        menu->addChild(flip);
        
        auto flipText = CCLabelBMFont::create("Flip", "goldFont.fnt");
        flipText->setPosition(ccp(-210, 82.5));
        flipText->setScale(0.6);
        layer->addChild(flipText);
        
        // Gamemodes
        auto icon = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_iconBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        icon->setPosition(ccp(-140, 70));
        if(p0->m_startMode != 0) icon->setColor({125, 125, 125});
        icon->setTag(200);
        menu->addChild(icon);
        
        auto ship = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_shipBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        ship->setPosition(ccp(-100, 70));
        if(p0->m_startMode != 1) ship->setColor({125, 125, 125});
        ship->setTag(201);
        menu->addChild(ship);
        
        auto ball = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_ballBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        ball->setPosition(ccp(-60, 70));
        if(p0->m_startMode != 2) ball->setColor({125, 125, 125});
        ball->setTag(202);
        menu->addChild(ball);
        
        auto ufo = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_birdBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        ufo->setPosition(ccp(-20, 70));
        if(p0->m_startMode != 3) ufo->setColor({125, 125, 125});
        ufo->setTag(203);
        menu->addChild(ufo);
        
        auto wave = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_dartBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        wave->setPosition(ccp(20, 70));
        if(p0->m_startMode != 4) wave->setColor({125, 125, 125});
        wave->setTag(204);
        menu->addChild(wave);
        
        auto robot = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_robotBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        robot->setPosition(ccp(60, 70));
        if(p0->m_startMode != 5) robot->setColor({125, 125, 125});
        robot->setTag(205);
        menu->addChild(robot);
        
        auto spider = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_spiderBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        spider->setPosition(ccp(100, 70));
        if(p0->m_startMode != 6) spider->setColor({125, 125, 125});
        spider->setTag(206);
        menu->addChild(spider);
        
        auto swing = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("gj_swingBtn_off_001.png"), this, menu_selector(CleanStartpos::setMode));
        swing->setPosition(ccp(140, 70));
        if(p0->m_startMode != 7) swing->setColor({125, 125, 125});
        swing->setTag(207);
        menu->addChild(swing);
        
        // Speeds
        auto slowSpeed = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("boost_01_001.png"), this, menu_selector(CleanStartpos::setSpeed));
        slowSpeed->setPosition(ccp(-122.5, 10));
        if(p0->m_startSpeed != Speed::Slow) slowSpeed->setColor({125, 125, 125});
        slowSpeed->setTag(301);
        menu->addChild(slowSpeed);
        
        auto normalSpeed = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("boost_02_001.png"), this, menu_selector(CleanStartpos::setSpeed));
        normalSpeed->setPosition(ccp(-81.625, 10));
        if(p0->m_startSpeed != Speed::Normal) normalSpeed->setColor({125, 125, 125});
        normalSpeed->setTag(300);
        menu->addChild(normalSpeed);
        
        auto doubleSpeed = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("boost_03_001.png"), this, menu_selector(CleanStartpos::setSpeed));
        doubleSpeed->setPosition(ccp(-33, 10));
        if(p0->m_startSpeed != Speed::Fast) doubleSpeed->setColor({125, 125, 125});
        doubleSpeed->setTag(302);
        menu->addChild(doubleSpeed);
        
        auto tripleSpeed = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("boost_04_001.png"), this, menu_selector(CleanStartpos::setSpeed));
        tripleSpeed->setPosition(ccp(31.625, 10));
        if(p0->m_startSpeed != Speed::Faster) tripleSpeed->setColor({125, 125, 125});
        tripleSpeed->setTag(303);
        menu->addChild(tripleSpeed);
        
        auto quadSpeed = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("boost_05_001.png"), this, menu_selector(CleanStartpos::setSpeed));
        quadSpeed->setPosition(ccp(105.5, 10));
        if(p0->m_startSpeed != Speed::Fastest) quadSpeed->setColor({125, 125, 125});
        quadSpeed->setTag(304);
        menu->addChild(quadSpeed);

        return true;
    }
};