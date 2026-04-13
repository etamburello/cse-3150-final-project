#pragma once
#include <string>
#include "graph.h"

class Parser {
public:
	static void parseFile(const std::string& filename, Graph& g);
};
