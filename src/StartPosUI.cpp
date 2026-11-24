#include "Hooks.hpp"
#include "StartPosExtraSettingsPopup.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

constexpr int DUAL_PROPERTY_TAG = -3;
constexpr int CAMERA_OFFSET_PROPERTY_TAG = -20;

constexpr void disableButton(CCObject* node) {
    auto button = static_cast<CCMenuItemSpriteExtra*>(node);
    button->setEnabled(false);
    button->getChildByType<CCSprite*>(0)->setOpacity(125);
}

constexpr void disableToggler(CCObject* node) {
    auto button = static_cast<CCMenuItemToggler*>(node);
    button->setEnabled(false);
    disableButton(button->m_offButton);
    disableButton(button->m_onButton);
}

bool HookedStartPosLayer::init(StartPosObject* startPosObject) {
    auto startPos = modify_cast<HookedStartPosObject*>(startPosObject);
    startPos->loadExtraSettings();
    startPos->mergeP1Settings();
    startPos->m_startSettings->m_platformerMode = LevelEditorLayer::get()->m_isPlatformer;

    if(!StartPosLayer::init(startPosObject)) return false;

    m_buttonMenu->getChildByType<InfoAlertButton*>()->m_description =
        "<cg>Y Velocity:</c> Adds speed up or down.\n"
        "<cy>Spawn Group ID:</c> Spawns a specific Group ID.\n"
        "<cl>Offset:</c> Offsets camera center.\n"
        "<cp>SP Name:</c> Gives the start position a name in menus.\n"
        "<cr>Free:</c> Removes borders for certain game modes.\n"
        "<co>Reset:</c> Resets the camera.\n"
        "<ca>Inactive:</c> Disables the start position for immediate spawning. Can still switch to it using start position switcher.\n"
        "<cb>Online:</c> Shows the start position in the Practice mode switcher when the level is played online.";

    m_mainLayer->getChildByID("p2-group-id-label")->setVisible(false);
    m_mainLayer->getChildByID("p2-group-id-background")->setVisible(false);
    m_mainLayer->getChildByID("p2-group-id-input")->setVisible(false);
    m_buttonMenu->getChildByID("p2-group-id-left")->setVisible(false);
    m_buttonMenu->getChildByID("p2-group-id-right")->setVisible(false);
    m_buttonMenu->getChildByID("p2-group-id-next")->setVisible(false);
    m_buttonMenu->getChildByID("p2-group-id-search")->setVisible(false);

    if(startPos->isP2()) {
        disableButton(m_buttonMenu->getChildByID("slow-speed-button"));
        disableButton(m_buttonMenu->getChildByID("normal-speed-button"));
        disableButton(m_buttonMenu->getChildByID("fast-speed-button"));
        disableButton(m_buttonMenu->getChildByID("faster-speed-button"));
        disableButton(m_buttonMenu->getChildByID("fastest-speed-button"));

        disableToggler(m_buttonMenu->getChildByID("dual-toggle"));
        disableToggler(m_buttonMenu->getChildByID("mirror-toggle"));
        disableToggler(m_buttonMenu->getChildByID("free-toggle"));
        disableToggler(m_buttonMenu->getChildByID("reset-toggle"));
        disableToggler(m_buttonMenu->getChildByID("online-toggle"));

        m_mainLayer->getChildByID("spawn-group-id-input")->setVisible(false);
        disableButton(m_buttonMenu->getChildByID("spawn-group-id-left"));
        disableButton(m_buttonMenu->getChildByID("spawn-group-id-right"));
        disableButton(m_buttonMenu->getChildByID("spawn-group-id-next"));
        disableButton(m_buttonMenu->getChildByID("spawn-group-id-search"));

        m_mainLayer->getChildByID("sp-name-input")->setVisible(false);

        m_mainLayer->getChildByID("order-input")->setVisible(false);
        disableButton(m_buttonMenu->getChildByID("order-left"));
        disableButton(m_buttonMenu->getChildByID("order-right"));

        m_mainLayer->getChildByID("channel-input")->setVisible(false);
        disableButton(m_buttonMenu->getChildByID("channel-left"));
        disableButton(m_buttonMenu->getChildByID("channel-right"));
    }

    auto dualToggle = static_cast<CCMenuItemToggler*>(m_buttonMenu->getChildByID("dual-toggle"));

    auto linkSpr = CCSprite::createWithSpriteFrameName("gj_linkBtn_001.png");
    linkSpr->setScale(.8f);
    m_fields->m_linkButton = CCMenuItemSpriteExtra::create(
        linkSpr,
        this,
        menu_selector(HookedStartPosLayer::onLink)
    );
    m_fields->m_linkButton->setID("link-button"_spr);
    m_fields->m_linkButton->setVisible(
        startPos->isP2() ||
        startPos->m_startSettings->m_startDual ||
        startPos->getOther()
    );
    auto linkPos = dualToggle->getPosition();
    linkPos -= {25.f, 0.f};
    m_fields->m_linkButton->setPosition(linkPos);
    m_buttonMenu->addChild(m_fields->m_linkButton);

    auto unlinkSpr = CCSprite::createWithSpriteFrameName("gj_linkBtnOff_001.png");
    unlinkSpr->setScale(.8f);
    m_fields->m_unlinkButton = CCMenuItemSpriteExtra::create(
        unlinkSpr,
        this,
        menu_selector(HookedStartPosLayer::onUnlink)
    );
    m_fields->m_unlinkButton->setID("unlink-button"_spr);
    m_fields->m_unlinkButton->setVisible(
        startPos->isP2() ||
        startPos->getOther()
    );
    auto unlinkPos = dualToggle->getPosition();
    unlinkPos += {25.f, 0.f};
    m_fields->m_unlinkButton->setPosition(unlinkPos);
    m_buttonMenu->addChild(m_fields->m_unlinkButton);

    auto offsetInputPieces = this->createValueControlAdvanced(
        CAMERA_OFFSET_PROPERTY_TAG, "Offset:",
        m_mainLayer->getChildByID("p2-group-id-background")->getPosition(), .6f,
        true, InputValueType::Int, 2, true,
        0.f, 0.f, 0, 0,
        GJInputStyle::GoldLabel, 0, false
    );
    static_cast<CCNode*>(offsetInputPieces->objectAtIndex(0))->setID("offset-label"_spr);
    static_cast<CCNode*>(offsetInputPieces->objectAtIndex(1))->setID("offset-bg"_spr);
    static_cast<CCNode*>(offsetInputPieces->objectAtIndex(2))->setID("offset-input"_spr);
    static_cast<CCNode*>(offsetInputPieces->objectAtIndex(3))->setID("offset-left"_spr);
    static_cast<CCNode*>(offsetInputPieces->objectAtIndex(4))->setID("offset-right"_spr);
    if(startPos->isP2()) {
        static_cast<CCNode*>(offsetInputPieces->objectAtIndex(2))->setVisible(false);
        disableButton(offsetInputPieces->objectAtIndex(3));
        disableButton(offsetInputPieces->objectAtIndex(4));
    }
    this->postSetup();

    auto extraSettingsLabel = CCLabelBMFont::create("Extra", "goldFont.fnt");
    extraSettingsLabel->setID("extra-settings-label"_spr);
    extraSettingsLabel->setScale(.49f);
    extraSettingsLabel->setAnchorPoint({.5f, 0.f});
    m_mainLayer->addChildAtPosition(extraSettingsLabel, Anchor::Center, {}, false);

    auto extraSettingsButton = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_optionsBtn_001.png", .75f, [startPos](auto) {
        StartPosExtraSettingsPopup::create(startPos)->show();
    });
    extraSettingsButton->setID("extra-settings-button"_spr);
    m_buttonMenu->addChildAtPosition(extraSettingsButton, Anchor::BottomLeft, {0.f, -20.f}, false);

    return true;
}

