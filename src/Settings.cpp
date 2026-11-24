#include "Settings.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

template <typename T>
constexpr void handleSetting(const std::string& name, T& var) {
    var = Mod::get()->getSettingValue<T>(name);
    listenForSettingChanges<T>(name, [&var](T value) {
        var = value;
    });
}

ccColor3B Settings::p1Color;
ccColor3B Settings::p2Color;
ccColor3B Settings::lineColor;
bool Settings::autoOnline;
bool Settings::hideStartPosButton;
bool Settings::migrateData;

$on_mod(DataLoaded) {
    handleSetting("migrate-data", Settings::migrateData);
    handleSetting("p1-startpos-color", Settings::p1Color);
    handleSetting("p2-startpos-color", Settings::p2Color);
    handleSetting("link-line-color", Settings::lineColor);
    handleSetting("auto-online", Settings::autoOnline);
    handleSetting("hide-startpos-button", Settings::hideStartPosButton);
}
