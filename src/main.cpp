#include <iostream>
#include "graph.hpp"

using std::cout, std::endl;

int main(){
	Graph g;
	cout << "Loading in file..." << endl;
	g.loadFile("dataset/20260301.as-rel2.txt");

	cout << "Checking for cycles..." << endl;
	g.detectCycles();

	cout << "Loading ROV..." << endl;
	g.loadROV("bench/subprefix/rov_asns.csv");

	cout << "Loading announcements..." << endl;
	g.seedAnnouncement("bench/subprefix/anns.csv");

	cout << "Assigning proper ranks..." << endl;
	g.assignRanks();

	cout << "Propagating..." << endl;
	g.propagate();

	g.writeCSV("output.csv");
	cout << "Simulation successful, wrote to output.csv!" << endl;

	return 0;
}
