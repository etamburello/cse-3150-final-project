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
        Relationship received_from_relationship;
        bool rov_invalid = false;
};
