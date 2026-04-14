#pragma once
#include <vector>
#include <climits>

class AS {
public:
	int asn;
	std::vector<AS*> providers;
	std::vector<AS*> customers;
	std::vector<AS*> peers;
	int distance;
	AS* parent;
	int relation;
	AS (int id);
};
