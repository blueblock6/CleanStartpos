#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Links.hpp>
#include <geode.custom-keybinds/include/OptionalAPI.hpp>
#include <Geode/loader/Dispatch.hpp>

using namespace geode::prelude;
using namespace keybinds;

$on_game(Loaded) {
    (void)[&]() -> Result<> {
        // CategoryV2::create returns Err if mod id is before geode.custom-keybinds? But manually extracting the function works?
        static auto CategoryV2_create = geode::geode_internal::callEventExportListener(&CategoryV2::create, "geode.custom-keybinds/CategoryV2::create");
        if(!CategoryV2_create) return Ok();

        GEODE_UNWRAP(BindManagerV2::registerBindable(GEODE_UNWRAP(BindableActionV2::create(
            "create-startpos"_spr,
            "Create StartPos",
            "Creates a Start Position at the player's position with all important gameplay elements",
            { GEODE_UNWRAP(KeybindV2::create(cocos2d::KEY_Apostrophe, ModifierV2::None)) },
            GEODE_UNWRAP(CategoryV2_create("Editor"))
        ))));
        return Ok();
    }();
}

class $modify(CEditorUI, EditorUI) {
    struct Fields {
        CCMenuItemSpriteExtra* createStartPosBtn = nullptr;
    };

    $override
    bool init(LevelEditorLayer* lel) {
        if(!EditorUI::init(lel)) return false;

        this->addEventListener<InvokeBindFilterV2>([this](InvokeBindEventV2* event) {
            if(event->isDown() && m_playtestStopBtn->isVisible()) {
                onCreateStartPos(nullptr);
            }
            return ListenerResult::Propagate;
        }, "create-startpos"_spr);

        if(Mod::get()->getSettingValue<bool>("hide-startpos-button")) return true;

        m_fields->createStartPosBtn = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSpriteFrameName("edit_eStartPosBtn_001.png", 1.f, CircleBaseColor::Green, CircleBaseSize::Small),
            this,
            menu_selector(CEditorUI::onCreateStartPos)
        );
        m_fields->createStartPosBtn->setID("create-startpos-button"_spr);
        m_fields->createStartPosBtn->setVisible(false);

        auto playtestMenu = this->getChildByID("playtest-menu");
        playtestMenu->addChild(m_fields->createStartPosBtn);
        playtestMenu->setContentWidth(playtestMenu->getContentWidth() + 50.f);
        playtestMenu->updateLayout();
        playtestMenu->setPositionX(playtestMenu->getPositionX() + 25);

        return true;
    }

    void onCreateStartPos(CCObject*) {
        auto pl = m_editorLayer->m_player1;
        auto startPos = static_cast<AdvancedStartPos*>(m_editorLayer->createObjectsFromString(fmt::format("1,31,2,{},3,{}", pl->getPositionX(), pl->getPositionY() - 90).c_str(), false, false)->firstObject());

        auto settings = startPos->getSettingsObject();
        settings->m_startsWithStartPos = true;
        settings->m_startMode = pl->isInNormalMode() ? 0 : pl->m_isShip ? 1 : pl->m_isBall ? 2 : pl->m_isBird ? 3 : pl->m_isDart ? 4 : pl->m_isRobot ? 5 : pl->m_isSpider ? 6 : 7;
        float speed = pl->m_playerSpeed;
        settings->m_startSpeed = speed == .7f ? Speed::Slow : speed == .9f ? Speed::Normal : speed == 1.1f ? Speed::Fast : speed == 1.3f ? Speed::Faster : Speed::Fastest;
        settings->m_isFlipped = pl->m_isUpsideDown;
        settings->m_startMini = pl->m_vehicleSize != 1.f;
        settings->m_startDual = m_editorLayer->m_gameState.m_isDualMode;
        settings->m_rotateGameplay = pl->m_isSideways;
        settings->m_reverseGameplay = pl->m_isGoingLeft;
        settings->m_targetChannel = m_editorLayer->m_gameState.m_currentChannel;
        settings->m_fields->yVelocity = static_cast<float>(pl->m_yVelocity);
        // TODO offset
        startPos->encodeSettings(settings);

        if(m_editorLayer->m_gameState.m_isDualMode) {
            pl = m_editorLayer->m_player2;
            auto startPos2 = static_cast<AdvancedStartPos*>(m_editorLayer->createObjectsFromString(fmt::format("1,34,2,{},3,{},121,1", pl->getPositionX(), pl->getPositionY() - 90).c_str(), false, false)->firstObject());

            short id = Links::nextId();
            startPos->m_linkId = id;
            startPos2->m_linkId = id;
            Links::insertPair(startPos, startPos2, id);

            settings = startPos2->getSettingsObject();
            settings->m_startMode = pl->isInNormalMode() ? 0 : pl->m_isShip ? 1 : pl->m_isBall ? 2 : pl->m_isBird ? 3 : pl->m_isDart ? 4 : pl->m_isRobot ? 5 : pl->m_isSpider ? 6 : 7;
            settings->m_isFlipped = pl->m_isUpsideDown;
            settings->m_startMini = pl->m_vehicleSize != 1.f;
            settings->m_reverseGameplay = pl->m_isGoingLeft;
            settings->m_fields->yVelocity = static_cast<float>(pl->m_yVelocity);
            startPos2->encodeSettings(settings);

            auto arr = CCArray::create();
            arr->addObject(startPos);
            arr->addObject(startPos2);
            m_editorLayer->m_undoObjects->addObject(UndoObject::createWithArray(arr, UndoCommand::Paste));
        } else {
            m_editorLayer->m_undoObjects->addObject(UndoObject::create(startPos, UndoCommand::New));
        }
    }
};

class $modify(LevelEditorLayer) {
    $override
    void onPlaytest() {
        if(auto btn = static_cast<CEditorUI*>(m_editorUI)->m_fields->createStartPosBtn) {
            btn->setVisible(true);
        }
        LevelEditorLayer::onPlaytest();
    }

    $override
    void onStopPlaytest() {
        if(auto btn = static_cast<CEditorUI*>(m_editorUI)->m_fields->createStartPosBtn) {
            btn->setVisible(false);
        }
        LevelEditorLayer::onStopPlaytest();
    }
};
