#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSettingsLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/DrawGridLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GameObject.hpp>

using namespace geode::prelude;

#define ADD_BUTTON(sprite, tag, callback, condition, disable)                                                           \
    btn = CCMenuItemSpriteExtra::create(                                                                                \
        disable ? CCSpriteGrayscale::createWithSpriteFrameName(sprite) : CCSprite::createWithSpriteFrameName(sprite),   \
        this, menu_selector(callback));                                                                                 \
    btn->setTag(tag);                                                                                                   \
    btn->setEnabled(!disable);                                                                                          \
    if(condition != tag) btn->setColor({127, 127, 127});                                                                \
    menu->addChild(btn);

#define ADD_TOGGLE(name, tag, condition, disable)                                                                           \
    label = CCLabelBMFont::create(name, "goldFont.fnt");                                                                    \
    label->limitLabelWidth(75, 0.7f, 0.1f);                                                                                 \
    label->setPositionY(start);                                                                                             \
    menu->addChild(label);                                                                                                  \
    if(disable) toggle = CCMenuItemToggler::create(check0, check1, this, nullptr);                                          \
    else toggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(CleanStartpos::toggleProperty), 0.8f);   \
    toggle->setTag(tag);                                                                                                    \
    toggle->toggle(condition);                                                                                              \
    toggle->setEnabled(!disable);                                                                                           \
    toggle->setPositionY(start - 22);                                                                                       \
    menu->addChild(toggle);                                                                                                 \
    start -= 45;

std::unordered_map<short, std::pair<StartPosObject*, GameObject*>> links;
short maxLink;
StartPosObject* selectedStartPos;

void encodeFakeStartpos(GameObject* obj, LevelSettingsObject* settings) {
    obj->m_isNonStickX = (settings->m_startMode & 0b100) >> 2;
    obj->m_isNonStickY = (settings->m_startMode & 0b010) >> 1;
    obj->m_isDontBoostY = (settings->m_startMode & 0b001);
    obj->m_isExtraSticky = settings->m_isFlipped;
    obj->m_isScaleStick = settings->m_startMini;
    obj->m_isDontBoostX = settings->m_reverseGameplay;
    obj->m_hasExtendedCollision = settings->m_disableStartPos;
    if(auto ref = links[obj->m_objectMaterial].first) ref->m_startSettings->m_propertykA44 = settings->m_spawnGroup;
}

LevelSettingsObject* loadFakeStartpos(GameObject* obj, StartPosObject* reference) {
    auto settings = new LevelSettingsObject(*reference->m_startSettings);
    settings->m_startMode = (obj->m_isNonStickX << 2) | (obj->m_isNonStickY << 1) | obj->m_isDontBoostY;
    settings->m_isFlipped = obj->m_isExtraSticky;
    settings->m_startMini = obj->m_isScaleStick;
    settings->m_reverseGameplay = obj->m_isDontBoostX;
    settings->m_disableStartPos = obj->m_hasExtendedCollision;
    settings->m_spawnGroup = reference->m_startSettings->m_propertykA44;
    return settings;
}

class $modify(CleanStartpos, LevelSettingsLayer) {
    struct Fields {
        bool isP2 = false;
        CCMenuItemSpriteExtra* linkBtn = nullptr;
        TextInput* velocityInput = nullptr;
        TextInput* offsetInput = nullptr;
    };

