#include <iostream>
#include "AS.h"

AS::AS (int id) : asn(id), distance(INT_MAX), parent(nullptr), relation(0) {}
