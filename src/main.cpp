#include <iostream>
#include "graph.hpp"

using std::cout, std::endl;

int main(){
	Graph g;
	cout << "Loading in file..." << endl;
	g.loadFile("dataset/20260301.as-rel2.txt");

	cout << "Checking for cycles..." << endl;
	g.detectCycles();

	cout << "Assigning proper ranks..." << endl;
	g.assignRanks();

	cout << "Seeding announcement..." << endl;
	g.seedAnnouncement(1, "1.2.0.0/16"); //test value

	cout << "Propagating..." << endl;
	g.propagate();

	cout << "Simulation successful!" << endl;

	return 0;
}