    $override
    bool init(LevelSettingsObject* p0, LevelEditorLayer* p1) {
        if(!LevelSettingsLayer::init(p0, p1)) return false;

        if(p1) return true;

        m_fields->isP2 = m_settingsObject->getUserObject() != nullptr;
        auto center = CCDirector::sharedDirector()->getWinSize() / 2;

        // Move Stuff
        m_mainLayer->getChildByType(0)->setContentSize({455, 250});         // BG
        m_buttonMenu->getChildByType(0)->setPositionY(-100);                // OK
        m_mainLayer->getChildByType(4)->setPositionX(center.width - 80);    // Target Order Label
        m_mainLayer->getChildByType(4)->setPositionY(center.height - 70);   // Target Order Label
        m_mainLayer->getChildByType(5)->setPositionX(center.width - 80);    // Target Order BG
        m_mainLayer->getChildByType(5)->setPositionY(center.height - 100);  // Target Order BG
        m_mainLayer->getChildByType(5)->setContentWidth(80);                // Target Order BG
        m_mainLayer->getChildByType(7)->setPositionX(center.width + 80);    // Target Channel Label
        m_mainLayer->getChildByType(7)->setPositionY(center.height - 70);   // Target Channel Label
        m_mainLayer->getChildByType(8)->setPositionX(center.width + 80);    // Target Channel BG
        m_mainLayer->getChildByType(8)->setPositionY(center.height - 100);  // Target Channel BG
        m_mainLayer->getChildByType(8)->setContentWidth(80);                // Target Channel BG
        if(m_fields->isP2) {
            m_mainLayer->getChildByType(6)->setVisible(false);              // Target Order Input
            m_mainLayer->getChildByType(9)->setVisible(false);              // Target Channel Input
        } else {
            m_mainLayer->getChildByType(6)->setPositionX(center.width - 80);    // Target Order Input
            m_mainLayer->getChildByType(6)->setPositionY(center.height - 100);  // Target Order Input
            m_mainLayer->getChildByType(9)->setPositionX(center.width + 80);    // Target Channel Input
            m_mainLayer->getChildByType(9)->setPositionY(center.height - 100);  // Target Channel Input
        }

        // Hide Stuff
        m_mainLayer->getChildByType(2)->setVisible(false);  // Disable Label
        m_buttonMenu->getChildByType(1)->setVisible(false); // Disable Toggle
        m_mainLayer->getChildByType(3)->setVisible(false);  // Reset Camera Label
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
        ADD_BUTTON("gj_iconBtn_off_001.png", 0, CleanStartpos::onCMode, p0->m_startMode, false);
        ADD_BUTTON("gj_shipBtn_off_001.png", 1, CleanStartpos::onCMode, p0->m_startMode, false);
        ADD_BUTTON("gj_ballBtn_off_001.png", 2, CleanStartpos::onCMode, p0->m_startMode, false);
        ADD_BUTTON("gj_birdBtn_off_001.png", 3, CleanStartpos::onCMode, p0->m_startMode, false);
        ADD_BUTTON("gj_dartBtn_off_001.png", 4, CleanStartpos::onCMode, p0->m_startMode, false);
        ADD_BUTTON("gj_robotBtn_off_001.png", 5, CleanStartpos::onCMode, p0->m_startMode, false);
        ADD_BUTTON("gj_spiderBtn_off_001.png", 6, CleanStartpos::onCMode, p0->m_startMode, false);
        ADD_BUTTON("gj_swingBtn_off_001.png", 7, CleanStartpos::onCMode, p0->m_startMode, false);
        menu->updateLayout(false);
        m_mainLayer->addChildAtPosition(menu, Anchor::Center, {0, 95}, false);
        
        menu = CCMenu::create();
        menu->setID("select-speed-menu"_spr);
        menu->setContentWidth(300.f);
        menu->setLayout(RowLayout::create());
        ADD_BUTTON("boost_01_001.png", 1, CleanStartpos::onCSpeed, static_cast<int>(p0->m_startSpeed), m_fields->isP2);
        ADD_BUTTON("boost_02_001.png", 0, CleanStartpos::onCSpeed, static_cast<int>(p0->m_startSpeed), m_fields->isP2);
        ADD_BUTTON("boost_03_001.png", 2, CleanStartpos::onCSpeed, static_cast<int>(p0->m_startSpeed), m_fields->isP2);
        ADD_BUTTON("boost_04_001.png", 3, CleanStartpos::onCSpeed, static_cast<int>(p0->m_startSpeed), m_fields->isP2);
        ADD_BUTTON("boost_05_001.png", 4, CleanStartpos::onCSpeed, static_cast<int>(p0->m_startSpeed), m_fields->isP2);
        menu->updateLayout(false);
        m_mainLayer->addChildAtPosition(menu, Anchor::Center, {0, 40}, false);

        CCLabelBMFont* label;
        auto check0 = CCSpriteGrayscale::createWithSpriteFrameName("GJ_checkOff_001.png");
        check0->setScale(0.8f);
        check0->setColor({127, 127, 127});
        auto check1 = CCSpriteGrayscale::createWithSpriteFrameName("GJ_checkOn_001.png");
        check1->setScale(0.8f);
        check1->setColor({127, 127, 127});
        CCMenuItemToggler* toggle;

        menu = CCMenu::create();
        menu->setID("left-properties-menu"_spr);
        float start = 100;
        ADD_TOGGLE("Flip", 100, p0->m_isFlipped, false);
        ADD_TOGGLE("Mini", 101, p0->m_startMini, false);
        ADD_TOGGLE("Dual", 102, p0->m_startDual, m_fields->isP2);
        ADD_TOGGLE("Mirror", 103, p0->m_mirrorMode, m_fields->isP2);
        bool freeMode = false;
        if(m_fields->isP2) {
            auto obj = static_cast<GameObject*>(p0->getUserObject());
            if(links[obj->m_objectMaterial].first) freeMode = links[obj->m_objectMaterial].first->m_isIceBlock;
        } else freeMode = selectedStartPos->m_isIceBlock;
        ADD_TOGGLE("Free Mode", 108, freeMode, m_fields->isP2);
        m_mainLayer->addChildAtPosition(menu, Anchor::Center, {-180, 10}, false);

        menu = CCMenu::create();
        menu->setID("right-properties-menu"_spr);
        start = 100;
        ADD_TOGGLE("Rotate", 104, p0->m_rotateGameplay, m_fields->isP2);
        ADD_TOGGLE("Reverse", 105, p0->m_reverseGameplay, false);
        ADD_TOGGLE("Reset Camera", 106, p0->m_resetCamera, m_fields->isP2);
        ADD_TOGGLE("Disable", 107, p0->m_disableStartPos, false);
        m_mainLayer->addChildAtPosition(menu, Anchor::Center, {180, 10}, false);

        m_fields->linkBtn = CCMenuItemExt::createSpriteExtraWithFrameName("gj_linkBtn_001.png", .8f, [this](auto sender){
            if(m_fields->isP2) {
                auto obj = static_cast<GameObject*>(m_settingsObject->getUserObject());
                if(!links[obj->m_objectMaterial].first) return;
                this->onClose(this);
                EditorUI::get()->selectObject(links[obj->m_objectMaterial].first, true);
                EditorUI::get()->updateButtons();
                EditorUI::get()->editObject(nullptr);
            } else if(links.contains(selectedStartPos->m_objectMaterial) && links[selectedStartPos->m_objectMaterial].second) {
                this->onClose(this);
                EditorUI::get()->selectObject(links[selectedStartPos->m_objectMaterial].second, true);
                EditorUI::get()->updateButtons();
                EditorUI::get()->editObject(nullptr);
            } else {
                int id = selectedStartPos->m_objectMaterial == 0 ? ++maxLink : selectedStartPos->m_objectMaterial;
                selectedStartPos->m_objectMaterial = id;
                // LevelEditorLayer::createObject and m_isNoTouch = true doesn't remove collision until editor is reloaded \_(._.)_/
                auto objs = LevelEditorLayer::get()->createObjectsFromString(
                    fmt::format("1,34,2,{},3,{},121,1", selectedStartPos->m_positionX, selectedStartPos->m_positionY - 30).c_str(),
                    false, false);
                auto obj = static_cast<GameObject*>(objs->firstObject());
                obj->m_objectMaterial = id;
                obj->m_isNoTouch = true;
                encodeFakeStartpos(obj, m_settingsObject);
                links[id] = {selectedStartPos, obj};
                this->onClose(this);
                EditorUI::get()->selectObject(obj, true);
                EditorUI::get()->updateButtons();
            }
        });
        m_fields->linkBtn->setVisible(m_fields->isP2 || p0->m_startDual || selectedStartPos->m_objectMaterial != 0);
        m_fields->linkBtn->setID("dual-link-btn"_spr);
        m_fields->linkBtn->setPosition({-152.f, -1.f});
        m_buttonMenu->addChild(m_fields->linkBtn);

        auto unlinkBtn = CCMenuItemExt::createSpriteExtraWithFrameName("gj_linkBtnOff_001.png", .8f, [this, p0](auto sender) {
            if(m_fields->isP2) {
                int id = static_cast<GameObject*>(m_settingsObject->getUserObject())->m_objectMaterial;
                links[id].first->m_objectMaterial = 0;
                links.erase(id);
                this->onClose(nullptr);
                LevelEditorLayer::get()->removeObject(static_cast<GameObject*>(m_settingsObject->getUserObject()), true);
            } else {
                LevelEditorLayer::get()->removeObject(links[selectedStartPos->m_objectMaterial].second, true);
                links.erase(selectedStartPos->m_objectMaterial);
                selectedStartPos->m_objectMaterial = 0;
                sender->setVisible(false);
                m_fields->linkBtn->setVisible(p0->m_startDual);
            }
        });
        unlinkBtn->setVisible(m_fields->isP2 || links.contains(selectedStartPos->m_objectMaterial));
        unlinkBtn->setID("dual-unlink-btn"_spr);
        unlinkBtn->setPosition({-208.f, -1.f});
        m_buttonMenu->addChild(unlinkBtn);

        auto velocityLabel = CCLabelBMFont::create("Velocity", "goldFont.fnt");
        velocityLabel->setID("velocity-label"_spr);
        velocityLabel->setScale(.6f);
        m_mainLayer->addChildAtPosition(velocityLabel, Anchor::Center, {-80.f, -12.f}, false);

        m_fields->velocityInput = TextInput::create(80, "Velocity");
        m_fields->velocityInput->setID("velocity-input"_spr);
        m_fields->velocityInput->setCommonFilter(CommonFilter::Float);
        m_mainLayer->addChildAtPosition(m_fields->velocityInput, Anchor::Center, {-80.f, -40.f}, false);

        float velocityValue = *reinterpret_cast<float*>(&m_settingsObject->m_spawnGroup);
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(3) << velocityValue;
        if(velocityValue != 0) m_fields->velocityInput->setString(stream.str());

        auto offsetLabel = CCLabelBMFont::create("Border Offset", "goldFont.fnt");
        offsetLabel->setID("offset-label"_spr);
        offsetLabel->setScale(.6f);
        m_mainLayer->addChildAtPosition(offsetLabel, Anchor::Center, {80.f, -12.f}, false);

        m_fields->offsetInput = TextInput::create(30, "0");
        m_fields->offsetInput->setID("offset-input"_spr);
        m_fields->offsetInput->setCommonFilter(CommonFilter::Int);
        if(m_fields->isP2) m_fields->offsetInput->getInputNode()->setVisible(false);
        else if(selectedStartPos->m_controlID != 0) m_fields->offsetInput->setString(std::to_string(selectedStartPos->m_controlID));
        m_mainLayer->addChildAtPosition(m_fields->offsetInput, Anchor::Center, {80.f, -40.f}, false);

        auto offsetLeft = CCMenuItemExt::createSpriteExtraWithFrameName("edit_leftBtn_001.png", 1.f, [this](auto sender) {
            int value = 0;
            auto str = m_fields->offsetInput->getString();
            std::from_chars(str.data(), str.data() + str.size(), value);
            m_fields->offsetInput->setString(std::to_string(--value));
        });
        offsetLeft->setID("decrease-offset-button"_spr);
        if(m_fields->isP2) offsetLeft->setEnabled(false);
        m_buttonMenu->addChildAtPosition(offsetLeft, Anchor::BottomLeft, {52.5f, -40.f}, false);

        auto offsetRight = CCMenuItemExt::createSpriteExtraWithFrameName("edit_rightBtn_001.png", 1.f, [this](auto sender) {
            int value = 0;
            auto str = m_fields->offsetInput->getString();
            std::from_chars(str.data(), str.data() + str.size(), value);
            m_fields->offsetInput->setString(std::to_string(++value));
        });
        offsetRight->setID("increase-offset-button"_spr);
        if(m_fields->isP2) offsetRight->setEnabled(false);
        m_buttonMenu->addChildAtPosition(offsetRight, Anchor::BottomLeft, {107.5f, -40.f}, false);

        handleTouchPriority(this);

        return true;
    }

