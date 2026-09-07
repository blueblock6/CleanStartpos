#pragma once
#include "StartPosUI.hpp"

class AdvancedUI : public StartPosUI {
private:
    geode::TextInput* m_velocityInput;
    geode::TextInput* m_offsetInput;
    geode::TextInput* m_targetOrderInput;
    geode::TextInput* m_targetChannelInput;

    bool init(AdvancedStartPos* startPos);
public:
    void onClose(CCObject* sender) override;
    static AdvancedUI* create(AdvancedStartPos* startPos);
};
