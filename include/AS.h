#pragma once
#include <vector>

class AS {
public:
	int asn;
	std::vector<AS*> providers;
	std::vector<AS*> customers;
	std::vector<AS*> peers;
	AS (int id);
};
