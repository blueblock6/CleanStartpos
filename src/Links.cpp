#include <Links.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/DrawGridLayer.hpp>

using namespace geode::prelude;

void Links::insertPair(AdvancedStartPos* primary, AdvancedStartPos* secondary, short id) {
    links[id] = {primary, secondary};
}

AdvancedStartPos* Links::getPrimary(short id) {
    if(links.contains(id)) {
        return links[id].first;
    } else {
        return nullptr;
    }
}

AdvancedStartPos* Links::getSecondary(short id) {
    if(links.contains(id)) {
        return links[id].second;
    } else {
        return nullptr;
    }
}

void Links::setPrimary(short id, AdvancedStartPos* primary) {
    if(!links.contains(id)) {
        links[id] = {primary, nullptr};
    } else {
        links[id].first = primary;
    }
}

void Links::setSecondary(short id, AdvancedStartPos* secondary) {
    if(!links.contains(id)) {
        links[id] = {nullptr, secondary};
    } else {
        links[id].second = secondary;
    }
}

void Links::destroy(short id) {
    if(!links.contains(id)) return;

    auto link = links[id];
    link.first->m_linkId = 0;
    link.second->m_linkId = 0;
    links.erase(id);
}
short Links::nextId() {
    if(links.empty()) {
        return 1;
    } else {
        return links.rbegin()->first + 1;
    }
}

class $modify(LevelEditorLayer) {
    $override
    void removeSpecial(GameObject* p0) {
        LevelEditorLayer::removeSpecial(p0);
        short id = p0->m_linkId;

        if(p0->m_objectID == 34) {
            Links::setSecondary(id, nullptr);
        } else if(p0->m_objectID == 31 && id != 0) {
            Links::setPrimary(id, nullptr);
            geode::queueInMainThread([this, p0, id]() {
                auto link = Links::getSecondary(id);
                if(!link || m_undoObjects->count() == 0) return;

                auto undo = static_cast<UndoObject*>(m_undoObjects->lastObject());
                if(undo->m_command == UndoCommand::Delete) {
                    undo = UndoObject::createWithArray(CCArray::createWithObject(p0), UndoCommand::DeleteMulti);
                    m_undoObjects->replaceObjectAtIndex(m_undoObjects->count() - 1, undo);
                }

                if(undo->m_command == UndoCommand::DeleteMulti) {
                    undo->m_objects->addObject(link);
                } else {
                    return;
                }

                this->removeObject(link, true);
            });
        }
    }

    $override
    void addSpecial(GameObject* p0) {
        LevelEditorLayer::addSpecial(p0);
        short id = p0->m_linkId;
        if(p0->m_objectID == 31 && id != 0) {
            Links::setPrimary(id, static_cast<AdvancedStartPos*>(p0));
        } else if(p0->m_objectID == 34 && id != 0) {
            Links::setSecondary(id, static_cast<AdvancedStartPos*>(p0));
        }
    }
};

class $modify(EditorUI) {
    $override
    void selectObject(GameObject* p0, bool pl) {
        if(p0->m_objectID == 34) {
            p0->m_baseColor->m_defaultColorID = 1011; // makes edit object button work
        }

        EditorUI::selectObject(p0, pl);
    }

    $override
    void editObject(CCObject* p0) {
        if(m_selectedObject && m_selectedObject->m_objectID == 34 && m_selectedObject->m_linkId != 0) {
            auto adv = static_cast<AdvancedStartPos*>(m_selectedObject);
            LevelSettingsLayer::create(adv->getSettingsObject(), nullptr)->show();
        } else {
            if(m_selectedObject && m_selectedObject->m_objectID == 31) {
                static_cast<StartPosObject*>(m_selectedObject)->m_startSettings->setUserObject(m_selectedObject);
            }
            EditorUI::editObject(p0);
        }
    }
};

class $modify(DrawGridLayer) {
    $override
    void draw() {
        DrawGridLayer::draw();

        ccDrawColor4B(191, 191, 191, 255);
        glLineWidth(this->getParent()->getScale() * 10.f);

        for(auto entry : Links::links) {
            if(entry.second.second && !entry.second.second->m_isSelected) {
                entry.second.second->setColor({127, 127, 127});
            }
            if(entry.second.first && entry.second.second) {
                ccDrawLine(entry.second.first->getPosition(), entry.second.second->getPosition());
            }
        }
    }
};
