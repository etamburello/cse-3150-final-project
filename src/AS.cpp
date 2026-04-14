#include <iostream>
#include <memory>
#include "AS.hpp"
#include "bgp.hpp"

AS::AS (int id) : asn(id) {
    p = std::make_unique<BGP>();
}
