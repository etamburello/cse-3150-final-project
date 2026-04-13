#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "graph.h"

Graph::~Graph() {
	for (auto& pair : nodes) {
		delete pair.second;
	}
}

AS* Graph::makeNode(int asn){
	if (nodes.find(asn) == nodes.end()){
		nodes[asn] = new AS(asn);
	}
	return nodes[asn];
}

void Graph::addProvCustom(int provider, int customer){
	AS* prov = makeNode(provider);
	AS* custom = makeNode(customer);
	prov->customers.push_back(custom);
	custom->providers.push_back(prov);
}

void Graph::addPeerRelation(int p1, int p2){
	AS* one = makeNode(p1);
	AS* two = makeNode(p2);
	one->peers.push_back(two);
	two->peers.push_back(one);
}

bool Graph::hasCycles(AS* node, std::unordered_set<int>& visited, std::unordered_set<int>& r_stack, bool checkProv){
	if (r_stack.count(node->asn)){
		return true;
	}
	if (visited.count(node->asn)){
		return false;
	}

	visited.insert(node->asn);
	r_stack.insert(node->asn);
	const std::vector<AS*>& neighborhood = checkProv ? node->providers : node->customers;

	for (AS* neighbor : neighborhood){
		if (hasCycles(neighbor, visited, r_stack, checkProv)){
			return true;
		}
	}

	r_stack.erase(node->asn);
	return false;
}

bool Graph::hasProvCycle(){
	std::unordered_set<int> visited, r_stack;
	for (auto& pair : nodes){
		if (hasCycles(pair.second, visited, r_stack, true)){
			return true;
		}
	}
	return false;
}

bool Graph::hasCustomCycle(){
	std::unordered_set<int> visited, r_stack;
	for (auto& pair : nodes){
		if (hasCycles(pair.second, visited, r_stack, false)){
			return true;
		}
	}
	return false;
}

void Graph::printGraph(){
	for (const auto& pair : nodes) {
		AS* node = pair.second;
		std::cout << "AS: " << node->asn << std::endl;

		std::cout << "Providers: ";
		for (const auto p : node->providers) {
			std::cout << p->asn << " ";
		}

		std::cout << "Customers: ";
		for (const auto c : node->customers) {
			std::cout << c->asn << " ";
		}

		std::cout << "Peers: ";
		for (const auto p : node->peers) {
			std::cout << p->asn << " ";
		}
		std::cout << "\n\n";
	}
}

