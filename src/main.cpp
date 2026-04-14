#include <iostream>
#include "../include/graph.hpp"

using std::cout, std::endl;

int main(){
	Graph g;
	cout << "Loading in file..." << endl;
	g.loadFile("dataset/20260301.as-rel2.txt");

	cout << "Checking for cycles..." << endl;
	g.detectCycles();

	cout << "Assigning proper ranks..." << endl;
	g.assignRanks();

	cout << "Seeding annoucement..." << endl;
	g.seedAnnoucement(1, "1.2.0.0/16"); //test value

	cout << "Propagating..." << endl;
	g.propagate();

	cout << "Simulation successful!" << endl;

	return 0;
}
