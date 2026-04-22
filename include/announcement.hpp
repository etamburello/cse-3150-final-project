#pragma once
#include <string>
#include <vector>

enum class Relationship {
        ORIGIN,   
        CUSTOMER, //from customer
        PROVIDER, //from provider (upstream)
        PEER      //from peer
};

struct Announcement {
        std::string prefix;
        std::vector<int> path; 
        int next;                
        Relationship received_from_relationship;
        bool rov_invalid = false; //if true, ROV policy may drop before queuing
};
