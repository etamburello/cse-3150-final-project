#pragma once
#include <string>
#include <vector>

enum class Relationship {
        ORIGIN,
        CUSTOMER,
        PROVIDER,
        PEER
};

struct Announcement {
        std::string prefix;
        std::vector<int> path;
        int next;
        Relationship relation;
        bool rov_invalid = false;
};
