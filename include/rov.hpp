#pragma once 
#include "announcement.hpp"
#include "bgp.hpp"

class ROV : public BGP {
public: 
    void receive(const Announcement& a) override;
};