#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSettingsObject.hpp>

class $modify(LevelSettingsObjectExt, LevelSettingsObject) {
    struct Fields {
        bool isFreeCam = false;
        int cameraOffset = 0;
        float yVelocity = 0;
        float rotationSpeed = 0;
        float rotateSpeed = 0;
    };
};
