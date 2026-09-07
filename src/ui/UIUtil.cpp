#include "UIUtil.hpp"
#include "Links.hpp"
#include "StartPosUI.hpp"

using namespace geode::prelude;

inline void createModeButton(const char* sprite, IconType mode, LevelSettingsObjectExt* settings, CCMenu* menu) {
    auto btn = CCMenuItemExt::createSpriteExtraWithFrameName(
        sprite,
        1.f,
        [settings, menu](CCMenuItemSpriteExtra* sender) {
            auto prev = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByTag(settings->m_startMode));
            prev->setColor({127, 127, 127});
            sender->setColor({255, 255, 255});
            settings->m_startMode = sender->getTag();
        }
    );

    int tag = static_cast<int>(mode);
    btn->setTag(tag);
    if(tag != settings->m_startMode) {
        btn->setColor({127, 127, 127});
    }

    menu->addChild(btn);
}

cocos2d::CCMenu* createModeMenu(LevelSettingsObjectExt* settings) {
    auto menu = CCMenu::create();
    menu->setContentSize({300.f, 50.f});
    createModeButton("gj_iconBtn_off_001.png", IconType::Cube, settings, menu);
    createModeButton("gj_shipBtn_off_001.png", IconType::Ship, settings, menu);
    createModeButton("gj_ballBtn_off_001.png", IconType::Ball, settings, menu);
    createModeButton("gj_birdBtn_off_001.png", IconType::Ufo, settings, menu);
    createModeButton("gj_dartBtn_off_001.png", IconType::Wave, settings, menu);
    createModeButton("gj_robotBtn_off_001.png", IconType::Robot, settings, menu);
    createModeButton("gj_spiderBtn_off_001.png", IconType::Spider, settings, menu);
    createModeButton("gj_swingBtn_off_001.png", IconType::Swing, settings, menu);
    menu->setLayout(RowLayout::create());
    return menu;
}

inline void createSpeedButton(const char* sprite, Speed speed, AdvancedStartPos* startPos, CCMenu* menu) {
    bool isP2 = startPos->isP2();
    CCSprite* spr;
    if(isP2) {
        spr = CCSpriteGrayscale::createWithSpriteFrameName(sprite);
    } else {
        spr = CCSprite::createWithSpriteFrameName(sprite);
    }

    LevelSettingsObjectExt* settings = startPos->getSettingsObject();

    auto btn = CCMenuItemExt::createSpriteExtra(
        spr,
        [settings, menu](CCMenuItemSpriteExtra* sender) {
            int currentSpeed = static_cast<int>(settings->m_startSpeed);
            auto prev = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByTag(currentSpeed));
            prev->setColor({127, 127, 127});
            sender->setColor({255, 255, 255});
            settings->m_startSpeed = static_cast<Speed>(sender->getTag());
        }
    );

    btn->setEnabled(!isP2);
    btn->setTag(static_cast<int>(speed));
    if(speed != settings->m_startSpeed) {
        if(isP2) {
            btn->setColor({63, 63, 63});
        } else {
            btn->setColor({127, 127, 127});
        }
    }

    menu->addChild(btn);
}

cocos2d::CCMenu* createSpeedMenu(AdvancedStartPos* startPos) {
    auto menu = CCMenu::create();
    menu->setContentSize({300.f, 50.f});
    createSpeedButton("boost_01_001.png", Speed::Slow, startPos, menu);
    createSpeedButton("boost_02_001.png", Speed::Normal, startPos, menu);
    createSpeedButton("boost_03_001.png", Speed::Fast, startPos, menu);
    createSpeedButton("boost_04_001.png", Speed::Faster, startPos, menu);
    createSpeedButton("boost_05_001.png", Speed::Fastest, startPos, menu);
    menu->setLayout(RowLayout::create());
    return menu;
}

inline std::pair<CCSprite*, CCSprite*> createToggleSprites(bool enabled) {
    CCSprite* checkOn;
    CCSprite* checkOff;
    if(enabled) {
        checkOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        checkOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    } else {
        checkOn = CCSpriteGrayscale::createWithSpriteFrameName("GJ_checkOn_001.png");
        checkOff = CCSpriteGrayscale::createWithSpriteFrameName("GJ_checkOff_001.png");
        checkOn->setColor({127, 127, 127});
        checkOff->setColor({127, 127, 127});
    }
    checkOn->setScale(.8f);
    checkOff->setScale(.8f);

    return {checkOn, checkOff};
}

CCMenu* createToggle(const char* name, ToggleType type, bool toggled, LevelSettingsObjectExt* settings, bool enabled) {
    auto menu = CCMenu::create();
    menu->setContentSize({80.f, 40.f});

    auto label = CCLabelBMFont::create(name, "goldFont.fnt");
    label->limitLabelWidth(80.f, .7f, .1f);
    menu->addChildAtPosition(label, Anchor::Center, {0.f, 15.f});

    auto [checkOn, checkOff] = createToggleSprites(enabled);

    auto btn = CCMenuItemExt::createToggler(checkOn, checkOff, [settings](CCMenuItemToggler* toggler) {
        bool state = !toggler->isToggled();
        switch(toggler->getTag()) {
        case ToggleType::FLIP: settings->m_isFlipped = state; break;
        case ToggleType::MINI: settings->m_startMini = state; break;
        case ToggleType::DUAL: settings->m_startDual = state; break;
        case ToggleType::MIRROR: settings->m_mirrorMode = state; break;
        case ToggleType::FREE: settings->m_fields->isFreeCam = state; break;
        case ToggleType::ROTATE: settings->m_rotateGameplay = state; break;
        case ToggleType::REVERSE: settings->m_reverseGameplay = state; break;
        case ToggleType::RESET: settings->m_resetCamera = state; break;
        case ToggleType::DISABLE: settings->m_disableStartPos = state; break;
        }
    });
    btn->setEnabled(enabled);

    btn->setTag(type);
    btn->toggle(toggled);
    menu->addChildAtPosition(btn, Anchor::Center, {0.f, -7.f});

    return menu;
}

