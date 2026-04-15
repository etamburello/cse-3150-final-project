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
	if (!map.count(asn)) {
        map[asn] = std::make_shared<AS>(asn);
    }
    return map[asn];
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

void Graph::loadAnnouncement(const std::string& filename) {
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
		std::string asn_str, prefix;

		std::getline(iss, asn_str, ',');
        	std::getline(iss, prefix, ',');
		int asn = std::stoi(asn_str);

		if (!map.count(asn)) continue;

		auto node = map[asn];
		Announcement a;
		a.prefix = prefix;
		a.path = {asn};
		a.next = asn;
		a.relation = Relationship::ORIGIN;
		a.rov_invalid = false;
		node->p->receive(a);
		node->p->process(asn);
	}
}

void Graph::propagate() {
	//up(to providers)
	for (size_t a = 0; a < ranks.size(); a++) {
        for (auto r : ranks[a]) {
    		for (auto& [prefix, ann] : r->p->getRib()) {
                if (ann.relation == Relationship::CUSTOMER || ann.relation == Relationship::ORIGIN) {
                    for (auto prov : r->providers) {
                        Announcement new_a = ann;
                        new_a.next = r->asn;
                        new_a.relation = Relationship::CUSTOMER;
                        prov->p->receive(new_a);
                    }
                }
            }
        }
        for (auto r : ranks[a]) {
            r->p->process(r->asn);
        }
    }

    //across(to peers)
    for (auto& [n, aptr] : map) {
        for (auto& [prefix, ann] : aptr->p->getRib()) {
            if (ann.relation == Relationship::CUSTOMER || ann.relation == Relationship::ORIGIN) {
                for (auto peer : aptr->peers) {
                    Announcement new_a = ann;
                    new_a.next = aptr->asn;
                    new_a.relation = Relationship::PEER;
                    peer->p->receive(new_a);
                }
            }
        }
    }

	//process
    for (auto& [n, aptr] : map) {
        aptr->p->process(aptr->asn);
    }

    //down(to customers)
    for (int a = (int)ranks.size() - 1; a >= 0; a--) {
        for (auto r : ranks[a]) {
            for (auto& [prefix, ann] : r->p->getRib()) {
                for (auto cust : r->customers) {
                    Announcement new_a = ann;
                    new_a.next = r->asn;
                    new_a.relation = Relationship::PROVIDER;
                    cust->p->receive(new_a);
                }
            }
        }
        for (auto r : ranks[a]) {
            r->p->process(r->asn);
        }
    }
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
            		out << ")\"\n";
        	}
    	}
}

void Graph::processAll() {
    for (auto& [asn, as_ptr] : map) {
        as_ptr->p->process(asn);
    }
}
