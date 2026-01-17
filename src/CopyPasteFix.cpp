#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Links.hpp>

using namespace geode::prelude;

class $modify(LevelEditorLayer) {
    $override
    CCArray* createObjectsFromString(const gd::string& p0, bool p1, bool p2) {
        auto oldLinks = Links::links;

        auto ret = LevelEditorLayer::createObjectsFromString(p0, p1, p2);
        std::unordered_map<short, short> newIDs;
        std::vector<AdvancedStartPos*> newSPs;
        std::unordered_map<size_t, StartPosObject*> startPoses;
        auto arr = CCArrayExt<GameObject*>(ret);
        short next = Links::nextId();

        // Collect sps & linked sp objs
        for(size_t i = 0; i < arr.size(); ++i) {
            auto obj = arr[i];
            short id = obj->m_linkId;
            if(obj->m_objectID == 31) {
                startPoses[i] = static_cast<StartPosObject*>(obj);
            }
            // Generate new link ids
            if(id != 0 && (obj->m_objectID == 31 || obj->m_objectID == 34)) {
                if(!newIDs.contains(id)) {
                    newIDs[id] = next++;
                }
                newSPs.push_back(static_cast<AdvancedStartPos*>(obj));
            }
        }

        Links::links = oldLinks;

        // Reassign link ids
        for(auto obj : newSPs) {
            short id = newIDs[obj->m_linkId];
            obj->m_linkId = id;
            if(obj->m_objectID == 31) {
                Links::setPrimary(id, obj);
            } else {
                Links::setSecondary(id, obj);
            }
        }

        // Process extra sp data
        if(!startPoses.empty()) {
            std::stringstream ss(p0);
            std::string token;
            size_t i = 0;
            while(std::getline(ss, token, ';')) {
                if(startPoses.contains(i)) {
                    startPoses[i]->loadSettingsFromString(token);
                }
                ++i;
            }
        }

        return ret;
    }
};
