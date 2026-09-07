#pragma once
#include <Geode/Geode.hpp>
#include "AdvancedStartPos.hpp"

class SelectorUI : public cocos2d::CCLayerColor {
private:
    AdvancedStartPos* m_startPos;

    bool init(AdvancedStartPos* startPos);
    void onClose(CCObject*);
    void keyBackClicked() override;
    virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) override;
public:
    static SelectorUI* create(AdvancedStartPos* startPos);
    void show();
};
