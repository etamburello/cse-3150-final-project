#include <iostream>
#include "graph.h"
#include "parser.h"

int main(){
	Graph g;
	Parser::parseFile("dataset/20260301.as-rel2.txt", g);

	if (g.hasProvCycle()) {
		std::cerr << "Provider cycle detected" << std::endl;
		return 1;
	}
	else if (g.hasCustomCycle()) {
		std::cerr << "Customer cycle detected" << std::endl;
		return 1;
	}

	std::cout << "Graph built successfully!" << std::endl;

	int d = 13335;
	g.computeRoutes(d);
	std::cout << "Routing paths to: " << d << std::endl;
	g.printPaths(d);

	return 0;
}
