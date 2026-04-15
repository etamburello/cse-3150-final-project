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

	cout << "Testing rov..." << endl;
	g.setROV(123); //test values
	g.setROV(456);

	cout << "Seeding announcement..." << endl;
	g.seedAnnouncement(15169, "8.8.8.0/24"); //test value

	cout << "Propagating..." << endl;
	g.propagate();

	cout << "Simulation successful!" << endl;

	return 0;
}
