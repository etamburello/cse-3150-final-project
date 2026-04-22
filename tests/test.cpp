#include <iostream>
#include "test.hpp"
#include "graph.hpp"
#include "announcement.hpp"

void test_pass(const std::string& name) {
	std::cout << "[PASS] " << name << std::endl;
}

//make + manual edges(same adjacency model used in loadFile)
void test_graph_build() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);
	auto c = g.make(3);

	//a is provider of b
	a->customers.push_back(b.get());
	b->providers.push_back(a.get());
	ASSERT_EQ(a->customers.size(), 1u);
	ASSERT_EQ(b->providers.size(), 1u);
	ASSERT_EQ(a->customers[0]->asn, 2);
	ASSERT_EQ(b->providers[0]->asn, 1);

	//a and c are peers
	a->peers.push_back(c.get());
	c->peers.push_back(a.get());
	ASSERT_EQ(a->peers.size(), 1u);
	ASSERT_EQ(c->peers.size(), 1u);
	ASSERT_EQ(a->peers[0]->asn, 3);
	ASSERT_EQ(c->peers[0]->asn, 1);

	test_pass("graph building checked");
}

//full proper propagation test: ranks + three-phase
void test_path_correctness() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);
	auto c = g.make(3);

	b->providers.push_back(a.get());
	a->customers.push_back(b.get());
	c->providers.push_back(b.get());
	b->customers.push_back(c.get());

	g.assignRanks();

	Announcement ann;
	ann.prefix = "1.2.0.0/16";
	ann.path = {3};
	ann.next = 3;
	ann.received_from_relationship = Relationship::ORIGIN;

	c->p->receive(ann);
	c->p->process(3);

	g.propagate();

	auto rib = a->p->getRib();
	auto path = rib.at("1.2.0.0/16").path;

	ASSERT_EQ(path[0], 1);
	ASSERT_EQ(path[1], 2);
	ASSERT_EQ(path[2], 3);

	test_pass("Full Path Correctness");
}

//cycle detection testing
void test_cycle_detection_true() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);

	a->providers.push_back(b.get());
	b->providers.push_back(a.get());

	ASSERT_TRUE(g.detectCycles());

	test_pass("cycle for detection check");
}

void test_cycle_detection_false() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);
	auto c = g.make(3);

	b->providers.push_back(a.get());
	a->customers.push_back(b.get());
	c->providers.push_back(b.get());
	b->customers.push_back(c.get());

	ASSERT_TRUE(!g.detectCycles());

	test_pass("no cycle for detection check");
}

//rank assignment testing 
void test_rank_assignment() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);
	auto c = g.make(3);

	c->providers.push_back(b.get());
	b->customers.push_back(c.get());
	b->providers.push_back(a.get());
	a->customers.push_back(b.get());

	g.assignRanks();

	ASSERT_EQ(c->rank, 0);
	ASSERT_EQ(b->rank, 1);
	ASSERT_EQ(a->rank, 2);

	test_pass("rank assignment check");
}

//receive + process + annoucement testing 
void test_announcement_seed() {
	Graph g;
	auto a = g.make(1);

	Announcement ann;
	ann.prefix = "1.2.0.0/16";
	ann.path = {1};
	ann.next = 1;
	ann.received_from_relationship = Relationship::ORIGIN;

	a->p->receive(ann);
	a->p->process(1);

	auto rib = a->p->getRib();

	ASSERT_TRUE(rib.count("1.2.0.0/16") != 0u);
	ASSERT_EQ(rib.at("1.2.0.0/16").path.size(), 1u);

	test_pass("announcement seeding check");
}

void test_mult_prefixes() {
	Graph g;
	auto a = g.make(1);

	Announcement p1;
	p1.prefix = "1.2.0.0/16";
	p1.path = {1};
	p1.next = 1;
	p1.received_from_relationship = Relationship::ORIGIN;

	Announcement p2 = p1;
	p2.prefix = "2.3.0.0/16";

	a->p->receive(p1);
	a->p->receive(p2);
	a->p->process(1);

	auto rib = a->p->getRib();

	ASSERT_TRUE(rib.count("1.2.0.0/16") != 0u);
	ASSERT_TRUE(rib.count("2.3.0.0/16") != 0u);

	test_pass("multiple prefix independence check");
}

//more propagate using up/across/down ordering from graph
void test_propagation_simple() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);
	auto c = g.make(3);

	a->customers.push_back(b.get());
	b->providers.push_back(a.get());
	b->customers.push_back(c.get());
	c->providers.push_back(b.get());

	g.assignRanks();

	Announcement ann;
	ann.prefix = "1.2.0.0/16";
	ann.path = {3};
	ann.next = 3;
	ann.received_from_relationship = Relationship::ORIGIN;

	c->p->receive(ann);
	c->p->process(3);

	g.propagate();

	auto rib = a->p->getRib();

	ASSERT_TRUE(rib.count("1.2.0.0/16") != 0u);

	auto path = rib.at("1.2.0.0/16").path;

	ASSERT_EQ(path.size(), 3u);
	ASSERT_EQ(path[0], 1);

	test_pass("simple chain propagation check");
}

