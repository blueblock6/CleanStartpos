#include "Hooks.hpp"

class StartPosExtraSettingsPopup : public SetupTriggerPopup {
public:
    static StartPosExtraSettingsPopup* create(HookedStartPosObject* startPosObject);

private:
    HookedStartPosObject* m_startPosObject;

    bool init(HookedStartPosObject* startPosObject);
    float getValue(int tag) override;
    void valueDidChange(int tag, float value) override;
};
