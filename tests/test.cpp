#include <iostream>
#include "test.hpp"

int main() {
	//graph building
	test_graph_build();
	test_path_correctness();

	//cycle detection
	test_cycle_detection_true();
	test_cycle_detection_false();

	//rank assignment
	test_rank_assignment();

	//announcement seeding
	test_announcement_seed();
	test_mult_prefixes();

	//propagation
	test_propagation_simple();
	test_peer_single_hop();

	//bgp behavior
	test_bgp_decision();
	test_bgp_shortest_path();
	test_full_tiebreak();

	//rov behavior
	test_rov();
	test_rov_mixed();
	return 0;
}
