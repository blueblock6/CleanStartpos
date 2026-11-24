#include <Geode/binding/ValueControlSettings.hpp>
#include "StartPosExtraSettingsPopup.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

constexpr int ROTATION_PROPERTY_TAG = -1;
constexpr int ROTATION_SPEED_PROPERTY_TAG = -2;
constexpr int ROTATE_SPEED_PROPERTY_TAG = -3;
constexpr int P2_GROUP_ID_PROPERTY_TAG = -4;

constexpr void idPieces(CCArray* objects, std::string name) {
    static_cast<CCNode*>(objects->objectAtIndex(0))->setID(name + "-label");
    static_cast<CCNode*>(objects->objectAtIndex(1))->setID(name + "-bg");
    static_cast<CCNode*>(objects->objectAtIndex(2))->setID(name + "-input");
}

constexpr void disableButton(CCMenuItemSpriteExtra* button) {
    button->getChildByType<CCSprite*>()->setOpacity(125);
    button->setEnabled(false);
}

StartPosExtraSettingsPopup* StartPosExtraSettingsPopup::create(HookedStartPosObject* startPosObject) {
    auto ret = new StartPosExtraSettingsPopup();
    if(ret && ret->init(startPosObject)) {
        ret->autorelease();
        return ret;
    } else {
        delete ret;
        return nullptr;
    }
}

bool StartPosExtraSettingsPopup::init(HookedStartPosObject* startPosObject) {
    if(!SetupTriggerPopup::init(nullptr, nullptr, 240, 160, 1, 0)) return false;
    m_startPosObject = startPosObject;

    SetupTriggerPopup::preSetup();

    auto okButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("OK", .8f),
        this,
        menu_selector(StartPosExtraSettingsPopup::onClose)
    );
    okButton->setID("ok-button");
    m_buttonMenu->addChild(okButton);

    auto settingsBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_optionsBtn02_001.png", .8f, [](auto){
        openSettingsPopup(Mod::get(), false);
    });
    settingsBtn->setID("settings-button");
    m_buttonMenu->addChildAtPosition(settingsBtn, Anchor::BottomLeft, {95.f, 0.f}, false);

    auto infoBtn = InfoAlertButton::create(
        "Start Pos Extra", 
        "<cg>Rotation:</c> Sets player rotation.\n"
        "<cy>Rotation Speed:</c> Sets rotation speed for Cube and Ball.\n"
        "<cl>Ball Speed:</c> Amplifies rotation speed of Ball.\n"
        "<cp>P2 Group ID:</c> If not '0', spawns player 2 at this Group ID.\n",
        .9f
    );
    infoBtn->setID("info-button");
    m_buttonMenu->addChildAtPosition(infoBtn, Anchor::BottomLeft, {118.f, 134.f}, false);

    CCPoint center = CCDirector::get()->getWinSize() / 2;

    ValueControlSettings inputSettings = {
        .m_scale = .6f,
        .m_noSlider = true,
        .m_valueType = InputValueType::Default,
        .m_maxDigits = 4,
        .m_inputStyle = GJInputStyle::GoldLabel,
        .m_decimalPlaces = 3,
        .m_vertical = true,
        .m_width = 140.f,
        .m_font = "goldFont.fnt",
        .m_labelScale = .7f,
        .m_placeholderText = "Num"
    };

    inputSettings.m_property = ROTATION_PROPERTY_TAG;
    inputSettings.m_label = "Rotation:";
    inputSettings.m_position = center;
    inputSettings.m_position += {-60.f, 35.f};
    idPieces(SetupTriggerPopup::createValueControlAdvanced(inputSettings), "rotation");

    inputSettings.m_property = ROTATION_SPEED_PROPERTY_TAG;
    inputSettings.m_label = "Rotation Speed:";
    inputSettings.m_position = center;
    inputSettings.m_position += {60.f, 35.f};
    idPieces(SetupTriggerPopup::createValueControlAdvanced(inputSettings), "rotation-speed");

    inputSettings.m_property = ROTATE_SPEED_PROPERTY_TAG;
    inputSettings.m_label = "Ball Speed:";
    inputSettings.m_position = center;
    inputSettings.m_position += {-60.f, -15.f};
    idPieces(SetupTriggerPopup::createValueControlAdvanced(inputSettings), "rotate-speed");

    auto p2Pos = center;
    p2Pos += {60.f, -15.f};
    auto input = IDNameInput::create(this, this, 1, P2_GROUP_ID_PROPERTY_TAG, "P2 Group ID:", p2Pos, .6f, 6, 0, 0, false);
    input->m_background->setID("p2-group-id-background");
    input->m_textInput->setID("p2-group-id-input");
    input->m_label->setID("p2-group-id-label");
    input->m_groupName->setID("p2-group-id-name-label");
    input->m_groupNameShadow->setID("p2-group-id-shadow-label");
    input->m_leftButton->setID("p2-group-id-left");
    input->m_rightButton->setID("p2-group-id-right");
    input->m_nextButton->setID("p2-group-id-next");
    input->m_searchButton->setID("p2-group-id-search");

    if(startPosObject->isP2()) {
        input->m_textInput->setVisible(false);
        disableButton(input->m_leftButton);
        disableButton(input->m_rightButton);
        disableButton(input->m_nextButton);
        disableButton(input->m_searchButton);
    }

    SetupTriggerPopup::postSetup();
    return true;
}

float StartPosExtraSettingsPopup::getValue(int tag) {
    switch(tag) {
    case ROTATION_PROPERTY_TAG:
        return m_startPosObject->getRotation();
    case ROTATION_SPEED_PROPERTY_TAG:
        return m_startPosObject->rotationSpeed();
    case ROTATE_SPEED_PROPERTY_TAG:
        return m_startPosObject->rotateSpeed();
    case P2_GROUP_ID_PROPERTY_TAG:
        return m_startPosObject->m_startSettings->m_player2SpawnGroup;
    default:
        return 0;
    }
}

void StartPosExtraSettingsPopup::valueDidChange(int tag, float value) {
    switch(tag) {
    case ROTATION_PROPERTY_TAG:
        m_startPosObject->setRotation(value); break;
    case ROTATION_SPEED_PROPERTY_TAG:
        m_startPosObject->rotationSpeed() = value; break;
    case ROTATE_SPEED_PROPERTY_TAG:
        m_startPosObject->rotateSpeed() = value; break;
    case P2_GROUP_ID_PROPERTY_TAG:
        m_startPosObject->m_startSettings->m_player2SpawnGroup = value; break;
    }
}
