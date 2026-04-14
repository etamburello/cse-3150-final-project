#pragma once
#include <unordered_map>
#include <unordered_set>
#include "AS.h"

class Graph {
private:
	std::unordered_map<int, AS*> nodes;
	bool hasCycles(AS* node, std::unordered_set<int>& visited, std::unordered_set<int>& r_stack, bool checkProv);
public:
	~Graph();
	AS* makeNode(int asn);
	void addProvCustom(int provider, int customer);
	void addPeerRelation(int p1, int p2);
	bool hasProvCycle();
	bool hasCustomCycle();
	void printGraph();
	void computeRoutes(int distance);
	void printPaths(int distance);
};
