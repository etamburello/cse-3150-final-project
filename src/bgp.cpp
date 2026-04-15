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
			if (ann.empty()) continue;
			bool found = false;
			Announcement best;
			for (auto& a : ann) {
				bool loop = false;
				for (int p : a.path) {
					if(p == curr_asn) {
						loop = true;
						break;
					}
				}
				if(loop) continue;

				if (!found || better(a, best)) {
					best = a;
					found = true;
				}
			}
			if (!found) continue;

			Announcement cand = best;
			cand.path.insert(cand.path.begin(), curr_asn);
			if (!local_rib.count(prefix) || better(cand, local_rib[prefix])) {
				local_rib[prefix] = cand;
			}
		}
		rec_queue.clear();
}

const std::unordered_map<std::string, Announcement>& BGP::getRib() const {
	return local_rib;
}
