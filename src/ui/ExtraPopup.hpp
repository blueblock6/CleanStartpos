#pragma once
#include "AdvancedStartPos.hpp"
#include <Geode/Geode.hpp>

class ExtraPopup : public geode::Popup {
private:
    AdvancedStartPos* m_startPos;
    LevelSettingsObjectExt* m_settings;

    geode::TextInput* m_velocityInput;
    geode::TextInput* m_offsetInput;
    geode::TextInput* m_rotationInput;
    geode::TextInput* m_targetOrderInput;
    geode::TextInput* m_targetChannelInput;
    geode::TextInput* m_rotateSpeedInput;
    geode::TextInput* m_rotationSpeedInput;

    bool init(AdvancedStartPos* startPos);
    void onClose(CCObject* sender) override;
public:
    static ExtraPopup* create(AdvancedStartPos* startPos);
};
