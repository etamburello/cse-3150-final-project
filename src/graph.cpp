#include <cstddef>
#include <iostream>
#include <queue>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "graph.hpp"
#include "AS.hpp"
#include "rov.hpp"

std::shared_ptr<AS> Graph::make(int asn) {
	auto it = map.find(asn);
	if (it == map.end()) {
		it = map.emplace(asn, std::make_shared<AS>(asn)).first;
	}
	return it->second;
}

void Graph::loadFile(const std::string& filename) {
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Cannot open CAIDA file\n";
		return;
	}
	std::string line;

	while(std::getline(file, line)) {
			if (line.empty() || line[0] == '#') {
				continue;
			}

			std::stringstream iss(line);
			std::string a_str, b_str, rel_str;
			if (!std::getline(iss, a_str, '|') ||
			    !std::getline(iss, b_str, '|') ||
			    !std::getline(iss, rel_str, '|')) {
				continue;
			}

			int a, b, rel;
			try {
				a = std::stoi(a_str);
				b = std::stoi(b_str);
				rel = std::stoi(rel_str);
			} catch (const std::exception&) {
				continue;
			}

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
	visited[node->asn] = 1;
	for(auto prov : node->providers) {
		if (visited[prov->asn] != 2) {
			if (dfsCycle(prov, visited)) {
				return true;
			}
		}
	}

	visited[node->asn] = 2;
	return false;
}

bool Graph::detectCycles() {
	std::unordered_map<int, int> visited;

	for (auto& [n, aptr] : map) {
		if (dfsCycle(aptr.get(), visited)) {
			return true;
		}
	}
	return false;
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
		que.pop();
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

void Graph::seedAnnouncement(const std::string& filename) {
	std::ifstream file(filename);
    	if (!file) {
        	std::cerr << "Cannot open announcements file\n";
        	return;
	}

	std::string line;
	std::getline(file, line);
	while(std::getline(file, line)) {
		if (line.empty()) continue;
		std::stringstream iss(line);
		std::string asn_str, prefix, rov_str;

		std::getline(iss, asn_str, ',');
        	std::getline(iss, prefix, ',');
		std::getline(iss, rov_str, ',');
		int asn = std::stoi(asn_str);
		std::transform(rov_str.begin(), rov_str.end(), rov_str.begin(), ::tolower);

		auto node = make(asn);
		Announcement a;
		a.prefix = prefix;
		a.path = {asn};
		a.next = asn;
		a.received_from_relationship = Relationship::ORIGIN;
		a.rov_invalid = (rov_str == "true");
		node->p->receive(a);
		node->p->process(asn);
	}
}

void Graph::propagate() {
	std::cout << "before up: nodes=" << map.size() << std::endl;
	//up(to providers)
	for (size_t r = 0; r < ranks.size(); r++) {
		for(auto as : ranks[r]) {
			as->p->process(as->asn);
    		}
        	for(auto as : ranks[r]) {
    			for(auto& [prefix, ann] : as->p->getRib()) {
                		if(ann.received_from_relationship == Relationship::CUSTOMER || ann.received_from_relationship == Relationship::ORIGIN) {
                			for(auto prov : as->providers) {
                    				Announcement new_a = ann;
                   					new_a.next = as->asn;
                    				new_a.received_from_relationship = Relationship::CUSTOMER;
									prov->p->receive(new_a);
									
                    			}
                		}
            		}
        	}
       	}
		std::cout << "after up: nodes=" << map.size() << std::endl;

		std::cout << "before across: nodes=" << map.size() << std::endl;
    	//across(to peers)
    	for(auto& [n, aptr] : map) {
        	for(auto& [prefix, ann] : aptr->p->getRib()) {
            		if(ann.received_from_relationship == Relationship::CUSTOMER || ann.received_from_relationship == Relationship::ORIGIN) {
               			for(auto peer : aptr->peers) {
                  			Announcement new_a = ann;
                    			new_a.next = aptr->asn;
                    			new_a.received_from_relationship = Relationship::PEER;
                    			peer->p->receive(new_a);
               			}
           		}
        	}
    	}
	for(auto& [n, aptr] : map) {
		aptr->p->process(n);
	}
	std::cout << "after across: nodes=" << map.size() << std::endl;

	std::cout << "before down: nodes=" << map.size() << std::endl;
    	//down(to customers)
    	for (int r = (int)ranks.size() - 1; r >= 0; r--) {
        	for (auto as : ranks[r]) {
            		as->p->process(as->asn);
       		}
       		for (auto as : ranks[r]) {
          		for (auto& [prefix, ann] : as->p->getRib()) {
                		for (auto cust : as->customers) {
                 			Announcement new_a = ann;
                    			new_a.next = as->asn;
                    			new_a.received_from_relationship = Relationship::PROVIDER;
                    			cust->p->receive(new_a);
                		}
            		}
        	}
    	}
	std::cout << "after down: nodes=" << map.size() << std::endl;
}

void Graph::setROV(int asn) {
	if (!map.count(asn)) {
		return;
	}

    map[asn]->p = std::make_unique<ROV>();
}

void Graph::loadROV(const std::string& filename) {
	std::ifstream file(filename);
    	if (!file) {
        	std::cerr << "Cannot open ROV file\n";
        	return;
    	}

    	int asn;
    	while (file >> asn) {
        	setROV(asn);
    	}
}

void Graph::writeCSV(const std::string& filename) {

	std::ofstream out(filename);
    	if (!out) {
        	std::cerr << "Cannot open output file\n";
        	return;
    	}

	out << "asn,prefix,as_path\n";
	for (auto& [asn, as_ptr] : map) {
        	for (auto& [prefix, ann] : as_ptr->p->getRib()) {
			out << asn << "," << prefix << ",\"(";

            		for (size_t a = 0; a < ann.path.size(); a++) {
                		out << ann.path[a];
                		if (a != ann.path.size() - 1) {
                    			out << ", ";
                		}
            		}
			if (ann.path.size() == 1) {
				out << ",";
			}
            		out << ")\"\n";
        	}
    	}
}

