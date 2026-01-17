#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <LevelSettingsObjectExt.hpp>
#include <Links.hpp>

using namespace geode::prelude;

class $modify(GJBaseGameLayer) {
    struct Fields {
        TeleportPortalObject* tpObj = nullptr;

        ~Fields() {
            Links::links.clear();
            for(auto sp : secondaryStartPosSettings) sp.second->release();
            secondaryStartPosSettings.clear();
        }
    };

    $override
    void loadStartPosObject() {
        auto settings = m_startPosObject ? static_cast<AdvancedStartPos*>(static_cast<GameObject*>(m_startPosObject))->getSettingsObject() : nullptr;
        auto other = (settings && settings->m_startDual) ? Links::getSecondary(m_startPosObject->m_linkId) : nullptr;
        auto p2Settings = other ? other->getSettingsObject() : nullptr;
        int startMode = -1;

        if(settings) {
            m_startPosObject->m_startPosition += ccp(0, settings->m_fields->cameraOffset * 30);
            if(p2Settings && !p2Settings->m_disableStartPos && !largeDual(settings->m_startMode) && largeDual(p2Settings->m_startMode)) {
                startMode = settings->m_startMode;
                settings->m_startMode = 4;
            }
        }

        GJBaseGameLayer::loadStartPosObject();
        if(!settings) return;

        if(PlayLayer::get()) GJBaseGameLayer::toggleFlipped(settings->m_mirrorMode, true);

        m_startPosObject->m_startPosition -= ccp(0, settings->m_fields->cameraOffset * 30);
        if(startMode != -1) {
            settings->m_startMode = startMode;
            setMode(m_player1, startMode);
        }

        if(settings->m_fields->isFreeCam) {
            CC_SAFE_RELEASE(m_fields->tpObj);
            m_fields->tpObj = TeleportPortalObject::create("edit_eGameRotBtn_001.png", true);
            m_fields->tpObj->m_cameraIsFreeMode = true;
            m_fields->tpObj->m_cameraEasingValue = 10;
            m_fields->tpObj->m_cameraPaddingValue = .5f;
            m_fields->tpObj->retain();
            this->playerWillSwitchMode(m_player1, m_fields->tpObj);
        }

        m_player1->setYVelocity(settings->m_fields->yVelocity, 0);

        if(!p2Settings || p2Settings->m_disableStartPos) {
            if(m_player2) {
                m_player2->setYVelocity(-settings->m_fields->yVelocity, 0);
            }
            return;
        }

        m_player2->setPosition(other->getPosition());
        m_player2->m_position = other->getPosition();
        m_player2->setYVelocity(p2Settings->m_fields->yVelocity * 2, 0);
        m_player2->flipGravity(p2Settings->m_isFlipped, true);
        m_player2->togglePlayerScale(p2Settings->m_startMini, true);
        m_player2->doReversePlayer(p2Settings->m_reverseGameplay);

        if(p2Settings->m_startMode != settings->m_startMode) {
            setMode(m_player2, p2Settings->m_startMode);
        }
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
};
