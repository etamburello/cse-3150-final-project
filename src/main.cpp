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

	cout << "Loading announcements..." << endl;
	g.loadAnnouncement("bench/subprefix/anns.csv");

	cout << "Propagating..." << endl;
	g.propagate();

	cout << "Loading ROV..." << endl;
	g.loadROV("bench/subprefix/rov_asns.csv");

	g.writeCSV("output.csv");
	cout << "Simulation successful, wrote to output.csv!" << endl;

	return 0;
}
