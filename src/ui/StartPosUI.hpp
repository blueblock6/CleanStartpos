#pragma once
#include "AdvancedStartPos.hpp"
#include <Geode/Geode.hpp>

class StartPosUI : public geode::Popup {
public:
    AdvancedStartPos* m_startPos;
    LevelSettingsObjectExt* m_settings;
    bool m_isP2;

    inline virtual void onClose(CCObject* sender) override {
        geode::Popup::onClose(sender);
    }
};

void showStartPosUI(AdvancedStartPos* startPos);
