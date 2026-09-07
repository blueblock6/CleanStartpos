#pragma once
#include "StartPosUI.hpp"

class SimpleUI : public StartPosUI {
private:
    bool init(AdvancedStartPos* startPos);
public:
    virtual void onClose(CCObject* sender) override;
    static SimpleUI* create(AdvancedStartPos* startPos);
};
