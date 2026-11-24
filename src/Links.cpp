#include "Hooks.hpp"
#include "Settings.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

void HookedGameObject::customSetup() {
    GameObject::customSetup();
    if(m_objectID == 31 || m_objectID == 34) {
        auto startPos = modify_cast<HookedStartPosObject*>(this);
        getBGL()->addLink(startPos);
        if(m_objectID == 34 && startPos->linkId() != 0 && PlayLayer::get()) {
            m_isHide = true;
        }
    }
}

void HookedGJBaseGameLayer::addLink(HookedStartPosObject* startPos) {
    short id = startPos->linkId();
    if(id == 0) return;

    auto& pair = m_fields->m_links[id];

    if(startPos->isP2()) {
        pair.second = startPos;
        if(auto other = pair.first) {
            startPos->getOther() = other;
            other->getOther() = startPos;
        }
    } else {
        pair.first = startPos;
        if(auto other = pair.second) {
            startPos->getOther() = other;
            other->getOther() = startPos;
        }
    }
}

std::pair<HookedStartPosObject*, HookedStartPosObject*>& HookedGJBaseGameLayer::getLink(short id) {
    return m_fields->m_links.at(id);
}

void HookedLevelEditorLayer::addSpecial(GameObject* object) {
    LevelEditorLayer::addSpecial(object);
    if(object->m_objectID == 31 || object->m_objectID == 34) {
        getBGL()->addLink(modify_cast<HookedStartPosObject*>(object));
    }
}

void HookedLevelEditorLayer::removeSpecial(GameObject* object) {
    LevelEditorLayer::removeSpecial(object);

    if(object->m_objectID == 34) {
        auto startPos = modify_cast<HookedStartPosObject*>(object);
        if(auto other = startPos->getOther()) {
            getBGL()->getLink(other->linkId()).second = nullptr;
            other->getOther() = nullptr;
        }
    } else if(object->m_objectID == 31) {
        auto startPos = modify_cast<HookedStartPosObject*>(object);
        if(startPos->getOther() == nullptr) return;
        getBGL()->getLink(startPos->linkId()) = {nullptr, nullptr};

        queueInMainThread([startPos = Ref(startPos), ts = Ref(this)]() {
            auto other = startPos->getOther();
            if(other == nullptr || ts->m_undoObjects->count() == 0) return;
            auto undo = static_cast<UndoObject*>(ts->m_undoObjects->lastObject());

            if(undo->m_command == UndoCommand::Delete) {
                undo = UndoObject::createWithArray(CCArray::createWithObject(startPos), UndoCommand::DeleteMulti);
                ts->m_undoObjects->replaceObjectAtIndex(ts->m_undoObjects->count() - 1, undo);
            }

            if(undo->m_command == UndoCommand::DeleteMulti) {
                undo->m_objects->addObject(other);
                ts->removeObject(other, true);
            }
        });
    }
}

void HookedDrawGridLayer::draw() {
    DrawGridLayer::draw();

    ccDrawColor4B(Settings::lineColor.r, Settings::lineColor.g, Settings::lineColor.b, 255);
    glLineWidth(this->getParent()->getScale() * 10.f);

    auto bgl = getBGL();
    for(auto& [id, pair] : bgl->m_fields->m_links) {
        if(auto obj = pair.first; pair.second && obj && !obj->m_isSelected) {
            obj->setColor(Settings::p1Color);
        }

        if(auto obj = pair.second; obj && !obj->m_isSelected) {
            obj->setColor(Settings::p2Color);
        }

        if(pair.first && pair.second) {
            ccDrawLine(pair.first->getPosition(), pair.second->getPosition());
        }
    }

    glLineWidth(1.f);
}

CCArray* HookedLevelEditorLayer::createObjectsFromString(const std::string& str, bool noUndo, bool noLimit) {
    auto bgl = modify_cast<HookedGJBaseGameLayer*>(this);
    auto oldLinks = bgl->m_fields->m_links;

    auto ret = LevelEditorLayer::createObjectsFromString(str, noUndo, noLimit);

    auto objects = CCArrayExt<GameObject*>(ret);
    std::unordered_map<short, short> idMap;
    std::vector<HookedStartPosObject*> startPositions;
    short nextId = 1;

    for(size_t i = 0; i < objects.size(); ++i) {
        auto obj = objects[i];
        if(obj->m_objectID != 31 && obj->m_objectID != 34) {
            continue;
        }

        auto startPos = modify_cast<HookedStartPosObject*>(obj);
        if(short id = startPos->linkId(); id != 0) {
            if(!idMap.contains(id)) {
                while(oldLinks.contains(nextId)) ++nextId;
                idMap[id] = nextId;
            }
            startPositions.push_back(startPos);
        }
    }

    bgl->m_fields->m_links = oldLinks;

    for(auto obj : startPositions) {
        short& id = obj->linkId();
        id = idMap[id];
        bgl->addLink(obj);
    }

    return ret;
}
