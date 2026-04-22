#pragma once
#include "announcement.hpp"
#include "policy.hpp"
#include <unordered_map>

class BGP : public Policy {
private:
    std::unordered_map<std::string, Announcement> local_rib; //best route per prefix
    std::unordered_map<std::string, std::vector<Announcement>> rec_queue; //pending batch

    bool better(const Announcement& a, const Announcement& b);

public:
    void receive(const Announcement& a) override;
    bool process(int curr_asn) override;
    const std::unordered_map<std::string, Announcement>& getRib() const override;
};
