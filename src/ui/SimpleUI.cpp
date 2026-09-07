#include "SimpleUI.hpp"
#include "UIUtil.hpp"
#include "ExtraPopup.hpp"

using namespace geode::prelude;

bool SimpleUI::init(AdvancedStartPos* startPos) {
    if(!Popup::init({455.f, 200.f})) return false;

    this->ignoreAnchorPointForPosition(false);
    this->setAnchorPoint({0.f, 0.f});

    m_mainLayer->ignoreAnchorPointForPosition(false);
    m_noElasticity = true;

    m_buttonMenu->getChildByType()->setVisible(false);
    auto okBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("OK"),
        this,
        menu_selector(SimpleUI::onClose)
    );
    m_buttonMenu->addChildAtPosition(okBtn, Anchor::Center, {0.f, -70.f});

    m_startPos = startPos;
    m_settings = startPos->getSettingsObject();
    m_isP2 = startPos->isP2();
    
    auto modeMenu = createModeMenu(m_settings);
    m_mainLayer->addChildAtPosition(modeMenu, Anchor::Center, {0.f, 60.f});

    auto speedMenu = createSpeedMenu(startPos);
    m_mainLayer->addChildAtPosition(speedMenu, Anchor::Center, {-5.f, 0.f});

    auto toggleMenu = CCMenu::create();
    toggleMenu->setContentSize({100.f, 185.f});
    toggleMenu->addChild(createToggle("Flip", ToggleType::FLIP, m_settings->m_isFlipped, m_settings));
    toggleMenu->addChild(createToggle("Mini", ToggleType::MINI, m_settings->m_startMini, m_settings));

    auto [dualToggle, linkMenu] = createDualToggleAndMenu(startPos, this);
    toggleMenu->addChild(dualToggle);
    m_mainLayer->addChildAtPosition(linkMenu, Anchor::Center, {-220.f, -40.f});

    toggleMenu->addChild(createToggle("Disable", ToggleType::DISABLE, m_settings->m_disableStartPos, m_settings));
    toggleMenu->setLayout(ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
    m_mainLayer->addChildAtPosition(toggleMenu, Anchor::Center, {-180.f, 0.f});


    auto extraSettingsMenu = CCMenu::create();
    extraSettingsMenu->setContentSize({80.f, 80.f});
    auto extraSettingsLabel = CCLabelBMFont::create("Extra", "goldFont.fnt");
    extraSettingsLabel->setScale(.7f);
    extraSettingsMenu->addChildAtPosition(extraSettingsLabel, Anchor::Top, {0.f, -10.f});
    auto extraSettingsBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_optionsBtn_001.png", .75f, [startPos](auto) {
        ExtraPopup::create(startPos)->show();
    });
    extraSettingsMenu->addChildAtPosition(extraSettingsBtn, Anchor::Center);
    m_mainLayer->addChildAtPosition(extraSettingsMenu, Anchor::Center, {180.f, 0.f});

    if(!Mod::get()->getSettingValue<bool>("disable-watermark")) {
        auto watermark = CCLabelBMFont::create("Clean StartPos\nby BlueBlock6", "chatFont.fnt");
        watermark->setAlignment(cocos2d::kCCTextAlignmentRight);
        watermark->setScale(.5f);
        watermark->setOpacity(125);
        watermark->setAnchorPoint({1.f, 0.f});
        m_mainLayer->addChildAtPosition(watermark, Anchor::Center, {220.f, -95.f}, false);
    }

    return true;
}

void SimpleUI::onClose(CCObject* sender) {
    m_startPos->encodeSettings(m_settings);
    Popup::onClose(sender);
}

SimpleUI* SimpleUI::create(AdvancedStartPos* startPos) {
    auto ret = new SimpleUI();
    if(ret && ret->init(startPos)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}
