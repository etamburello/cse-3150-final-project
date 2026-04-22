#pragma once
#include <cassert>
#include <string>

#define ASSERT_TRUE(x) assert((x))
#define ASSERT_EQ(x, y) assert((x) == (y))

void test_pass(const std::string& name);

void test_graph_build();
void test_path_correctness();
void test_cycle_detection_true();
void test_cycle_detection_false();
void test_rank_assignment();
void test_announcement_seed();
void test_mult_prefixes();
void test_propagation_simple();
void test_peer_single_hop();
void test_bgp_decision();
void test_bgp_shortest_path();
void test_full_tiebreak();
void test_rov();
void test_rov_mixed();
