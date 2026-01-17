#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSettingsLayer.hpp>
#include <AdvancedStartPos.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Links.hpp>

using namespace geode::prelude;

class $modify(CustomUI, LevelSettingsLayer) {
    struct Fields {
        AdvancedStartPos* object;
        LevelSettingsObjectExt* settings;
        CCMenu* modeMenu;
        CCMenu* speedMenu;
        TextInput* targetOrderInput;
        TextInput* targetChannelInput;
        TextInput* velocityInput;
        TextInput* offsetInput;
        CCMenuItemSpriteExtra* linkBtn;
        CCMenuItemSpriteExtra* unlinkBtn;
    };

    $override
    bool init(LevelSettingsObject* settings, LevelEditorLayer* lel) {
        if(!LevelSettingsLayer::init(settings, lel)) return false;
        if(!settings->m_startsWithStartPos) return true;

        m_fields->object = static_cast<AdvancedStartPos*>(settings->getUserObject());
        m_fields->settings = m_fields->object->getSettingsObject();
        auto realStartPos = m_fields->object->getRealStartPos();

        for(auto child : CCArrayExt<CCNode*>(m_mainLayer->getChildren())) child->setVisible(false);
        for(auto child : CCArrayExt<CCNode*>(m_buttonMenu->getChildren())) child->setVisible(false);
        m_buttonMenu->setVisible(true);

        auto bg = m_mainLayer->getChildByType(0);
        bg->setContentSize({455.f, 250.f});
        bg->setVisible(true);

        auto okBtn = m_buttonMenu->getChildByType(0);
        okBtn->setPositionY(-100.f);
        okBtn->setVisible(true);

        auto leftMenu = createLeftMenu();
        m_mainLayer->addChildAtPosition(leftMenu, Anchor::Center, {-180.f, 0.f}, false);
        auto rightMenu = createRightMenu();
        m_mainLayer->addChildAtPosition(rightMenu, Anchor::Center, {180.f, 0.f}, false);

        m_fields->modeMenu = createModeMenu();
        m_mainLayer->addChildAtPosition(m_fields->modeMenu, Anchor::Center, {0.f, 90.f}, false);
        m_fields->speedMenu = createSpeedMenu();
        m_mainLayer->addChildAtPosition(m_fields->speedMenu, Anchor::Center, {-5.f, 40.f}, false);

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << m_fields->settings->m_fields->yVelocity;

        m_mainLayer->addChildAtPosition(
            createInput(CCNode::create(), m_fields->velocityInput, "Velocity", "velocity", CommonFilter::Float, "Velocity", oss.str()),
            Anchor::Center, {-80.f, -25.f}, false
        );
        m_mainLayer->addChildAtPosition(
            createInput(CCNode::create(), m_fields->offsetInput, "Offset", "offset", CommonFilter::Int, "Offset", std::to_string(m_fields->settings->m_fields->cameraOffset), realStartPos),
            Anchor::Center, {80.f, -25.f}, false
        );
        m_mainLayer->addChildAtPosition(
            createInput(CCNode::create(), m_fields->targetOrderInput, "Target Order", "target-order", CommonFilter::Int, "Order", std::to_string(m_fields->settings->m_targetOrder), realStartPos),
            Anchor::Center, {-80.f, -89.f}, false
        );
        m_mainLayer->addChildAtPosition(
            createInput(CCNode::create(), m_fields->targetChannelInput, "Target Channel", "target-channel", CommonFilter::Int, "Channel", std::to_string(m_fields->settings->m_targetChannel), realStartPos),
            Anchor::Center, {80.f, -89.f}, false
        );

        auto linkMenu = CCMenu::create();
        linkMenu->setID("link-menu"_spr);
        linkMenu->setContentSize({80.f, 30.f});
        m_mainLayer->addChildAtPosition(linkMenu, Anchor::Center, {-180.f - 40.f, -20.f}, false);

        m_fields->linkBtn = CCMenuItemExt::createSpriteExtraWithFrameName("gj_linkBtn_001.png", .8f, [this](auto){ onLink(); });
        m_fields->linkBtn->setID("link-button");
        m_fields->linkBtn->setVisible(
            m_fields->settings->m_startDual ||
            !realStartPos ||
            m_fields->object->m_linkId != 0
        );
        linkMenu->addChildAtPosition(m_fields->linkBtn, Anchor::Left, {15.f, 0.f}, false);

        m_fields->unlinkBtn = CCMenuItemExt::createSpriteExtraWithFrameName("gj_linkBtnOff_001.png", .8f, [this](auto){ onUnlink(); });
        m_fields->unlinkBtn->setID("unlink-button");
        m_fields->unlinkBtn->setVisible(
            !realStartPos ||
            (m_fields->object->m_linkId != 0 && Links::getSecondary(m_fields->object->m_linkId))
        );
        linkMenu->addChildAtPosition(m_fields->unlinkBtn, Anchor::Right, {-15.f, 0.f}, false);

        handleTouchPriority(this);

        return true;
    }

