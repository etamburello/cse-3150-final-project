#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include "AS.hpp"


class Graph {
private:
	std::unordered_map<int, std::shared_ptr<AS>> map;
	bool dfsCycle(AS* node, std::unordered_map<int, int>& visited);

public:
	std::vector<std::vector<AS*>> ranks;

	std::shared_ptr<AS> make(int asn);
	void loadFile(const std::string& filename);
	bool detectCycles();
	void assignRanks();
	void loadAnnouncement(const std::string& filename);
	void propagate();

	void setROV(int asn);
	void loadROV(const std::string& filename);

	void writeCSV(const std::string& filename);
};

