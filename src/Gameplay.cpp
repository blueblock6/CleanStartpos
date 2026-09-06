#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <LevelSettingsObjectExt.hpp>
#include <Links.hpp>

using namespace geode::prelude;

class $modify(GJBaseGameLayer) {
    struct Fields {
        Ref<GameObject> offsetObj;
    };

    $override
    bool init() {
        Links::links.clear();
        for(auto sp : secondaryStartPosSettings) sp.second->release();
        secondaryStartPosSettings.clear();
        return GJBaseGameLayer::init();
    }

    $override
    void loadStartPosObject() {
        auto settings = m_startPosObject ? static_cast<AdvancedStartPos*>(static_cast<GameObject*>(m_startPosObject))->getSettingsObject() : nullptr;
        auto other = (settings && settings->m_startDual) ? Links::getSecondary(m_startPosObject->m_linkId) : nullptr;
        auto p2Settings = other ? other->getSettingsObject() : nullptr;

        GJBaseGameLayer::loadStartPosObject();
        if(!settings) return;

        if(PlayLayer::get()) GJBaseGameLayer::toggleFlipped(settings->m_mirrorMode, true);

        if(settings->m_fields->isFreeCam) {
            m_gameState.m_isFreeMode = true;
            updateDualGround(m_player1, 0, false, 0.f);
            m_gameState.m_cameraPosition.y += settings->m_fields->cameraOffset * 30;
        } else {
            if(!settings->m_startDual && (settings->m_startMode == 0 || settings->m_startMode == 5)) {
                m_gameState.m_cameraPosition.y += settings->m_fields->cameraOffset * 30;
            } else {
                auto offset = GameObject::createWithKey(286); // dual portal - id doesn't seem to matter though
                auto pos = m_startPosObject->m_startPosition;
                pos.y += settings->m_fields->cameraOffset * 30;
                offset->m_startPosition = pos;
                m_gameState.m_lastActivatedPortal1 = offset;
                m_gameState.m_lastActivatedPortal2 = offset;
                m_fields->offsetObj.swap(offset);

                GameObjectType mode;
                if(other) {
                    if(largeDual(settings->m_startMode) || largeDual(p2Settings->m_startMode)) {
                        mode = GameObjectType::ShipPortal;
                    } else {
                        mode = GameObjectType::CubePortal;
                    }
                } else switch(settings->m_startMode) {
                    case 0: mode = GameObjectType::CubePortal; break;
                    case 1: mode = GameObjectType::ShipPortal; break;
                    case 2: mode = GameObjectType::BallPortal; break;
                    case 3: mode = GameObjectType::UfoPortal; break;
                    case 4: mode = GameObjectType::WavePortal; break;
                    case 5: mode = GameObjectType::RobotPortal; break;
                    case 6: mode = GameObjectType::SpiderPortal; break;
                    case 7: mode = GameObjectType::SwingPortal; break;
                }

                this->updateDualGround(m_player1, static_cast<int>(mode), true, 0.f);
            }
        }

        m_player1->setYVelocity(settings->m_fields->yVelocity, 0);
        applyRotation(m_player1, m_startPosObject, settings);

        if(!p2Settings || p2Settings->m_disableStartPos) {
            if(m_player2) {
                m_player2->setYVelocity(-settings->m_fields->yVelocity, 0);
                applyRotation(m_player2, m_startPosObject, settings);
            }
            return;
        }

        if(p2Settings->m_startMode != settings->m_startMode) {
            setMode(m_player2, p2Settings->m_startMode);
        }

        m_player2->setPosition(other->getPosition());
        m_player2->m_position = other->getPosition();
        m_player2->setYVelocity(p2Settings->m_fields->yVelocity, 0);
        m_player2->flipGravity(p2Settings->m_isFlipped, true);
        m_player2->togglePlayerScale(p2Settings->m_startMini, true);
        m_player2->doReversePlayer(p2Settings->m_reverseGameplay);
        applyRotation(m_player2, other, p2Settings);
    }

    inline bool largeDual(int mode) {
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

    inline void setMode(PlayerObject* player, int mode) {
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

    inline void applyRotation(PlayerObject* player, GameObject* object, LevelSettingsObjectExt* settings) {
        player->setRotation(object->getRotation());
        if(settings->m_startMode == 0) {
            player->m_rotationSpeed = settings->m_fields->rotationSpeed;
            player->m_isOnGround2 = false;
        } else if(settings->m_startMode == 2) {
            player->m_rotationSpeed = settings->m_fields->rotationSpeed;
            player->m_rotateSpeed = settings->m_fields->rotateSpeed;
            player->m_isBallRotating = true;
        }
    }
};