    void onCMode(CCObject* sender) {
        static_cast<CCMenuItemSpriteExtra*>(static_cast<CCMenuItemSpriteExtra*>(sender)->getParent()->getChildByTag(m_settingsObject->m_startMode))->setColor({127, 127, 127});
        m_settingsObject->m_startMode = sender->getTag();
        static_cast<CCMenuItemSpriteExtra*>(sender)->setColor({255, 255, 255});
    }

    void onCSpeed(CCObject* sender) {
        static_cast<CCMenuItemSpriteExtra*>(static_cast<CCMenuItemSpriteExtra*>(sender)->getParent()->getChildByTag(static_cast<int>(m_settingsObject->m_startSpeed)))->setColor({127, 127, 127});
		m_settingsObject->m_startSpeed = static_cast<Speed>(sender->getTag());
        static_cast<CCMenuItemSpriteExtra*>(sender)->setColor({255, 255, 255});
    }

    void toggleProperty(CCObject* sender) {
        auto state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
        switch(sender->getTag()) {
            case 100:
                m_settingsObject->m_isFlipped = state;
                break;
            case 101:
                m_settingsObject->m_startMini = state;
                break;
            case 102:
                m_settingsObject->m_startDual = state;
                m_fields->linkBtn->setVisible(state || m_fields->isP2 || selectedStartPos->m_objectMaterial != 0);
                break;
            case 103:
                m_settingsObject->m_mirrorMode = state;
                break;
            case 104:
                m_settingsObject->m_rotateGameplay = state;
                break;
            case 105:
                m_settingsObject->m_reverseGameplay = state;
                break;
            case 106:
                m_settingsObject->m_resetCamera = state;
                break;
            case 107:
                m_settingsObject->m_disableStartPos = state;
                break;
            case 108:
                selectedStartPos->m_isIceBlock = state;
                break;
        }
    }

