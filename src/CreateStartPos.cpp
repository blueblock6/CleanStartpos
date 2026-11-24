#include "Hooks.hpp"
#include "Settings.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

HookedStartPosObject* createStartPosShared(PlayerObject* player, bool isP2) {
    auto object = modify_cast<HookedStartPosObject*>(GameObject::createWithKey(isP2 ? 34 : 31));
    object->firstSetup();
    object->customSetup();
    object->setStartPos(player->getPosition());

    if(player->m_isShip) {
        object->m_startSettings->m_startMode = 1;
    } else if(player->m_isBall) {
        object->m_startSettings->m_startMode = 2;
    } else if(player->m_isBird) {
        object->m_startSettings->m_startMode = 3;
    } else if(player->m_isDart) {
        object->m_startSettings->m_startMode = 4;
    } else if(player->m_isRobot) {
        object->m_startSettings->m_startMode = 5;
    } else if(player->m_isSpider) {
        object->m_startSettings->m_startMode = 6;
    } else if(player->m_isSwing) {
        object->m_startSettings->m_startMode = 7;
    } else {
        object->m_startSettings->m_startMode = 0;
    }

    if(player->m_playerSpeed == 0.7f) {
        object->m_startSettings->m_startSpeed = Speed::Slow;
    } else if(player->m_playerSpeed == 1.1f) {
        object->m_startSettings->m_startSpeed = Speed::Fast;
    } else if(player->m_playerSpeed == 1.3f) {
        object->m_startSettings->m_startSpeed = Speed::Faster;
    } else if(player->m_playerSpeed == 1.6f) {
        object->m_startSettings->m_startSpeed = Speed::Fastest;
    } else {
        object->m_startSettings->m_startSpeed = Speed::Normal;
    }

    object->m_startSettings->m_isFlipped = player->m_isUpsideDown;
    object->m_startSettings->m_startMini = player->m_vehicleSize < 1.0f;
    object->m_startSettings->m_reverseGameplay = player->m_isGoingLeft;
    object->m_startSettings->m_rotateGameplay = player->m_isSideways;
    object->m_startSettings->m_yVelocity = player->getYVelocity();
    object->setRotation(player->getRotation());
    object->rotationSpeed() = player->m_rotationSpeed;
    object->rotateSpeed() = player->m_rotateSpeed;
    object->isP2() = isP2;

    return object;
}

constexpr int calculateOffset(GJBaseGameLayer* bgl) {
    float center = (bgl->m_groundLayer->getPositionY() + bgl->m_groundLayer2->getPositionY()) / 2.f - bgl->m_gameState.m_cameraOffset.y;
    float offset = (center - bgl->m_player1->getPositionY() - bgl->m_player1->getParent()->getPositionY()) / 30.f;
    if(offset > 0) offset += .99f;
    return offset;
}

std::pair<HookedStartPosObject*, HookedStartPosObject*> createStartPositions() {
    auto bgl = getBGL();
    auto p1 = createStartPosShared(bgl->m_player1, false);

    p1->m_startSettings->m_startDual = bgl->m_gameState.m_isDualMode;
    p1->m_startSettings->m_mirrorMode = bgl->m_gameState.m_isFlipped;
    p1->m_startSettings->m_targetChannel = bgl->m_gameState.m_currentChannel;
    p1->m_startSettings->m_freeMode = bgl->m_gameState.m_isFreeMode;
    p1->m_startSettings->m_online = Settings::autoOnline;

    if(auto ui = modify_cast<HookedEditorUI*>(EditorUI::get()); ui && ui->m_fields->m_paused) {
        p1->cameraOffset() = ui->m_fields->m_offset;
    } else {
        p1->cameraOffset() = calculateOffset(bgl);
    }

    p1->saveExtraSettings();

    if(!bgl->m_gameState.m_isDualMode) {
        return {p1, nullptr};
    }

    auto p2 = createStartPosShared(bgl->m_player2, true);
    p2->m_isNoTouch = true;
    p2->setType(GameObjectType::Decoration); // disable collision

    int id = 1;
    while(bgl->m_fields->m_links.contains(id)) {
        ++id;
    }

    p1->linkId() = id;
    p2->linkId() = id;
    bgl->addLink(p1);
    bgl->addLink(p2);

    p2->saveExtraSettings();

    return {p1, p2};
}

