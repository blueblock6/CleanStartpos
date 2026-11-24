#include "Hooks.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

void HookedEditorUI::selectObject(GameObject* object, bool ignoreFilter) {
    if(object->m_objectID == 34) {
        // magic to enable edit object button
        object->m_baseColor->m_defaultColorID = 1011;
    }
    EditorUI::selectObject(object, ignoreFilter);
}

void HookedEditorUI::editObject(CCObject* sender) {
    if(m_selectedObject && m_selectedObject->m_objectID == 34) {
        auto startPos = static_cast<StartPosObject*>(m_selectedObject);
        StartPosLayer::create(startPos)->show();
    } else {
        EditorUI::editObject(sender);
    }
}
