#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>
#include <LevelSettingsObjectExt.hpp>

#define m_linkId m_objectMaterial

static std::map<GameObject*, LevelSettingsObjectExt*> secondaryStartPosSettings;

class $modify(AdvancedStartPos, GameObject) {
    StartPosObject* getRealStartPos();
    LevelSettingsObjectExt* getSettingsObject();
    void encodeSettings(LevelSettingsObjectExt* settings);
    void setYVelocity(float value);
    float getYVelocity();

    $override void customSetup();
};
