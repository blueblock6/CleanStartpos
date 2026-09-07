#include "ExtraPopup.hpp"
#include "ui/UIUtil.hpp"

using namespace geode::prelude;

bool ExtraPopup::init(AdvancedStartPos* startPos) {
    if(!Popup::init({455.f, 250.f})) return false;

    m_noElasticity = true;

    m_startPos = startPos;
    m_settings = startPos->getSettingsObject();
    bool isP2 = startPos->isP2();

    this->setTitle("Extra Settings", "goldFont.fnt", .7f, 15.f);
    m_buttonMenu->getChildByType()->setVisible(false);

    auto okBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("OK"),
        this,
        menu_selector(ExtraPopup::onClose)
    );
    m_buttonMenu->addChildAtPosition(okBtn, Anchor::Center, {0.f, -84.f});

    auto infoBtn = InfoAlertButton::create(
        "Help",
        "These are more advanced options. It is better to let these get set by the Create StartPos button / keybind instead of setting them manually.\n"
        "<cr>Velocity</c> sets player Y velocity\n"
        "<cr>Offset</c> shifts the foor & ceiling\n"
        "<cr>Rotation</c> sets player rotation\n"
        "<cr>Rotation Speed</c> only affects Cube and Ball\n"
        "<cr>Ball Rotation</c> multiplies ball rotation speed (caused by slopes)",
        1.f
    );
    m_buttonMenu->addChildAtPosition(infoBtn, Anchor::TopLeft);

    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_velocityInput, "Velocity", CommonFilter::Float, "Velocity", fmt::format("{:.3f}", m_settings->m_fields->yVelocity).c_str()),
        Anchor::Center, {-120.f, 64.f}, false
    );
    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_offsetInput, "Offset", CommonFilter::Int, "Offset", std::to_string(m_settings->m_fields->cameraOffset), !isP2),
        Anchor::Center, {0.f, 64.f}, false
    );
    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_rotationInput, "Rotation", CommonFilter::Float, "Rotation", fmt::format("{:.3f}", startPos->getRotation()).c_str()),
        Anchor::Center, {120.f, 64.f}, false
    );

    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_targetOrderInput, "Target Order", CommonFilter::Int, "Order", std::to_string(m_settings->m_targetOrder), !isP2),
        Anchor::Center, {-80.f, -5.f}, false
    );
    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_targetChannelInput, "Target Channel", CommonFilter::Int, "Channel", std::to_string(m_settings->m_targetChannel), !isP2),
        Anchor::Center, {80.f, -5.f}, false
    );

    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_rotationSpeedInput, "Rotation Speed", CommonFilter::Float, "Speed", fmt::format("{:.3f}", m_settings->m_fields->rotationSpeed).c_str()),
        Anchor::Center, {-80.f, -74.f}, false
    );
    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_rotateSpeedInput, "Ball Rotation", CommonFilter::Float, "Speed", fmt::format("{:.3f}", m_settings->m_fields->rotateSpeed).c_str()),
        Anchor::Center, {80.f, -74.f}, false
    );

    auto leftMenu = CCMenu::create();
    leftMenu->setContentSize({100, 135});
    leftMenu->addChild(createToggle("Mirror", ToggleType::MIRROR, m_settings->m_mirrorMode, m_settings, !isP2));
    leftMenu->addChild(createToggle("Free Cam", ToggleType::FREE, m_settings->m_fields->isFreeCam, m_settings, !isP2));
    leftMenu->addChild(createToggle("Reset Camera", ToggleType::RESET, m_settings->m_resetCamera, m_settings, !isP2));
    leftMenu->setLayout(ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
    m_mainLayer->addChildAtPosition(leftMenu, Anchor::Center, {-180.f, -34.f});

    auto rightMenu = CCMenu::create();
    rightMenu->setContentSize({100, 135});
    rightMenu->addChild(createToggle("Rotate", ToggleType::ROTATE, m_settings->m_rotateGameplay, m_settings, !isP2));
    rightMenu->addChild(createToggle("Reverse", ToggleType::REVERSE, m_settings->m_reverseGameplay, m_settings));

    auto settingsBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_optionsBtn02_001.png", 1.f, [](auto){
        openSettingsPopup(Mod::get(), false);
    });
    rightMenu->addChild(settingsBtn);

    rightMenu->setLayout(ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
    m_mainLayer->addChildAtPosition(rightMenu, Anchor::Center, {180.f, -34.f});

    return true;
}

void ExtraPopup::onClose(CCObject* sender) {
    m_settings->m_fields->yVelocity = utils::numFromString<float>(m_velocityInput->getString()).unwrapOrDefault();
    m_settings->m_fields->cameraOffset = utils::numFromString<int>(m_offsetInput->getString()).unwrapOrDefault();
    m_startPos->setRotation(utils::numFromString<float>(m_rotationInput->getString()).unwrapOrDefault());
    m_settings->m_targetOrder = utils::numFromString<float>(m_targetOrderInput->getString()).unwrapOrDefault();
    m_settings->m_targetChannel = utils::numFromString<float>(m_targetChannelInput->getString()).unwrapOrDefault();
    m_settings->m_fields->rotationSpeed = utils::numFromString<float>(m_rotationSpeedInput->getString()).unwrapOrDefault();
    m_settings->m_fields->rotateSpeed = utils::numFromString<float>(m_rotateSpeedInput->getString()).unwrapOrDefault();
    Popup::onClose(sender);
}

ExtraPopup* ExtraPopup::create(AdvancedStartPos* startPos) {
    auto ret = new ExtraPopup();
    if(ret && ret->init(startPos)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}