void HookedStartPosLayer::onLink(CCObject*) {
    auto startPos = modify_cast<HookedStartPosObject*>(m_startPosObject);
    auto eu = EditorUI::get();

    if(auto other = startPos->getOther()) {
        this->onClose(nullptr);
        eu->selectObject(other, true);
        eu->updateButtons();
        eu->editObject(nullptr);
        return;
    }

    if(startPos->isP2()) {
        log::error("Stranded P2 StartPos {}", startPos->getPosition());
        Notification::create("Could not find main StartPos!", NotificationIcon::Error)->show();
        return;
    }

    short& id = startPos->linkId();
    auto bgl = getBGL();
    if(id == 0) {
        id = 1;
        while(bgl->m_fields->m_links.contains(id)) {
            ++id;
        }
        bgl->addLink(startPos);
    }
    auto pos = startPos->m_startPosition;
    pos.y += 60;
    auto obj = modify_cast<HookedStartPosObject*>(eu->m_editorLayer->createObject(34, pos, false));
    obj->linkId() = id;
    bgl->addLink(obj);
    startPos->getOther() = obj;
    obj->m_isNoTouch = true;
    obj->setType(GameObjectType::Decoration); // disable collision

    this->onClose(nullptr);
    eu->selectObject(obj, true);
    eu->updateButtons();
}

void HookedStartPosLayer::onUnlink(CCObject*) {
    auto startPos = modify_cast<HookedStartPosObject*>(m_startPosObject);
    auto lel = LevelEditorLayer::get();
    if(startPos->isP2()) {
        this->onClose(nullptr);
        lel->removeObject(startPos, false);
        lel->m_editorUI->deselectAll();
    } else if(auto other = startPos->getOther()) {
        lel->removeObject(other, false);
    }
}

float HookedStartPosLayer::getValue(int tag) {
    if(tag == CAMERA_OFFSET_PROPERTY_TAG) {
        return modify_cast<HookedStartPosObject*>(m_startPosObject)->cameraOffset();
    } else {
        return StartPosLayer::getValue(tag);
    }
}

void HookedStartPosLayer::valueDidChange(int tag, float value) {
    auto startPos = modify_cast<HookedStartPosObject*>(m_startPosObject);

    if(tag == CAMERA_OFFSET_PROPERTY_TAG) {
        startPos->cameraOffset() = value;
    } else {
        StartPosLayer::valueDidChange(tag, value);
        if(tag == DUAL_PROPERTY_TAG) {
            m_fields->m_linkButton->setVisible(
                startPos->isP2() ||
                startPos->m_startSettings->m_startDual ||
                startPos->getOther()
            );

            m_fields->m_unlinkButton->setVisible(
                startPos->isP2() ||
                startPos->getOther()
            );
        }
    }
}

void HookedStartPosLayer::onClose(CCObject* sender) {
    auto startPos = modify_cast<HookedStartPosObject*>(m_startPosObject);
    StartPosLayer::onClose(sender);
    startPos->saveExtraSettings();
}