    inline CCMenu* createLeftMenu() {
        auto menu = CCMenu::create();
        menu->setLayout(ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
        menu->setContentSize({100, 225});

        bool isReal = m_fields->object->getRealStartPos();
        menu->setID("left-menu"_spr);
        menu->addChild(createToggle("Flip", "flip", 0, m_fields->settings->m_isFlipped));
        menu->addChild(createToggle("Mini", "mini", 1, m_fields->settings->m_startMini));
        menu->addChild(createToggle("Dual", "dual", 2, m_fields->settings->m_startDual, isReal));
        menu->addChild(createToggle("Mirror", "mirror", 3, m_fields->settings->m_mirrorMode, isReal));
        menu->addChild(createToggle("Free Mode", "free-mode", 4, m_fields->settings->m_fields->isFreeCam, isReal));

        menu->updateLayout();
        return menu;
    }

    inline CCMenu* createRightMenu() {
        auto menu = CCMenu::create();
        menu->setLayout(ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
        menu->setContentSize({100, 225});

        bool isReal = m_fields->object->getRealStartPos();
        menu->setID("right-menu"_spr);
        menu->addChild(createToggle("Rotate", "rotate", 5, m_fields->settings->m_rotateGameplay, isReal));
        menu->addChild(createToggle("Reverse", "reverse", 6, m_fields->settings->m_reverseGameplay));
        menu->addChild(createToggle("Reset Camera", "reset-camera", 7, m_fields->settings->m_resetCamera, isReal));
        menu->addChild(createToggle("Disable", "disable", 8, m_fields->settings->m_disableStartPos));

        auto btn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_optionsBtn02_001.png", 1.f, [](auto){ openSettingsPopup(Mod::get(), false); });
        btn->setID("mod-settings"_spr);
        menu->addChild(btn);

        menu->updateLayout();
        return menu;
    }

    inline CCMenu* createModeMenu() {
        auto menu = CCMenu::create();
        menu->setLayout(RowLayout::create());
        menu->setContentSize({300, 50});
        menu->setID("mode-menu"_spr);
        menu->addChild(createModeButton("cube", "gj_iconBtn_off_001.png", 0));
        menu->addChild(createModeButton("ship", "gj_shipBtn_off_001.png", 1));
        menu->addChild(createModeButton("ball", "gj_ballBtn_off_001.png", 2));
        menu->addChild(createModeButton("ufo", "gj_birdBtn_off_001.png", 3));
        menu->addChild(createModeButton("wave", "gj_dartBtn_off_001.png", 4));
        menu->addChild(createModeButton("robot", "gj_robotBtn_off_001.png", 5));
        menu->addChild(createModeButton("spider", "gj_spiderBtn_off_001.png", 6));
        menu->addChild(createModeButton("swing", "gj_swingBtn_off_001.png", 7));
        menu->updateLayout();
        return menu;
    }

    inline CCMenu* createSpeedMenu() {
        auto menu = CCMenu::create();
        menu->setLayout(RowLayout::create());
        menu->setContentSize({300, 50});
        menu->setID("speed-menu"_spr);
        menu->addChild(createSpeedButton("slow", "boost_01_001.png", 1));
        menu->addChild(createSpeedButton("normal", "boost_02_001.png", 0));
        menu->addChild(createSpeedButton("double", "boost_03_001.png", 2));
        menu->addChild(createSpeedButton("triple", "boost_04_001.png", 3));
        menu->addChild(createSpeedButton("quadruple", "boost_05_001.png", 4));
        menu->updateLayout();
        return menu;
    }

    inline CCNode* createInput(CCNode* base, TextInput*& input, const char* title, const std::string& id, CommonFilter filter, const char* placeholder, const std::string& value, bool enabled = true) {
        base->setID(id + "-node");
        base->setContentSize({120, 50});
        base->setAnchorPoint({.5f, .5f});

        auto label = CCLabelBMFont::create(title, "goldFont.fnt");
        label->setID(id + "-label");
        label->setScale(.6f);
        base->addChildAtPosition(label, Anchor::Center, {0, 17});

        input = TextInput::create(80, placeholder);
        input->setID(id + "-input");
        input->setEnabled(enabled);
        input->getInputNode()->setVisible(enabled);
        input->setCommonFilter(filter);
        if(value != "0" && value != "0.000") {
            input->setString(value);
        }
        base->addChildAtPosition(input, Anchor::Center, {0, -11});

        return base;
    }

    inline CCMenu* createToggle(const char* name, const std::string& id, int tag, bool toggle, bool enabled = true) {
        auto menu = CCMenu::create();
        menu->setContentSize({80, 40});

        auto label = CCLabelBMFont::create(name, "goldFont.fnt");
        label->limitLabelWidth(80.f, .7f, .1f);
        label->setID(id + "-label");
        menu->addChildAtPosition(label, Anchor::Center, {0, 15});

        CCMenuItemToggler* btn;
        if(enabled) {
            btn = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(CustomUI::onToggle), .8f);
        } else {
            auto checkOff = CCSpriteGrayscale::createWithSpriteFrameName("GJ_checkOff_001.png");
            checkOff->setScale(.8f);
            checkOff->setColor({127, 127, 127});
            auto checkOn = CCSpriteGrayscale::createWithSpriteFrameName("GJ_checkOn_001.png");
            checkOn->setScale(.8f);
            checkOn->setColor({127, 127, 127});
            btn = CCMenuItemToggler::create(checkOff, checkOn, this, menu_selector(CustomUI::onToggle));
            btn->setEnabled(false);
        }
        btn->setID(id);
        btn->setTag(tag);
        btn->toggle(toggle);
        menu->addChildAtPosition(btn, Anchor::Center, {0, -7});

        return menu;
    }

    inline CCMenuItemSpriteExtra* createModeButton(const std::string& id, const char* sprite, int tag) {
        auto btn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName(sprite), this, menu_selector(CustomUI::onCMode));
        btn->setID(id);
        btn->setTag(tag);
        if(tag != m_fields->settings->m_startMode) {
            btn->setColor({127, 127, 127});
        }
        return btn;
    }

