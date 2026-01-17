#include <AdvancedStartPos.hpp>
#include <Links.hpp>

using namespace geode::prelude;

StartPosObject* AdvancedStartPos::getRealStartPos() {
    return typeinfo_cast<StartPosObject*>(this);
}

LevelSettingsObjectExt* AdvancedStartPos::getSettingsObject() {
    if(auto startPos = getRealStartPos()) {
        auto settings = static_cast<LevelSettingsObjectExt*>(startPos->m_startSettings);
        settings->m_fields->isFreeCam = m_isIceBlock;
        settings->m_fields->cameraOffset = startPos->m_controlID;
        settings->m_fields->yVelocity = *reinterpret_cast<float*>(&settings->m_spawnGroup);
        return settings;
    }

    LevelSettingsObjectExt* settings;
    if(secondaryStartPosSettings.contains(this)) {
        settings = secondaryStartPosSettings.at(this);
    } else {
        settings = static_cast<LevelSettingsObjectExt*>(LevelSettingsObject::create());
        settings->m_startMode = (m_isNonStickX << 2) | (m_isNonStickY << 1) | m_isDontBoostY;
        settings->m_isFlipped = m_isExtraSticky;
        settings->m_startMini = m_isScaleStick;
        settings->m_reverseGameplay = m_isDontBoostX;
        settings->m_disableStartPos = m_hasExtendedCollision;

        settings->m_startsWithStartPos = true;
        settings->retain();
        settings->setUserObject(this);
        secondaryStartPosSettings.insert({this, settings});
    }

    auto primary = Links::getPrimary(m_linkId);
    if(primary) {
        auto pSettings = primary->getSettingsObject();
        settings->m_startDual = pSettings->m_startDual;
        settings->m_mirrorMode = pSettings->m_mirrorMode;
        settings->m_fields->isFreeCam = pSettings->m_fields->isFreeCam;
        settings->m_resetCamera = pSettings->m_resetCamera;
        settings->m_rotateGameplay = pSettings->m_rotateGameplay;

        settings->m_fields->yVelocity = *reinterpret_cast<float*>(&pSettings->m_propertykA44);
    }

    return settings;
}

void AdvancedStartPos::encodeSettings(LevelSettingsObjectExt* settings) {
    if(auto startPos = getRealStartPos()) {
        startPos->m_isIceBlock = settings->m_fields->isFreeCam;
        startPos->m_controlID = settings->m_fields->cameraOffset;
        settings->m_spawnGroup = *reinterpret_cast<int*>(&settings->m_fields->yVelocity);
        return;
    }
    m_isNonStickX = (settings->m_startMode & 0b100) >> 2;
    m_isNonStickY = (settings->m_startMode & 0b010) >> 1;
    m_isDontBoostY = (settings->m_startMode & 0b001);
    m_isExtraSticky = settings->m_isFlipped;
    m_isScaleStick = settings->m_startMini;
    m_isDontBoostX = settings->m_reverseGameplay;
    m_hasExtendedCollision = settings->m_disableStartPos;
    if(auto primary = Links::getPrimary(m_linkId)) {
        primary->getSettingsObject()->m_propertykA44 = *reinterpret_cast<int*>(&settings->m_fields->yVelocity);
    }
}

void AdvancedStartPos::customSetup() {
    GameObject::customSetup();
    if(m_objectID == 31) {
        geode::queueInMainThread([this](){
            getRealStartPos()->m_startSettings->setUserObject(this);
        });
        if(m_linkId != 0) {
            Links::setPrimary(m_linkId, this);
        }
    } else if(m_objectID == 34 && m_linkId != 0) {
        if(PlayLayer::get()) {
            m_isHide = true;
        }
        Links::setSecondary(m_linkId, this);
    }
}
