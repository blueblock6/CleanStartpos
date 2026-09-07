#pragma once
#include <Geode/Geode.hpp>
#include "AdvancedStartPos.hpp"
#include "StartPosUI.hpp"

enum ToggleType {
    FLIP,
    MINI,
    DUAL,
    MIRROR,
    FREE,
    ROTATE,
    REVERSE,
    RESET,
    DISABLE,
};

cocos2d::CCMenu* createModeMenu(LevelSettingsObjectExt* settings);
cocos2d::CCMenu* createSpeedMenu(AdvancedStartPos* startPos);
cocos2d::CCMenu* createToggle(const char* name, ToggleType type, bool toggled, LevelSettingsObjectExt* settings, bool enabled = true);
cocos2d::CCNode* createInput(cocos2d::CCNode* base, geode::TextInput*& input, const char* title, geode::CommonFilter filter, const char* placeholder, const std::string& value, bool enabled = true);
std::pair<cocos2d::CCMenu*, cocos2d::CCMenu*> createDualToggleAndMenu(AdvancedStartPos* startPos, StartPosUI* ui);
