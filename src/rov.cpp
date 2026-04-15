#include <iostream>
#include "announcement.hpp"
#include "rov.hpp"

void ROV::receive(const Announcement& a) {
    if (a.rov_invalid) {
        return; //drop announcement
    }
    BGP::receive(a);
}