#include <iostream>
#include <unordered_map>
#include "bgp.hpp"
#include "annoucement.hpp"

int rPriority(Relationship relation) {
	if (relation == Relationship::ORIGIN) {
			return 4;
	}
	else if (relation == Relationship::CUSTOMER) {
			return 3;
	}
	else if (relation == Relationship::PROVIDER) {
			return 1;
	}
	else {
		return 2;
	}
}

bool BGP::better(const Annoucement& a, const Annoucement& b) {
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

void BGP::receive(const Annoucement& a) {
	rec_queue[a.prefix].push_back(a);
}

void BGP::process(int curr_asn) {
		for (auto& [prefix, ann] : rec_queue) {
				Annoucement best = ann[0];
				for (auto& a : ann) {
					if (better(a, best)) {
							best = a;
					}
				}

				best.path.insert(best.path.begin(), curr_asn);
				local_rib[prefix] = best;
		}
		rec_queue.clear();
}

const std::unordered_map<std::string, Annoucement>& BGP:: getRib() const {
	return local_rib;
}