CCNode* createInput(CCNode* base, TextInput*& input, const char* title, CommonFilter filter, const char* placeholder, const std::string& value, bool enabled) {
    base->setContentSize({120, 50});
    base->setAnchorPoint({.5f, .5f});

    auto label = CCLabelBMFont::create(title, "goldFont.fnt");
    label->setScale(.6f);
    base->addChildAtPosition(label, Anchor::Center, {0, 17});

    input = TextInput::create(80, placeholder);
    input->setEnabled(enabled);
    input->getInputNode()->setVisible(enabled);
    input->setCommonFilter(filter);
    if(value != "0" && value != "0.000") {
        input->setString(value);
    }
    base->addChildAtPosition(input, Anchor::Center, {0, -11});

    return base;
}

inline void onLink(AdvancedStartPos* startPos, StartPosUI* ui) {
    bool isP2 = startPos->isP2();
    int id = startPos->m_linkId;

    if(isP2) {
        auto primary = Links::getPrimary(id);
        if(!primary) return;
        ui->onClose(nullptr);
        auto eu = EditorUI::get();
        eu->selectObject(primary, true);
        eu->updateButtons();
        eu->editObject(nullptr);
        return;
    }

    if(auto secondary = Links::getSecondary(id)) {
        ui->onClose(nullptr);
        auto eu = EditorUI::get();
        eu->selectObject(secondary, true);
        eu->updateButtons();
        eu->editObject(nullptr);
        return;
    }

    if(id == 0) id = Links::nextId();
    startPos->m_linkId = id;
    // LevelEditorLayer::createObject + m_isNoTouch = true doesn't remove collision until editor is reloaded \_(._.)_/
    auto obj = static_cast<AdvancedStartPos*>(
        LevelEditorLayer::get()->createObjectsFromString(fmt::format("1,34,2,{},3,{},121,1", startPos->m_positionX, startPos->m_positionY - 30).c_str(), true, true)
        ->firstObject()
    );
    obj->m_linkId = id;
    obj->m_isNoTouch = true;
    obj->encodeSettings(startPos->getSettingsObject());
    Links::insertPair(startPos, obj, id);

    ui->onClose(nullptr);
    auto eu = EditorUI::get();
    eu->m_editorLayer->m_undoObjects->addObject(UndoObject::create(obj, UndoCommand::New));
    eu->selectObject(obj, true);
    eu->updateButtons();
}

inline void onUnlink(AdvancedStartPos* startPos, StartPosUI* ui, CCMenuItemSpriteExtra* unlinkBtn) {
    unlinkBtn->setVisible(false);
    int id = startPos->m_linkId;
    bool isP2 = startPos->isP2();
    auto other = Links::getSecondary(id);
    Links::destroy(id);
    auto lel = LevelEditorLayer::get();
    if(isP2) {
        ui->onClose(nullptr);
        lel->removeObject(startPos, true);
        lel->m_editorUI->deselectAll();
    } else {
        if(other) lel->removeObject(other, true);
    }
}


std::pair<cocos2d::CCMenu*, cocos2d::CCMenu*> createDualToggleAndMenu(AdvancedStartPos* startPos, StartPosUI* ui) {
    auto settings = startPos->getSettingsObject();
    bool isP2 = startPos->isP2();


    auto linkMenu = CCMenu::create();
    linkMenu->setContentSize({80.f, 30.f});

    auto linkBtn = CCMenuItemExt::createSpriteExtraWithFrameName("gj_linkBtn_001.png", .8f, [startPos, ui](auto){
        onLink(startPos, ui);
    });
    linkBtn->setID("link-button");
    linkBtn->setVisible(
        settings->m_startDual ||
        isP2 ||
        startPos->m_linkId != 0
    );
    linkMenu->addChildAtPosition(linkBtn, Anchor::Left, {15.f, 0.f}, false);

    auto unlinkBtn = CCMenuItemExt::createSpriteExtraWithFrameName("gj_linkBtnOff_001.png", .8f, [startPos, ui](CCMenuItemSpriteExtra* unlinkBtn){
        onUnlink(startPos, ui, unlinkBtn);
    });
    unlinkBtn->setID("unlink-button");
    unlinkBtn->setVisible(
        isP2 ||
        (startPos->m_linkId != 0 && Links::getSecondary(startPos->m_linkId))
    );
    linkMenu->addChildAtPosition(unlinkBtn, Anchor::Right, {-15.f, 0.f}, false);


    auto toggleMenu = CCMenu::create();
    toggleMenu->setContentSize({80.f, 40.f});

    auto label = CCLabelBMFont::create("Dual", "goldFont.fnt");
    label->limitLabelWidth(80.f, .7f, .1f);
    toggleMenu->addChildAtPosition(label, Anchor::Center, {0.f, 15.f});

    auto [checkOn, checkOff] = createToggleSprites(!isP2);

    auto btn = CCMenuItemExt::createToggler(checkOn, checkOff, [startPos, linkBtn, isP2](CCMenuItemToggler* toggle){
        bool state = !toggle->isToggled();
        startPos->getSettingsObject()->m_startDual = state;
        linkBtn->setVisible(
            state ||
            isP2 ||
            startPos->m_linkId != 0
        );
    });
    btn->setEnabled(!isP2);

    btn->toggle(settings->m_startDual);
    toggleMenu->addChildAtPosition(btn, Anchor::Center, {0.f, -7.f});

    return {toggleMenu, linkMenu};
}
