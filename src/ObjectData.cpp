#include "Hooks.hpp"
#include "Settings.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

GameObject* HookedGameObject::createWithKey(int key) {
    if(key == 34) {
        auto obj = modify_cast<HookedStartPosObject*>(GameObject::createWithKey(31));
        obj->m_objectID = 34;
        obj->isP2() = true;
        return obj;
    } return GameObject::createWithKey(key);
}

void HookedStartPosObject::loadExtraSettings() {
    if(Settings::migrateData && !isP2()) {
        if(m_startSettings->m_spawnGroup != 0) {
            m_startSettings->m_yVelocity = std::bit_cast<float>(m_startSettings->m_spawnGroup);
            m_startSettings->m_spawnGroup = 0;
        }

        if(m_isIceBlock) {
            m_startSettings->m_freeMode = true;
            m_isIceBlock = false;
        }
    }

    if(isP2()) {
        m_startSettings->m_startMode = (m_isNonStickX << 2) | (m_isNonStickY << 1) | m_isDontBoostY;
        m_startSettings->m_isFlipped = m_isExtraSticky;
        m_startSettings->m_startMini = m_isScaleStick;
        m_startSettings->m_reverseGameplay = m_isDontBoostX;
        m_startSettings->m_rotateGameplay = m_hasNoAudioScale;
        m_startSettings->m_disableStartPos = m_hasExtendedCollision;
        if(auto other = getOther()) {
            m_startSettings->m_yVelocity = std::bit_cast<float>(other->m_startSettings->m_propertykA44);
        }
    }
}

void HookedStartPosObject::saveExtraSettings() {
    if(isP2()) {
        m_isNonStickX = (m_startSettings->m_startMode & 0b100) >> 2;
        m_isNonStickY = (m_startSettings->m_startMode & 0b010) >> 1;
        m_isDontBoostY = (m_startSettings->m_startMode & 0b001);
        m_isExtraSticky = m_startSettings->m_isFlipped;
        m_isScaleStick = m_startSettings->m_startMini;
        m_isDontBoostX = m_startSettings->m_reverseGameplay;
        m_hasNoAudioScale = m_startSettings->m_rotateGameplay;
        m_hasExtendedCollision = m_startSettings->m_disableStartPos;
        if(auto other = getOther()) {
            other->m_startSettings->m_propertykA44 = std::bit_cast<int>(m_startSettings->m_yVelocity);
        }
    }
}

void HookedStartPosObject::mergeP1Settings() {
    m_startSettings->m_platformerMode = GJBaseGameLayer::get()->m_isPlatformer;

    if(!isP2()) return;
    auto p1 = getOther();
    if(p1 == nullptr) return;

    m_startSettings->m_startSpeed = p1->m_startSettings->m_startSpeed;
    m_startSettings->m_startDual = p1->m_startSettings->m_startDual;
    m_startSettings->m_mirrorMode = p1->m_startSettings->m_mirrorMode;
    m_startSettings->m_freeMode = p1->m_startSettings->m_freeMode;
    m_startSettings->m_resetCamera = p1->m_startSettings->m_resetCamera;
    m_startSettings->m_online = p1->m_startSettings->m_online;
    m_startSettings->m_player2SpawnGroup = p1->m_startSettings->m_player2SpawnGroup;
}
