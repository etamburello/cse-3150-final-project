#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "bgp.hpp"
#include "announcement.hpp"

int rPriority(Relationship r) {
	if (r == Relationship::ORIGIN) {
			return 4;
	}
	else if (r == Relationship::CUSTOMER) {
			return 3;
	}
	else if (r == Relationship::PEER) {
			return 2;
	}
	else {
		return 1;
	}
}

bool BGP::better(const Announcement& a, const Announcement& b) {
	int a_priority = rPriority(a.received_from_relationship);
	int b_priority = rPriority(b.received_from_relationship);
	if (a_priority != b_priority) {
		return a_priority > b_priority;
	}

	if (a.path.size() != b.path.size()) {
		return a.path.size() < b.path.size();
	}

	return a.next < b.next;
}

static bool sameAnnouncement(const Announcement& a, const Announcement& b) {
	return a.prefix == b.prefix
		&& a.path == b.path
		&& a.next == b.next
		&& a.received_from_relationship == b.received_from_relationship
		&& a.rov_invalid == b.rov_invalid;
}

void BGP::receive(const Announcement& a) {
	rec_queue[a.prefix].push_back(a);
}

bool BGP::process(int curr_asn) {
	bool changed = false;
	for (auto& [prefix, ann] : rec_queue) {
		Announcement best{};
		bool found = false;
		auto existing = local_rib.find(prefix);
		if (existing != local_rib.end()) {
			best = existing->second;
			found = true;
		}
		for (auto& a : ann) {
			if(a.received_from_relationship != Relationship::ORIGIN) {
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
		auto it = local_rib.find(prefix);
		if (it == local_rib.end()) {
			local_rib.emplace(prefix, best);
			changed = true;
		} else if (!sameAnnouncement(it->second, best)) {
			it->second = best;
			changed = true;
		}
	}
	rec_queue.clear();
	return changed;
}

const std::unordered_map<std::string, Announcement>& BGP::getRib() const {
	return local_rib;
}
