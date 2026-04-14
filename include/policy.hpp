#pragma once
#include <unordered_map>
#include "annoucement.hpp"

class Policy {
public:
	virtual void receive(const Annoucement& a) = 0;
	virtual void process(int curr_asn) = 0;
	virtual const std::unordered_map<std::string, Annoucement>& getRib() const = 0;
	virtual ~Policy() {};
};
