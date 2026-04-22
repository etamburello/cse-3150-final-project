#pragma once
#include "policy.hpp"
#include <memory>
#include <vector>

class AS {
public:
	int asn;

	std::vector<AS*> providers;  //upstream
	std::vector<AS*> customers; //downstream
	std::vector<AS*> peers;

	std::unique_ptr<Policy> p;
	int rank = -1;

	AS (int id);
};