void test_peer_single_hop() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);
	auto c = g.make(3);

	a->peers.push_back(b.get());
	b->peers.push_back(a.get());
	b->peers.push_back(c.get());
	c->peers.push_back(b.get());

	g.assignRanks();

	Announcement ann;
	ann.prefix = "1.2.0.0/16";
	ann.path = {1};
	ann.next = 1;
	ann.received_from_relationship = Relationship::ORIGIN;

	a->p->receive(ann);
	a->p->process(1);

	g.propagate();

	auto rib_c = c->p->getRib();

	ASSERT_TRUE(rib_c.count("1.2.0.0/16") == 0u);

	test_pass("peer single-hop constraint check");
}

//BGP decision testing
void test_bgp_decision() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);
	auto c = g.make(3);

	a->customers.push_back(b.get());
	b->providers.push_back(a.get());
	a->peers.push_back(c.get());
	c->peers.push_back(a.get());

	Announcement from_b;
	from_b.prefix = "1.2.0.0/16";
	from_b.path = {2};
	from_b.next = 2;
	from_b.received_from_relationship = Relationship::CUSTOMER;

	Announcement from_c;
	from_c.prefix = "1.2.0.0/16";
	from_c.path = {3};
	from_c.next = 3;
	from_c.received_from_relationship = Relationship::PEER;

	a->p->receive(from_c);
	a->p->receive(from_b);
	a->p->process(1);

	auto rib = a->p->getRib();

	ASSERT_EQ(rib.at("1.2.0.0/16").next, 2);

	test_pass("BGP behavior check");
}

void test_bgp_shortest_path() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);
	auto c = g.make(3);
	auto d = g.make(4);

	b->providers.push_back(a.get());
	a->customers.push_back(b.get());
	c->providers.push_back(a.get());
	a->customers.push_back(c.get());
	d->providers.push_back(b.get());
	b->customers.push_back(d.get());
	d->providers.push_back(c.get());
	c->customers.push_back(d.get());

	g.assignRanks();

	Announcement ann;
	ann.prefix = "1.2.0.0/16";
	ann.path = {4};
	ann.next = 4;
	ann.received_from_relationship = Relationship::ORIGIN;

	d->p->receive(ann);
	d->p->process(4);

	g.propagate();

	auto rib = a->p->getRib();

	ASSERT_TRUE(rib.count("1.2.0.0/16") != 0u);

	test_pass("bgp shortest path check");
}

void test_full_tiebreak() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);
	auto c = g.make(3);

	a->customers.push_back(b.get());
	b->providers.push_back(a.get());

	a->customers.push_back(c.get());
	c->providers.push_back(a.get());

	Announcement from_b;
	from_b.prefix = "1.2.0.0/16";
	from_b.path = {2};
	from_b.next = 2;
	from_b.received_from_relationship = Relationship::CUSTOMER;

	Announcement from_c;
	from_c.prefix = "1.2.0.0/16";
	from_c.path = {3};
	from_c.next = 3;
	from_c.received_from_relationship = Relationship::CUSTOMER;

	a->p->receive(from_c);
	a->p->receive(from_b);
	a->p->process(1);

	auto rib = a->p->getRib();

	ASSERT_EQ(rib.at("1.2.0.0/16").next, 2);

	test_pass("tie-break (next hop) check");
}

//ROV behavior testing
void test_rov() {
	Graph g;
	auto a = g.make(1);

	g.setROV(1);

	Announcement bad;
	bad.prefix = "1.2.0.0/16";
	bad.path = {2};
	bad.next = 2;
	bad.received_from_relationship = Relationship::CUSTOMER;
	bad.rov_invalid = true;

	a->p->receive(bad);
	a->p->process(1);

	auto rib = a->p->getRib();

	ASSERT_TRUE(rib.empty());

	test_pass("ROV behavior check");
}

void test_rov_mixed() {
	Graph g;
	auto a = g.make(1);
	auto b = g.make(2);

	a->customers.push_back(b.get());
	b->providers.push_back(a.get());

	g.setROV(1);

	Announcement good;
	good.prefix = "1.2.0.0/16";
	good.path = {2};
	good.next = 2;
	good.received_from_relationship = Relationship::CUSTOMER;
	good.rov_invalid = false;

	Announcement bad = good;
	bad.rov_invalid = true;

	a->p->receive(bad);
	a->p->receive(good);
	a->p->process(1);

	auto rib = a->p->getRib();

	ASSERT_TRUE(rib.count("1.2.0.0/16") != 0u);
	ASSERT_EQ(rib.at("1.2.0.0/16").next, 2);

	test_pass("ROV mixed handling check");
}

int main() {
	test_graph_build();
	test_path_correctness();

	test_cycle_detection_true();
	test_cycle_detection_false();

	test_rank_assignment();

	test_announcement_seed();
	test_mult_prefixes();

	test_propagation_simple();
	test_peer_single_hop();

	test_bgp_decision();
	test_bgp_shortest_path();
	test_full_tiebreak();

	test_rov();
	test_rov_mixed();
	return 0;
}
