#pragma once
#include "AdvancedStartPos.hpp"
#include <Geode/Geode.hpp>

class RotationPopup : public geode::Popup {
private:
    AdvancedStartPos* m_startPos;
    LevelSettingsObjectExt* m_settings;

    geode::TextInput* m_rotationInput;
    geode::TextInput* m_rotationSpeedInput;
    geode::TextInput* m_rotateSpeedInput;

    bool init(AdvancedStartPos* startPos);
    virtual void onClose(CCObject* sender) override;
public:
    static RotationPopup* create(AdvancedStartPos* startPos);
};
