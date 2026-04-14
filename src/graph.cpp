#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <queue>
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

void Graph::computeRoutes(int dist) {
	for (auto& pair : nodes) {
		pair.second->distance = INT_MAX;
		pair.second->parent = nullptr;
		pair.second->relation = 0;
	}
	AS* destination = makeNode(dist);
	std::queue<AS*> que;

	destination->distance = 0;
	destination->relation = 1;
	que.push(destination);

	while (!que.empty()) {
		AS* curr = que.front();
		que.pop();

		//customers
		for (AS* c : curr->customers) {
			if (c->distance == INT_MAX) {
				c->distance = curr->distance + 1;
				c->parent = curr;
				c->relation = 1;
				que.push(c);
			}
		}

		//peers
		if (curr->relation != -1) {
			for (AS* p : curr->peers) {
				if (p->distance == INT_MAX) {
					p->distance = curr->distance + 1;
					p->parent = curr;
					p->relation = 0;
					que.push(p);
				}
			}
		}

		//providers
		if (curr->relation == 1) {
			for (AS* pr : curr->providers) {
				if (pr->distance == INT_MAX) {
					pr->distance = curr->distance + 1;
					pr->parent = curr;
					pr->relation = -1;
					que.push(pr);
				}
			}
		}
	}
}

void Graph::printPaths(int dist) {
	for (const auto& pair : nodes) {
		AS* sec = pair.second;
		if (sec->distance == INT_MAX) {
			continue;
		}

		std::cout << "AS: " << sec->asn << " -> ";

		AS* curr = sec;
		while (curr != nullptr) {
			std::cout << curr->asn;
			if (curr->parent) {
				std::cout << " -> ";
			}
			curr = curr->parent;
		}
		std::cout << "\n";
	}
}