    inline CCMenuItemSpriteExtra* createSpeedButton(const std::string& id, const char* sprite, int tag) {
        bool isReal = m_fields->object->getRealStartPos();
        auto spr = isReal ? CCSprite::createWithSpriteFrameName(sprite) : CCSpriteGrayscale::createWithSpriteFrameName(sprite);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(CustomUI::onCSpeed));
        btn->setEnabled(isReal);
        btn->setID(id);
        btn->setTag(tag);
        if(tag != static_cast<int>(m_fields->settings->m_startSpeed)) {
            btn->setColor(isReal ? ccColor3B{127, 127, 127} : ccColor3B{63, 63, 63});
        }
        return btn;
    }

    void onToggle(CCObject* sender) {
        auto toggle = static_cast<CCMenuItemToggler*>(sender);
        auto state = !toggle->isToggled();
        switch(toggle->getTag()) {
            case 0: m_fields->settings->m_isFlipped = state; break;
            case 1: m_fields->settings->m_startMini = state; break;
            case 2: {
                m_fields->settings->m_startDual = state;
                m_fields->linkBtn->setVisible(
                    state ||
                    !m_fields->object->getRealStartPos()||
                    m_fields->object->m_linkId != 0
                );
            } break;
            case 3: m_fields->settings->m_mirrorMode = state; break;
            case 4: m_fields->settings->m_fields->isFreeCam = state; break;
            case 5: m_fields->settings->m_rotateGameplay = state; break;
            case 6: m_fields->settings->m_reverseGameplay = state; break;
            case 7: m_fields->settings->m_resetCamera = state; break;
            case 8: m_fields->settings->m_disableStartPos = state; break;
        }
    }

    void onCMode(CCObject* sender) {
        static_cast<CCMenuItemSpriteExtra*>(m_fields->modeMenu->getChildByTag(m_fields->settings->m_startMode))->setColor({127, 127, 127});
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        btn->setColor({255, 255, 255});
        m_fields->settings->m_startMode = btn->getTag();
    }

    void onCSpeed(CCObject* sender) {
        static_cast<CCMenuItemSpriteExtra*>(m_fields->speedMenu->getChildByTag(static_cast<int>(m_fields->settings->m_startSpeed)))->setColor({127, 127, 127});
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        btn->setColor({255, 255, 255});
        m_fields->settings->m_startSpeed = static_cast<Speed>(btn->getTag());
    }

    void onLink() {
        auto realStartPos = m_fields->object->getRealStartPos();
        int id = m_fields->object->m_linkId;

        if(!realStartPos) {
            auto primary = Links::getPrimary(id);
            if(!primary) return;
            onClose(this);
            auto eu = EditorUI::get();
            eu->selectObject(primary, true);
            eu->updateButtons();
            eu->editObject(this);
            return;
        }

        if(auto secondary = Links::getSecondary(id)) {
            onClose(this);
            auto eu = EditorUI::get();
            eu->selectObject(secondary, true);
            eu->updateButtons();
            eu->editObject(this);
            return;
        }

        if(id == 0) id = Links::nextId();
        m_fields->object->m_linkId = id;
        // LevelEditorLayer::createObject + m_isNoTouch = true doesn't remove collision until editor is reloaded \_(._.)_/
        auto obj = static_cast<AdvancedStartPos*>(
            LevelEditorLayer::get()->createObjectsFromString(fmt::format("1,34,2,{},3,{},121,1", realStartPos->m_positionX, realStartPos->m_positionY - 30).c_str(), false, false)
            ->firstObject()
        );
        obj->m_linkId = id;
        obj->m_isNoTouch = true;
        obj->encodeSettings(m_fields->settings);
        Links::insertPair(m_fields->object, obj, id);

        onClose(this);
        auto eu = EditorUI::get();
        eu->selectObject(obj, true);
        eu->updateButtons();
    }

    void onUnlink() {
        m_fields->unlinkBtn->setVisible(false);
        int id = m_fields->object->m_linkId;
        bool isReal = m_fields->object->getRealStartPos();
        auto other = Links::getSecondary(id);
        Links::destroy(id);
        auto lel = LevelEditorLayer::get();
        if(isReal) {
            if(other) lel->removeObject(other, true);
        } else {
            onClose(this);
            lel->removeObject(m_fields->object, true);
            lel->m_editorUI->deselectAll();
        }
    }

    $override
    void onClose(CCObject* sender) {
        if(!m_settingsObject->m_startsWithStartPos) return LevelSettingsLayer::onClose(sender);

        m_fields->settings->m_targetOrder = m_fields->targetOrderInput ? utils::numFromString<int>(m_fields->targetOrderInput->getString()).unwrapOrDefault() : 0;
        m_fields->settings->m_targetChannel = m_fields->targetChannelInput ? utils::numFromString<int>(m_fields->targetChannelInput->getString()).unwrapOrDefault() : 0;
        m_fields->settings->m_fields->cameraOffset = m_fields->offsetInput ? utils::numFromString<int>(m_fields->offsetInput->getString()).unwrapOrDefault() : 0;
        m_fields->settings->m_fields->yVelocity = m_fields->velocityInput ? utils::numFromString<float>(m_fields->velocityInput->getString()).unwrapOrDefault() : 0;
        m_fields->object->encodeSettings(m_fields->settings);

        LevelSettingsLayer::onClose(sender);
    }
};
