#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "parser.h"

void Parser::parseFile(const std::string& filename, Graph& g) {
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Cannot open file\n";
		return;
	}

	std::string line;
	while (getline(file, line)) {
		if (line.empty() || line[0] == '#'){
			continue;
		}

		std::stringstream iss(line);
		int a, b, relationship;
		char delim;
		std::string leftovers;

		iss >> a >> delim >> b >> delim >> relationship >> delim >> leftovers;

		if (relationship == -1) {
			g.addProvCustom(a, b);
		}
		else if (relationship == 0) {
			g.addPeerRelation(a, b);
		}
	}
}

