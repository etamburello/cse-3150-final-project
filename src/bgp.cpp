#include <iostream>
#include <unordered_map>
#include "bgp.hpp"
#include "announcement.hpp"

int rPriority(Relationship relation) {
	if (relation == Relationship::ORIGIN) {
			return 4;
	}
	else if (relation == Relationship::CUSTOMER) {
			return 3;
	}
	else if (relation == Relationship::PEER) {
			return 2;
	}
	else {
		return 1;
	}
}

bool BGP::better(const Announcement& a, const Announcement& b) {
	int a_priority = rPriority(a.relation);
	int b_priority = rPriority(b.relation);
	if (a_priority != b_priority) {
		return a_priority > b_priority;
	}

	if (a.path.size() != b.path.size()) {
		return a.path.size() < b.path.size();
	}

	return a.next < b.next;
}

void BGP::receive(const Announcement& a) {
	rec_queue[a.prefix].push_back(a);
}

void BGP::process(int curr_asn) {
		for (auto& [prefix, ann] : rec_queue) {
			//debuging
			std::cout << "Processing AS " << curr_asn << " queue size: " << rec_queue.size() << std::endl;

			Announcement best;
			bool found = false;
			for (auto& a : ann) {
				if(a.relation != Relationship::ORIGIN) {
					bool loop = false;
					for (size_t in = 1; in < a.path.size(); in++) {
    						if (a.path[in] == curr_asn) {
        						loop = true;
        						break;
    						}
					}
					if (loop) continue;
				}

    				if(!found) {
        				best = a;
        				found = true;
					continue;
    				}

				if(better(a, best)) {
					best = a;
				}
			}
			if(!found) continue;

			if(best.path.empty() || best.path[0] != curr_asn) {
				best.path.insert(best.path.begin(), curr_asn);
			}
			local_rib[prefix] = best;
			//debuging
			std::cout << "Installed route at AS " << curr_asn << " for prefix " << prefix << std::endl;
		}
		rec_queue.clear();
}

const std::unordered_map<std::string, Announcement>& BGP::getRib() const {
	return local_rib;
}