    $override
    void onClose(CCObject* sender) {
        if(m_fields->linkBtn) {
            auto str = m_fields->velocityInput->getString();
            float velocityValue = 0;
            bool valid;
            if (str.empty()) valid = false;
            else if (std::isdigit(str[0])) valid = true;
            else valid = (str.length() > 1) && (
                    (str[0] == '-' && std::isdigit(str[1])) ||
                    (str[0] == '.' && std::isdigit(str[1])) ||
                    (str.length() > 2 && str[0] == '-' && str[1] == '.' && std::isdigit(str[2])));
            if(valid) velocityValue = std::stof(str);
            m_settingsObject->m_spawnGroup = *reinterpret_cast<int*>(&velocityValue);
            if(!m_fields->isP2) {
                int offsetValue = 0;
                str = m_fields->offsetInput->getString();
                std::from_chars(str.data(), str.data() + str.size(), offsetValue);
                selectedStartPos->m_controlID = offsetValue;
            }
        }
        if(auto obj = typeinfo_cast<GameObject*>(m_settingsObject->getUserObject())) encodeFakeStartpos(obj, m_settingsObject);
        m_settingsObject->setUserObject(nullptr);
        LevelSettingsLayer::onClose(sender);
    }
};

TeleportPortalObject* tpObj = nullptr;
class $modify(GJBaseGameLayer) {
    $override
    void loadStartPosObject() {
        if(m_startPosObject) m_startPosObject->m_startPosition += ccp(0, m_startPosObject->m_controlID * 30);
        GJBaseGameLayer::loadStartPosObject();
        if(!m_startPosObject) return;

        m_startPosObject->m_startPosition -= ccp(0, m_startPosObject->m_controlID * 30);
        if(PlayLayer::get()) toggleFlipped(m_startPosObject->m_startSettings->m_mirrorMode, true);
        if(m_startPosObject->m_isIceBlock) {
            CC_SAFE_RELEASE(tpObj);
            tpObj = TeleportPortalObject::create("edit_eGameRotBtn_001.png", true);
            tpObj->m_cameraIsFreeMode = true;
            tpObj->m_cameraEasingValue = 10;
            tpObj->m_cameraPaddingValue = .5f;
            tpObj->retain();
            this->playerWillSwitchMode(m_player1, tpObj);
        }
        if(m_startPosObject->m_startSettings->m_spawnGroup != 0) m_player1->setYVelocity(*reinterpret_cast<float*>(&m_startPosObject->m_startSettings->m_spawnGroup), 0);

        if(!m_player2 || !links.contains(m_startPosObject->m_objectMaterial)) return;
        auto pair = links[m_startPosObject->m_objectMaterial];
        if(!pair.second) return;
        auto settings = loadFakeStartpos(pair.second, m_startPosObject);
        if(settings->m_disableStartPos) return;

        if(settings->m_spawnGroup != 0) m_player2->setYVelocity(*reinterpret_cast<float*>(&m_startPosObject->m_startSettings->m_propertykA44) * 2, 0);

        m_player2->setPosition(pair.second->getPosition());
        if(settings->m_startMode != m_startPosObject->m_startSettings->m_startMode) switch(settings->m_startMode) {
            case 0:
                m_player2->toggleFlyMode(false, false);
                m_player2->toggleRollMode(false, false);
                m_player2->toggleBirdMode(false, false);
                m_player2->toggleDartMode(false, false);
                m_player2->toggleRobotMode(false, false);
                m_player2->toggleSpiderMode(false, false);
                m_player2->toggleSwingMode(false, false);
                break;
            case 1:
                m_player2->toggleFlyMode(true, true);
                break;
            case 2:
                m_player2->toggleRollMode(true, true);
                break;
            case 3:
                m_player2->toggleBirdMode(true, true);
                break;
            case 4:
                m_player2->toggleDartMode(true, true);
                break;
            case 5:
                m_player2->toggleRobotMode(true, true);
                break;
            case 6:
                m_player2->toggleSpiderMode(true, true);
                break;
            case 7:
                m_player2->toggleSwingMode(true, true);
                break;
        }
        m_player2->flipGravity(settings->m_isFlipped, true);
        m_player2->togglePlayerScale(settings->m_startMini, true);
        m_player2->doReversePlayer(settings->m_reverseGameplay);
    }

