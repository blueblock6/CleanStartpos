#include "RotationPopup.hpp"
#include "ui/UIUtil.hpp"

using namespace geode::prelude;

bool RotationPopup::init(AdvancedStartPos* startPos) {
    if(!Popup::init({360.f, 140.f})) return false;

    m_noElasticity = true;

    m_startPos = startPos;
    m_settings = startPos->getSettingsObject();
    bool isP2 = startPos->isP2();

    this->setTitle("Rotation Settings", "goldFont.fnt", .7f, 15.f);
    m_buttonMenu->getChildByType()->setVisible(false);

    auto okBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("OK"),
        this,
        menu_selector(RotationPopup::onClose)
    );
    m_buttonMenu->addChildAtPosition(okBtn, Anchor::Center, {0.f, -40.f});

    auto infoBtn = InfoAlertButton::create(
        "Help",
        "These are more advanced options. It is better to let these get set by the Create StartPos button / keybind instead of setting them manually.\n"
        "<cr>Rotation</c> sets player rotation\n"
        "<cr>Rotation Speed</c> only affects Cube and Ball\n"
        "<cr>Ball Rotation</c> multiplies ball rotation speed (caused by slopes)",
        1.f
    );
    m_buttonMenu->addChildAtPosition(infoBtn, Anchor::TopLeft);

    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_rotationInput, "Rotation", CommonFilter::Float, "Rotation", fmt::format("{:.3f}", startPos->getRotation()).c_str()),
        Anchor::Center, {-120.f, 10.f}, false
    );
    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_rotationSpeedInput, "Rotation Speed", CommonFilter::Float, "Speed", fmt::format("{:.3f}", m_settings->m_fields->rotationSpeed).c_str()),
        Anchor::Center, {0.f, 10.f}, false
    );
    m_mainLayer->addChildAtPosition(
        createInput(CCNode::create(), m_rotateSpeedInput, "Ball Rotation", CommonFilter::Float, "Speed", fmt::format("{:.3f}", m_settings->m_fields->rotateSpeed).c_str()),
        Anchor::Center, {120.f, 10.f}, false
    );

    return true;
}

void RotationPopup::onClose(CCObject* sender) {
    m_startPos->setRotation(utils::numFromString<float>(m_rotationInput->getString()).unwrapOrDefault());
    m_settings->m_fields->rotationSpeed = utils::numFromString<float>(m_rotationSpeedInput->getString()).unwrapOrDefault();
    m_settings->m_fields->rotateSpeed = utils::numFromString<float>(m_rotateSpeedInput->getString()).unwrapOrDefault();
    Popup::onClose(sender);
}

RotationPopup* RotationPopup::create(AdvancedStartPos* startPos) {
    auto ret = new RotationPopup();
    if(ret && ret->init(startPos)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}
