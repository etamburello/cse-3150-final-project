#pragma once
#include "annoucement.hpp"
#include "policy.hpp"
#include <unordered_map>

class BGP : public Policy {
private:
    std::unordered_map<std::string, Annoucement> local_rib;
    std::unordered_map<std::string, std::vector<Annoucement>> rec_queue;

    bool better(const Annoucement& a, const Annoucement& b);

public:
    void receive(const Annoucement& a) override;
    void process(int curr_asn) override;
    const std::unordered_map<std::string, Annoucement>& getRib() const override;
};