    $override
    bool init() {
        maxLink = 0;
        links.clear();
        return GJBaseGameLayer::init();
    }
};

class $modify(CPlayLayer, PlayLayer) {
    struct Fields {
        StartPosObject* previousStartpos = nullptr;
    };

    $override
    void resetLevel() {
        bool newSP = m_fields->previousStartpos != m_startPosObject;
        if(newSP) {
            m_fields->previousStartpos = m_startPosObject;
        } else if(m_startPosObject) m_startPosObject->m_startPosition += ccp(0, m_startPosObject->m_controlID * 30);
        PlayLayer::resetLevel();
        if(m_startPosObject) {
            if(newSP) scheduleOnce(schedule_selector(CPlayLayer::wait), .01f);
            else m_startPosObject->m_startPosition -= ccp(0, m_startPosObject->m_controlID * 30);
        }
    }

    $override
    void startGame() {
        PlayLayer::startGame();
        if(m_startPosObject) scheduleOnce(schedule_selector(CPlayLayer::wait), .01f);
    }

    void wait(float) { // p2 teleports to p1 on first attempt?
        PlayLayer::get()->resetLevel();
    }
};

class $modify(CEditorUI, EditorUI) {
    struct Fields {
        CCMenuItemSpriteExtra* spBtn = nullptr;
        bool paused = true;
        float center = 0;
    };
    
