#pragma once
#include <Geode/Geode.hpp>
#include "AdvancedStartPos.hpp"

class SelectorUI : public cocos2d::CCLayerColor {
private:
    AdvancedStartPos* m_startPos;

    bool init(AdvancedStartPos* startPos);
    void keyBackClicked() override;
public:
    static SelectorUI* create(AdvancedStartPos* startPos);
    void show();
    void onClose(CCObject*);
};

class SelectorOverlay : public cocos2d::CCLayer {
private:
    AdvancedStartPos* m_startPos;

    bool init(AdvancedStartPos* startPos);
    virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) override;
public:
    static SelectorOverlay* create(AdvancedStartPos* startPos);
};
