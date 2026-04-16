#pragma once
#include <unordered_map>
#include "announcement.hpp"

class Policy {
public:
	virtual void receive(const Announcement& a) = 0;
	virtual bool process(int curr_asn) = 0;
	virtual const std::unordered_map<std::string, Announcement>& getRib() const = 0;
	virtual ~Policy() {};
};