    $override
    bool init(LevelEditorLayer* lel) {
        if(!EditorUI::init(lel)) return false;

        m_fields->spBtn = CCMenuItemExt::createSpriteExtra(CircleButtonSprite::createWithSpriteFrameName("edit_eStartPosBtn_001.png", 1.f, CircleBaseColor::Green, CircleBaseSize::Small), [this, lel](auto sender){
            auto pl = lel->m_player1;
            StartPosObject* sp = static_cast<StartPosObject*>(lel->createObjectsFromString(fmt::format("1,31,2,{},3,{}", pl->getPositionX(), pl->getPositionY() - 90).c_str(), false, false)->firstObject());
            auto settings = sp->m_startSettings;
            settings->m_startsWithStartPos = true;
            settings->m_startMode = pl->isInNormalMode() ? 0 : pl->m_isShip ? 1 : pl->m_isBall ? 2 : pl->m_isBird ? 3 : pl->m_isDart ? 4 : pl->m_isRobot ? 5 : pl->m_isSpider ? 6 : 7;
            float speed = pl->m_playerSpeed;
            settings->m_startSpeed = speed == .7f ? Speed::Slow : speed == .9f ? Speed::Normal : speed == 1.1f ? Speed::Fast : speed == 1.3f ? Speed::Faster : Speed::Fastest;
            settings->m_isFlipped = pl->m_isUpsideDown;
            settings->m_startMini = pl->m_vehicleSize != 1.f;
            settings->m_startDual = lel->m_gameState.m_isDualMode;
            settings->m_rotateGameplay = pl->m_isSideways;
            settings->m_reverseGameplay = pl->m_isGoingLeft;
            float velocity = static_cast<float>(pl->m_yVelocity);
            settings->m_spawnGroup = *reinterpret_cast<int*>(&velocity);
            float offset = ((m_fields->paused ? m_fields->center : (lel->m_groundLayer->getPositionY() + lel->m_groundLayer2->getPositionY()) / 2) - pl->getPositionY() - pl->getParent()->getPositionY()) / 30;
            if(offset > 0) offset += .99f;
            sp->m_controlID = offset;
            if(lel->m_gameState.m_isDualMode) {
                pl = lel->m_player2;
                auto p2 = static_cast<GameObject*>(lel->createObjectsFromString(fmt::format("1,34,2,{},3,{},121,1", pl->getPositionX(), pl->getPositionY() - 90).c_str(), false, false)->firstObject());
                sp->m_objectMaterial = ++maxLink;
                p2->m_objectMaterial = maxLink;
                links[maxLink] = {sp, p2};
                settings = new LevelSettingsObject(*settings);
                settings->m_startMode = pl->isInNormalMode() ? 0 : pl->m_isShip ? 1 : pl->m_isBall ? 2 : pl->m_isBird ? 3 : pl->m_isDart ? 4 : pl->m_isRobot ? 5 : pl->m_isSpider ? 6 : 7;
                settings->m_isFlipped = pl->m_isUpsideDown;
                settings->m_startMini = pl->m_vehicleSize != 1.f;
                settings->m_reverseGameplay = pl->m_isGoingLeft;
                float velocity = static_cast<float>(pl->m_yVelocity);
                settings->m_spawnGroup = *reinterpret_cast<int*>(&velocity);
                encodeFakeStartpos(p2, settings);
            }
        });
        m_fields->spBtn->setID("create-startpos-button"_spr);
        m_fields->spBtn->setVisible(false);

        auto playtestMenu = this->getChildByID("playtest-menu");
        playtestMenu->addChild(m_fields->spBtn);
        playtestMenu->setContentWidth(playtestMenu->getContentWidth() + 50.f);
        playtestMenu->updateLayout();
        playtestMenu->setPositionX(playtestMenu->getPositionX() + 25);

        return true;
    }

