#include <iostream>
#include "graph.hpp"

using std::cout, std::endl;

int main(){
	Graph g;
	cout << "Loading in file..." << endl;
	g.loadFile("bench/prefix/CAIDAASGraphCollector_2025.10.15.txt");

	cout << "Checking for cycles..." << endl;
	g.detectCycles();

	cout << "Loading ROV..." << endl;
	g.loadROV("bench/prefix/rov_asns.csv");

	cout << "Loading announcements..." << endl;
	g.seedAnnouncement("bench/prefix/anns.csv");

	cout << "Assigning proper ranks..." << endl;
	g.assignRanks();

	cout << "Propagating..." << endl;
	g.propagate();

	g.writeCSV("output.csv");
	cout << "Simulation successful, wrote to output.csv!" << endl;

	return 0;
}
