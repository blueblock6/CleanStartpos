#include "SelectorUI.hpp"
#include "SimpleUI.hpp"
#include "AdvancedUI.hpp"

using namespace geode::prelude;

bool SelectorUI::init(AdvancedStartPos* startPos) {
    if(!CCLayerColor::initWithColor({0, 0, 0})) return false;

    this->ignoreAnchorPointForPosition(false);
    this->setAnchorPoint({0.f, 0.f});
    this->setOpacity(105);

    m_startPos = startPos;

    auto bg = CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize({280.f, 50.f});
    this->addChildAtPosition(bg, Anchor::Top, {0.f, -30.f});

    auto label = CCLabelBMFont::create("Select UI Design", "bigFont.fnt");
    label->setScale(.8f);
    this->addChildAtPosition(label, Anchor::Top, {0.f, -20.f});

    auto subLabel = CCLabelBMFont::create("This can be changed in mod settings", "bigFont.fnt");
    subLabel->setScale(.4f);
    this->addChildAtPosition(subLabel, Anchor::Top, {0.f, -40.f});

    auto clarifier = CCLabelBMFont::create("Tap on the UI you prefer", "chatFont.fnt");
    clarifier->setScale(.4f);
    this->addChildAtPosition(clarifier, Anchor::Top, {0.f, -60.f});

    auto simple = SimpleUI::create(startPos);
    simple->setScale(.5f);
    simple->setOpacity(0);
    simple->setAnchorPoint({0.f, .5f});
    this->addChildAtPosition(simple, Anchor::Left);

    auto simpleLabel = CCLabelBMFont::create("Simple", "goldFont.fnt");
    simple->m_mainLayer->addChildAtPosition(simpleLabel, Anchor::Top, {0.f, 20.f});

    auto advanced = AdvancedUI::create(startPos);
    advanced->setScale(.5f);
    advanced->setOpacity(0);
    advanced->setAnchorPoint({1.f, .5f});
    this->addChildAtPosition(advanced, Anchor::Right);

    auto advancedLabel = CCLabelBMFont::create("Advanced", "goldFont.fnt");
    advanced->m_mainLayer->addChildAtPosition(advancedLabel, Anchor::Top, {0.f, 20.f});

    this->setKeypadEnabled(true);
    this->setTouchMode(cocos2d::kCCTouchesOneByOne);
    this->setTouchEnabled(true);

    geode::queueInMainThread([this](){
        CCTouchDispatcher::get()->setPriority(-999, this);
    });

    return true;
}

void SelectorUI::onClose(CCObject*) {
    this->removeFromParentAndCleanup(true);
}

void SelectorUI::keyBackClicked() {
    onClose(nullptr);
}

bool SelectorUI::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent) {
    float width = CCDirector::get()->getWinSize().width;

    if(pTouch->m_point.x <= width / 2.f) {
        Mod::get()->setSettingValue<std::string_view>("menu-mode", "Simple");
        onClose(nullptr);
        geode::queueInMainThread([this](){ // prevents touch prio issues
            showStartPosUI(m_startPos);
        });
    } else {
        Mod::get()->setSettingValue<std::string_view>("menu-mode", "Advanced");
        onClose(nullptr);
        geode::queueInMainThread([this](){ // prevents touch prio issues
            showStartPosUI(m_startPos);
        });
    }

    return true;
}

SelectorUI* SelectorUI::create(AdvancedStartPos* startPos) {
    auto ret = new SelectorUI();
    if(ret && ret->init(startPos)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}

void SelectorUI::show() {
    auto scene = CCScene::get();
    scene->addChild(this, scene->getHighestChildZ() + 1);
}