    $override
    void onPlaytest(CCObject* sender) {
        m_fields->center = (m_editorLayer->m_groundLayer->getPositionY() + m_editorLayer->m_groundLayer2->getPositionY()) / 2;
        m_fields->paused = true;
        EditorUI::onPlaytest(sender);
    }

    $override
    void selectObject(GameObject* p0, bool p1) {
        if(p0->m_objectID == 31) selectedStartPos = static_cast<StartPosObject*>(p0);
        else if(p0->m_objectID == 34) p0->m_baseColor->m_defaultColorID = 1011; // make edit object button work
        EditorUI::selectObject(p0, p1);
    }

    $override
    void editObject(CCObject* p0) {
        if(m_selectedObject && m_selectedObject->m_objectID == 34 && m_selectedObject->m_objectMaterial != 0) {
            auto ref = links[m_selectedObject->m_objectMaterial].first;
            if(!ref) return;
            auto settings = loadFakeStartpos(m_selectedObject, ref);
            settings->setUserObject(m_selectedObject);
            LevelSettingsLayer::create(settings, nullptr)->show();
        } else EditorUI::editObject(p0);
    }
};

class $modify(DrawGridLayer) {
    $override
    void draw() {
        DrawGridLayer::draw();
        ccDrawColor4B(191, 191, 191, 255);
        for(auto entry : links) {
            if(entry.second.second && !entry.second.second->m_isSelected) entry.second.second->setColor({127, 127, 127});
            if(entry.second.first && entry.second.second) ccDrawLine(entry.second.first->getPosition(), entry.second.second->getPosition());
        }
    }
};

