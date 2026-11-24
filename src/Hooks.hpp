#pragma once

#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/StartPosObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/StartPosLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/DrawGridLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CreateStartPosPopup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/GJSpriteColor.hpp>

class $modify(HookedGameObject, GameObject) {
    $override static GameObject* createWithKey(int key);
    $override void customSetup();
};

class $modify(HookedStartPosObject, StartPosObject) {
    struct Fields {
        bool m_isP2 = false;
        HookedStartPosObject* m_other = nullptr;
        geode::Ref<GameObject> m_offsetObject = nullptr;
    };

    constexpr bool& isP2() {
        return m_fields->m_isP2;
    }

    constexpr HookedStartPosObject*& getOther() {
        return m_fields->m_other;
    }

    constexpr short& linkId() {
        return m_objectMaterial;
    }

    constexpr int& cameraOffset() {
        return m_controlID;
    }

    constexpr float& rotationSpeed() {
        return m_baseColor->m_hsv.h;
    }

    constexpr float& rotateSpeed() {
        return m_baseColor->m_hsv.v;
    }

    void loadExtraSettings();
    void saveExtraSettings();
    void mergeP1Settings();
};

class $modify(HookedGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        std::map<short, std::pair<HookedStartPosObject*, HookedStartPosObject*>> m_links = {};
    };

    void addLink(HookedStartPosObject* startPos);
    std::pair<HookedStartPosObject*, HookedStartPosObject*>& getLink(short id);

    $override void loadStartPosObject(StartPosObject* startPos);
};

class $modify(HookedStartPosLayer, StartPosLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* m_linkButton;
        CCMenuItemSpriteExtra* m_unlinkButton;
        geode::TextInput* m_offsetInput;
    };

    void onLink(CCObject*);
    void onUnlink(CCObject*);

    $override bool init(StartPosObject* startPosObject);
    $override float getValue(int tag);
    $override void valueDidChange(int tag, float value);
    $override void onClose(CCObject* sender);
};

class $modify(HookedEditorUI, EditorUI) {
    struct Fields {
        CCMenuItemSpriteExtra* m_createStartPosButton = nullptr;
        bool m_paused = true;
        float m_offset = 0.f;
    };

    $override void selectObject(GameObject* object, bool ignoreFilter);
    $override void editObject(CCObject* sender);
    $override bool init(LevelEditorLayer* editorLayer);
    $override void onPlaytest(CCObject* sender);

    void onCreateStartPos(CCObject* sender);
};

class $modify(HookedDrawGridLayer, DrawGridLayer) {
    $override void draw();
};

class $modify(HookedLevelEditorLayer, LevelEditorLayer) {
    $override cocos2d::CCArray* createObjectsFromString(const std::string& str, bool noUndo, bool ignoreLimit);
    $override void addSpecial(GameObject* object);
    $override void removeSpecial(GameObject* object);
    $override void onPlaytest();
    $override void onStopPlaytest();
};

class $modify(HookedCreateStartPosPopup, CreateStartPosPopup) {
    $override bool init();
    $override void onCreate(CCObject* sender);
};

constexpr HookedGJBaseGameLayer* getBGL() {
    return geode::cast::modify_cast<HookedGJBaseGameLayer*>(GJBaseGameLayer::get());
}
