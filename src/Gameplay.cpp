#include "Hooks.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

constexpr bool isLargeDual(int mode) {
    switch(mode) {
    case 0: return false;
    case 1: return true;
    case 2: return false;
    case 3: return true;
    case 4: return true;
    case 5: return false;
    case 6: return false;
    case 7: return true;
    }
    return false;
}

constexpr GameObjectType objectTypeForMode(int mode) {
    switch(mode) {
    case 0: return GameObjectType::CubePortal;
    case 1: return GameObjectType::ShipPortal;
    case 2: return GameObjectType::BallPortal;
    case 3: return GameObjectType::UfoPortal;
    case 4: return GameObjectType::WavePortal;
    case 5: return GameObjectType::RobotPortal;
    case 6: return GameObjectType::SpiderPortal;
    case 7: return GameObjectType::SwingPortal;
    }
    return GameObjectType::CubePortal;
}

constexpr void applyRotation(PlayerObject* player, HookedStartPosObject* startPos, bool inverse) {
    float inverseMultiplier = inverse ? -1 : 1;
    player->setRotation(startPos->getRotation() * inverseMultiplier);
    if(startPos->m_startSettings->m_startMode == 0) {
        player->m_rotationSpeed = startPos->rotationSpeed() * inverseMultiplier;
        player->m_isOnGround2 = false;
    } else {
        player->m_rotationSpeed = startPos->rotationSpeed() * inverseMultiplier;
        player->m_rotateSpeed = startPos->rotateSpeed() * inverseMultiplier;
        player->m_isBallRotating = true;
    }
}

constexpr void setMode(PlayerObject* player, int mode) {
    switch(mode) {
        case 0:
            player->toggleFlyMode(false, false);
            player->toggleRollMode(false, false);
            player->toggleBirdMode(false, false);
            player->toggleDartMode(false, false);
            player->toggleRobotMode(false, false);
            player->toggleSpiderMode(false, false);
            player->toggleSwingMode(false, false);
            break;
        case 1: player->toggleFlyMode(true, true); break;
        case 2: player->toggleRollMode(true, true); break;
        case 3: player->toggleBirdMode(true, true); break;
        case 4: player->toggleDartMode(true, true); break;
        case 5: player->toggleRobotMode(true, true); break;
        case 6: player->toggleSpiderMode(true, true); break;
        case 7: player->toggleSwingMode(true, true); break;
    }
}

void HookedGJBaseGameLayer::loadStartPosObject(StartPosObject* startPos) {
    auto p1 = modify_cast<HookedStartPosObject*>(startPos);
    p1->loadExtraSettings();

    auto p2 = p1->getOther();
    if(p2 != nullptr) {
        p2->loadExtraSettings();
        if(p2->m_startSettings->m_disableStartPos) {
            p2 = nullptr;
        }
    }

    GJBaseGameLayer::loadStartPosObject(startPos);

    if(m_player2 && !p1->m_startSettings->m_freeMode) {
        auto offset = GameObject::createWithKey(286); // dual portal
        offset->m_startPosition = p1->m_startPosition;
        offset->m_startPosition.y += p1->cameraOffset() * 30;
        m_gameState.m_lastActivatedPortal1 = offset;
        m_gameState.m_lastActivatedPortal2 = offset;
        p1->m_fields->m_offsetObject.swap(offset);

        GameObjectType mode;
        if(p2 != nullptr) {
            if(isLargeDual(p1->m_startSettings->m_startMode) || isLargeDual(p2->m_startSettings->m_startMode)) {
                mode = GameObjectType::ShipPortal;
            } else {
                mode = GameObjectType::CubePortal;
            }
        } else {
            mode = objectTypeForMode(p1->m_startSettings->m_startMode);
        }

        this->updateDualGround(m_player1, static_cast<int>(mode), true, 0.f);
    }

    applyRotation(m_player1, p1, false);

    if(!p2) {
        if(m_player2) {
            applyRotation(m_player2, p1, true);
        }
        return;
    }

    m_player2->setPosition(p2->getPosition());
    m_player2->m_position = p2->getPosition();
    setMode(m_player2, p2->m_startSettings->m_startMode);
    m_player2->flipGravity(p2->m_startSettings->m_isFlipped, true);
    m_player2->togglePlayerScale(p2->m_startSettings->m_startMini, true);
    m_player2->doReversePlayer(p2->m_startSettings->m_reverseGameplay);
    m_player2->rotateGameplayOnly(p2->m_startSettings->m_rotateGameplay);
    m_player2->setYVelocity(p2->m_startSettings->m_yVelocity, 0);
    applyRotation(m_player2, p2, false);
}
