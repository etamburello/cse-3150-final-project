#include <iostream>
#include "announcement.hpp"
#include "rov.hpp"

void ROV::receive(const Announcement& a) {
	if (a.relation == Relationship::ORIGIN) {
		BGP::receive(a);
		return;
	}
    	if (a.rov_invalid) {
        	return; //drop announcement
    	}
    	BGP::receive(a);
}
