#pragma once
#include <Geode/Geode.hpp>
#include <AdvancedStartPos.hpp>

class Links {
public:
    inline static std::map<short, std::pair<AdvancedStartPos*, AdvancedStartPos*>> links;

    static void insertPair(AdvancedStartPos* primary, AdvancedStartPos* secondary, short id);
    static AdvancedStartPos* getPrimary(short id);
    static AdvancedStartPos* getSecondary(short id);
    static void setPrimary(short id, AdvancedStartPos* primary);
    static void setSecondary(short id, AdvancedStartPos* secondary);
    static void destroy(short id);
    static short nextId();
};