void HookedCreateStartPosPopup::onCreate(CCObject* sender) {
    auto [p1, p2] = createStartPositions();
    auto nameInput = static_cast<CCTextInputNode*>(m_inputNodes->objectForKey(-1));
    p1->m_startSettings->m_startPosName = LevelTools::base64EncodeString(nameInput->getString());
    p1->m_startSettings->m_disableStartPos = true;

    auto pl = PlayLayer::get();
    if(p2 == nullptr) {
        pl->addLiveStartPosObject(p1);
        return CreateStartPosPopup::onClose(sender);
    }

    // based on PlayLayer::addLiveStartPosObject decomp, reimplemented to avoid recompressing the entire level string twice

    pl->addObject(p1);
    pl->addObject(p2);
    pl->sortStartPosObjects();

    std::string levelString = ZipUtils::decompressString(pl->m_level->m_levelString, false, 11);
    std::string p1String = p1->getSaveString(pl);
    std::string p2String = p2->getSaveString(pl);
    std::string newString = levelString + p1String + ";" + p2String + ";";
    pl->m_level->m_levelString = ZipUtils::compressString(newString, false, -1, 11);

    CreateStartPosPopup::onClose(sender);
}

void HookedEditorUI::onCreateStartPos(CCObject*) {
    auto [p1, p2] = createStartPositions();
    auto lel = LevelEditorLayer::get();
    lel->addToSection(p1);
    lel->addSpecial(p1);

    if(p2 == nullptr) {
        lel->m_undoObjects->addObject(UndoObject::create(p1, UndoCommand::New));
    } else {
        lel->addToSection(p2);
        lel->addSpecial(p2);
        CCArray* objects = CCArray::create();
        objects->addObject(p1);
        objects->addObject(p2);
        lel->m_undoObjects->addObject(UndoObject::createWithArray(objects, UndoCommand::Paste));
    }
}

bool HookedEditorUI::init(LevelEditorLayer* editorLayer) {
    if(!EditorUI::init(editorLayer)) return false;

    this->addEventListener(KeybindSettingPressedEvent(Mod::get(), "create-startpos-keybind"), [this](const Keybind& keybind, bool down, bool repeat, double) {
        if(down && !repeat && m_playtestStopBtn->isVisible()) {
            onCreateStartPos(nullptr);
        }
    });

    if(Settings::hideStartPosButton) return true;

    m_fields->m_createStartPosButton = CCMenuItemSpriteExtra::create(
        CircleButtonSprite::createWithSprite("start-pos.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::Small),
        this,
        menu_selector(HookedEditorUI::onCreateStartPos)
    );
    m_fields->m_createStartPosButton->setID("create-startpos-button"_spr);
    m_fields->m_createStartPosButton->setVisible(false);

    auto playtestMenu = this->getChildByID("playtest-menu");
    playtestMenu->addChild(m_fields->m_createStartPosButton);
    playtestMenu->setContentWidth(playtestMenu->getContentWidth() * 1.5f);
    playtestMenu->updateLayout();

    return true;
}

void HookedEditorUI::onPlaytest(CCObject* sender) {
    m_fields->m_paused = true;
    m_fields->m_offset = calculateOffset(m_editorLayer);
    EditorUI::onPlaytest(sender);
}

void HookedLevelEditorLayer::onPlaytest() {
    auto ui = modify_cast<HookedEditorUI*>(m_editorUI);
    ui->m_fields->m_paused = false;
    if(auto btn = ui->m_fields->m_createStartPosButton) {
        btn->setVisible(true);
    }
    LevelEditorLayer::onPlaytest();
}

void HookedLevelEditorLayer::onStopPlaytest() {
    if(auto btn = modify_cast<HookedEditorUI*>(m_editorUI)->m_fields->m_createStartPosButton) {
        btn->setVisible(false);
    }
    LevelEditorLayer::onStopPlaytest();
}

bool HookedCreateStartPosPopup::init() { 
    if(!CreateStartPosPopup::init()) return false;
    // remove dual & camera inaccuracy warning
    m_buttonMenu->getChildByType<InfoAlertButton*>()->m_description = "Creates a start position here with the current settings.";
    return true;
}
