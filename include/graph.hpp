#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include "AS.hpp"


class Graph {
private:
	std::unordered_map<int, std::shared_ptr<AS>> map;
	bool dfsCycle(AS* node, std::unordered_map<int, int>& visted);

public:
	std::vector<std::vector<AS*>> ranks;

	std::shared_ptr<AS> make(int asn);
	void loadFile(const std::string& filename);
	void detectCycles();
	void assignRanks();

	void seedAnnoucement(int asn, const std::string& prefix);
	void propagate();
};	
