#include "AdvancedUI.hpp"
#include "UIUtil.hpp"
#include "RotationPopup.hpp"

using namespace geode::prelude;

bool AdvancedUI::init(AdvancedStartPos* startPos) {
    if(!Popup::init({455.f, 250.f})) return false;

    this->ignoreAnchorPointForPosition(false);
    this->setAnchorPoint({0.f, 0.f});

    m_mainLayer->ignoreAnchorPointForPosition(false);
    m_noElasticity = true;

    m_buttonMenu->getChildByType()->setVisible(false);
    auto okBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("OK"),
        this,
        menu_selector(AdvancedUI::onClose)
    );
    m_buttonMenu->addChildAtPosition(okBtn, Anchor::Center, {0.f, -100.f});

    m_startPos = startPos;
    m_settings = startPos->getSettingsObject();
    m_isP2 = startPos->isP2();
    
    auto modeMenu = createModeMenu(m_settings);
    m_mainLayer->addChildAtPosition(modeMenu, Anchor::Center, {0.f, 90.f});

    auto speedMenu = createSpeedMenu(startPos);
    m_mainLayer->addChildAtPosition(speedMenu, Anchor::Center, {-5.f, 40.f});

    auto leftMenu = CCMenu::create();
    leftMenu->setContentSize({100.f, 225.f});
    leftMenu->addChild(createToggle("Flip", ToggleType::FLIP, m_settings->m_isFlipped, m_settings));
    leftMenu->addChild(createToggle("Mini", ToggleType::MINI, m_settings->m_startMini, m_settings));

    auto [dualToggle, linkMenu] = createDualToggleAndMenu(startPos, this);
    leftMenu->addChild(dualToggle);
    m_mainLayer->addChildAtPosition(linkMenu, Anchor::Center, {-220.f, -20.f});

    leftMenu->addChild(createToggle("Mirror", ToggleType::MIRROR, m_settings->m_mirrorMode, m_settings));
    leftMenu->addChild(createToggle("Free Mode", ToggleType::FREE, m_settings->m_fields->isFreeCam, m_settings));
    leftMenu->setLayout(ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
    m_mainLayer->addChildAtPosition(leftMenu, Anchor::Center, {-180.f, 0.f});

    auto rightMenu = CCMenu::create();
    rightMenu->setContentSize({100.f, 225.f});
    rightMenu->addChild(createToggle("Rotate", ToggleType::ROTATE, m_settings->m_rotateGameplay, m_settings));
    rightMenu->addChild(createToggle("Reverse", ToggleType::REVERSE, m_settings->m_reverseGameplay, m_settings));
    rightMenu->addChild(createToggle("Reset Camera", ToggleType::RESET, m_settings->m_resetCamera, m_settings));
    rightMenu->addChild(createToggle("Disable", ToggleType::DISABLE, m_settings->m_disableStartPos, m_settings));

    auto settingsBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_optionsBtn02_001.png", 1.f, [](auto){
        openSettingsPopup(Mod::get(), false);
    });
    rightMenu->addChild(settingsBtn);

    rightMenu->setLayout(ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
    m_mainLayer->addChildAtPosition(rightMenu, Anchor::Center, {180.f, 0.f});

    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_velocityInput, "Velocity", CommonFilter::Float, "Velocity", fmt::format("{:.3f}", m_settings->m_fields->yVelocity).c_str()),
        Anchor::Center, {-80.f, -25.f}, false
    );
    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_offsetInput, "Offset", CommonFilter::Int, "Offset", std::to_string(m_settings->m_fields->cameraOffset), !m_isP2),
        Anchor::Center, {80.f, -25.f}, false
    );
    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_targetOrderInput, "Target Order", CommonFilter::Int, "Order", std::to_string(m_settings->m_targetOrder), !m_isP2),
        Anchor::Center, {-80.f, -89.f}, false
    );
    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_targetChannelInput, "Target Channel", CommonFilter::Int, "Channel", std::to_string(m_settings->m_targetChannel), !m_isP2),
        Anchor::Center, {80.f, -89.f}, false
    );

    auto extraSettingsMenu = CCMenu::create();
    extraSettingsMenu->setContentSize({80.f, 80.f});
    auto extraSettingsLabel = CCLabelBMFont::create("Extra", "goldFont.fnt");
    extraSettingsLabel->setScale(.6f);
    extraSettingsMenu->addChildAtPosition(extraSettingsLabel, Anchor::Top, {0.f, -10.f});
    auto extraSettingsBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_optionsBtn_001.png", .75f, [startPos](auto) {
        RotationPopup::create(startPos)->show();
    });
    extraSettingsMenu->addChildAtPosition(extraSettingsBtn, Anchor::Center);
    m_mainLayer->addChildAtPosition(extraSettingsMenu, Anchor::Center, {0.f, -36.f});

    if(!Mod::get()->getSettingValue<bool>("disable-watermark")) {
        auto watermark = CCLabelBMFont::create("Clean StartPos by BlueBlock6", "chatFont.fnt");
        watermark->setAlignment(cocos2d::kCCTextAlignmentRight);
        watermark->setScale(.5f);
        watermark->setOpacity(125);
        watermark->setAnchorPoint({1.f, 0.f});
        m_mainLayer->addChildAtPosition(watermark, Anchor::BottomRight, {-5.f, 5.f}, false);
    }

    return true;
}

void AdvancedUI::onClose(CCObject* sender) {
    m_settings->m_fields->yVelocity = utils::numFromString<float>(m_velocityInput->getString()).unwrapOrDefault();
    m_settings->m_fields->cameraOffset = utils::numFromString<int>(m_offsetInput->getString()).unwrapOrDefault();
    m_settings->m_targetOrder = utils::numFromString<int>(m_targetOrderInput->getString()).unwrapOrDefault();
    m_settings->m_targetChannel = utils::numFromString<int>(m_targetChannelInput->getString()).unwrapOrDefault();
    m_startPos->encodeSettings(m_settings);
    Popup::onClose(sender);
}

AdvancedUI* AdvancedUI::create(AdvancedStartPos* startPos) {
    auto ret = new AdvancedUI();
    if(ret && ret->init(startPos)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}
