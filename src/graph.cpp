#include <cstddef>
#include <iostream>
#include <queue>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include "graph.hpp"
#include "AS.hpp"

std::shared_ptr<AS> Graph::make(int asn) {
	if (!map.count(asn)) {
        map[asn] = std::make_shared<AS>(asn);
    }
    return map[asn];
}

void Graph::loadFile(const std::string& filename) {
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Cannot open file\n";
	}
	std::string line;

	while(std::getline(file, line)) {
			if (line.empty() || line[0] == '#') {
				continue;
			}

			std::stringstream iss(line);
			int a, b, rel;
			iss >> a >> b >> rel;

			auto first = make(a);
			auto second = make(b);

			if (rel == -1) {
				first->customers.push_back(second.get());
				second->providers.push_back(first.get());
			}
			else if (rel == 0) {
				first->peers.push_back(second.get());
				second->peers.push_back(first.get());
			}
			else {
				first->providers.push_back(second.get());
				second->customers.push_back(first.get());
			}
	}
}

bool Graph::dfsCycle(AS* node, std::unordered_map<int,int>& visited) {
	if (visited[node->asn] == 1) {
		return true;
	}
	if (visited[node->asn] == 2) {
		return false;
	}

	visited[node->asn] = 2;
	return false;
}

void Graph::detectCycles() {
	std::unordered_map<int, int> visited;

	for (auto& [n, aptr] : map) {
		if (dfsCycle(aptr.get(), visited)) {
			std::cerr << "Cycle detected!\n";
			exit(1);
		}
	}
}

void Graph::assignRanks() {
	std::queue<AS*> que;

	for (auto& [n, aptr] : map) {
		if (aptr->customers.empty()) {
			aptr->rank = 0;
			que.push(aptr.get());
		}
	}

	while(!que.empty()) {
		AS* curr = que.front();
		for (auto p : curr->providers) {
			if (p->rank < curr->rank + 1) {
				p->rank = curr->rank + 1;
				que.push(p);
			}
		}
	}

	int max_rank = 0;
	for (auto& [n, aptr] : map) {
		max_rank = std::max(max_rank, aptr->rank);
	}

	ranks.clear();
	ranks.resize(max_rank + 1);
	for (auto& [n, aptr] : map) {
		ranks[aptr->rank].push_back(aptr.get());
	}
}

void Graph::seedAnnoucement(int asn, const std::string& prefix) {
	Annoucement a;
	a.prefix = prefix;
	a.path = {asn};
	a.next = asn;
	a.relation = Relationship::ORIGIN;

	auto an = map[asn];
	an->p->receive(a);
	an->p->process(asn);
}

void Graph::propagate() {
	//up
	for (size_t a = 0; a < ranks.size(); a++) {
		for (auto r : ranks[a]) {
			for (auto& [prefex, ann] : r->p->getRib()) {
				for (auto prov : r->providers) {
					Annoucement new_annouce = ann;
					new_annouce.next = r->asn;
					new_annouce.relation = Relationship::CUSTOMER;
					prov->p->receive(new_annouce);
				}
			}
		}

		for (auto r : ranks[a]) {
			r->p->process(r->asn);
		}
	}

	//across
	for (auto& [n, aptr] : map) {
		for (auto& [prefex, ann] : aptr->p->getRib()) {
			for (auto peer : aptr->peers) {
				Annoucement new_annouce = ann;
				new_annouce.next = aptr->asn;
				new_annouce.relation = Relationship::PEER;
				peer->p->receive(new_annouce);
			}

		}
	}

	//process
	for (auto& [n, aptr] : map) {
		aptr->p->process(aptr->asn);
	}

	//down
	for (int a = (int)ranks.size() -1; a >= 0; a--) {
		for (auto r : ranks[a]) {
			for (auto& [prefex, ann] : r->p->getRib()) {
				for (auto custom : r->customers) {
					Annoucement new_annouce = ann;
					new_annouce.next = r->asn;
					new_annouce.relation = Relationship::PROVIDER;
					custom->p->receive(new_annouce);
				}
			}
		}

		for (auto r : ranks[a]) {
			r->p->process(r->asn);
		}
	}
}
