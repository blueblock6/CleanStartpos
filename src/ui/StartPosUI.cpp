#include "StartPosUI.hpp"
#include "SimpleUI.hpp"
#include "AdvancedUI.hpp"
#include "SelectorUI.hpp"

using namespace geode::prelude;

void showStartPosUI(AdvancedStartPos* startPos) {
    if(!Mod::get()->setSavedValue<bool>("shown-picker", true)) {
        SelectorUI::create(startPos)->show();
        return;
    }

    auto mode = Mod::get()->getSettingValue<std::string_view>("menu-mode");
    if(mode == "Simple") {
        SimpleUI::create(startPos)->show();
    } else {
        AdvancedUI::create(startPos)->show();
    }
}
