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

bool BGP::process(int curr_asn) {
	bool changed = false;
	for (auto& [prefix, ann] : rec_queue) {
		Announcement best;
		bool found = false;
		for (auto& a : ann) {
			if(a.relation != Relationship::ORIGIN) {
				if (std::find(a.path.begin(), a.path.end(), curr_asn) != a.path.end()) {
                    			continue;
                		}
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
		if (!local_rib.count(prefix) || local_rib[prefix].path != best.path) {
            		local_rib[prefix] = best;
            		changed = true;
		}
	}
	rec_queue.clear();
	return changed;
}

const std::unordered_map<std::string, Announcement>& BGP::getRib() const {
	return local_rib;
}