class $modify(LevelEditorLayer) {
    $override
    CCArray* createObjectsFromString(const gd::string& p0, bool p1, bool p2) {
        auto oldLinks = links;
        auto ret = LevelEditorLayer::createObjectsFromString(p0, true, true);
        std::unordered_map<short, short> newIDs;
        std::vector<GameObject*> newSPs;
        std::unordered_map<size_t, StartPosObject*> startPoses;
        auto arr = CCArrayExt<GameObject*>(ret);
        
        for(size_t i = 0; i < arr.size(); i++) {
            if(arr[i]->m_objectID == 31) startPoses[i] = static_cast<StartPosObject*>(arr[i]);
            if(arr[i]->m_objectMaterial != 0 && (arr[i]->m_objectID == 31 || arr[i]->m_objectID == 34)) {
                if(!newIDs.contains(arr[i]->m_objectMaterial)) newIDs[arr[i]->m_objectMaterial] = ++maxLink;
                newSPs.push_back(arr[i]);
            }
        }
        
        links = oldLinks;
        for(auto obj : newSPs) {
            obj->m_objectMaterial = newIDs[obj->m_objectMaterial];
            if(links.contains(obj->m_objectMaterial)) {
                if(obj->m_objectID == 31) links[obj->m_objectMaterial].first = static_cast<StartPosObject*>(obj);
                else links[obj->m_objectMaterial].second = obj;
            } else {
                if(obj->m_objectID == 31) links[obj->m_objectMaterial] = {static_cast<StartPosObject*>(obj), nullptr};
                else links[obj->m_objectMaterial] = {nullptr, obj};
            }
        }

        if(startPoses.empty()) return ret;
        std::stringstream ss(p0);
        std::string token;
        size_t i = 0;
        while(std::getline(ss, token, ';')) {
            if(startPoses.contains(i)) startPoses[i]->loadSettingsFromString(token);
            ++i;
        }

        return ret;
    }

    $override
    void removeSpecial(GameObject* p0) {
        LevelEditorLayer::removeSpecial(p0);
        if(p0->m_objectID == 34 && links.contains(p0->m_objectMaterial)) links[p0->m_objectMaterial].second = nullptr;
        else if(p0 && p0->m_objectID == 31 && links.contains(p0->m_objectMaterial)) {
            links[p0->m_objectMaterial].first = nullptr;
            geode::queueInMainThread([this, p0](){
                auto link = links[p0->m_objectMaterial].second;
                if(!link || m_undoObjects->count() == 0) return;
                auto undo = static_cast<UndoObject*>(m_undoObjects->lastObject());
                if(undo->m_command == UndoCommand::Delete) {
                    auto arr = CCArray::createWithObject(p0);
                    undo = UndoObject::createWithArray(CCArray::createWithObject(p0), UndoCommand::DeleteMulti);
                    m_undoObjects->replaceObjectAtIndex(m_undoObjects->count()-1, undo);
                }
                if(undo->m_command == UndoCommand::DeleteMulti) undo->m_objects->addObject(link);
                else return;
                this->removeObject(link, true);
            });
        }
    }

    $override
    void addSpecial(GameObject* p0) {
        LevelEditorLayer::addSpecial(p0);
        if(p0->m_objectID == 31 && p0->m_objectMaterial != 0) {
            maxLink = std::max(maxLink, p0->m_objectMaterial);
            if(links.contains(p0->m_objectMaterial)) links[p0->m_objectMaterial].first = (StartPosObject*)(p0);
            else links[p0->m_objectMaterial] = {(StartPosObject*)(p0), nullptr};
        } else if(p0->m_objectID == 34 && p0->m_objectMaterial != 0) {
            if(PlayLayer::get()) p0->m_isHide = true;
            maxLink = std::max(maxLink, p0->m_objectMaterial);
            if(links.contains(p0->m_objectMaterial)) links[p0->m_objectMaterial].second = p0;
            else links[p0->m_objectMaterial] = {nullptr, p0};
        }
    }

    $override
    void onPlaytest() {
        LevelEditorLayer::onPlaytest();
        static_cast<CEditorUI*>(m_editorUI)->m_fields->spBtn->setVisible(true);
        static_cast<CEditorUI*>(m_editorUI)->m_fields->paused = false;
    }

    $override
    void onStopPlaytest() {
        static_cast<CEditorUI*>(m_editorUI)->m_fields->spBtn->setVisible(false);
        LevelEditorLayer::onStopPlaytest();
    }
};

class $modify(GameObject) {
    $override
    void customSetup() {
        GameObject::customSetup();
        if(m_objectID == 31 && m_objectMaterial != 0) {
            maxLink = std::max(maxLink, m_objectMaterial);
            if(links.contains(m_objectMaterial)) links[m_objectMaterial].first = (StartPosObject*)(this);
            else links[m_objectMaterial] = {(StartPosObject*)(this), nullptr};
        } else if(m_objectID == 34 && m_objectMaterial != 0) {
            if(PlayLayer::get()) m_isHide = true;
            maxLink = std::max(maxLink, m_objectMaterial);
            if(links.contains(m_objectMaterial)) links[m_objectMaterial].second = this;
            else links[m_objectMaterial] = {nullptr, this};
        }
    }
};
