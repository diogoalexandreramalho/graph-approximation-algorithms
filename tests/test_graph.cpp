#include "graph_approx/graph.h"

#include "fixtures.h"

#include <gtest/gtest.h>

#include <sstream>

using namespace graph_approx;

TEST(Graph, empty_graph_has_zero_degree) {
	auto g = make_empty(3);
	EXPECT_EQ(g.num_nodes(), 3);
	EXPECT_EQ(g.num_edges(), 0);
	for (int i = 0; i < 3; ++i) EXPECT_EQ(g.degree(i), 0);
	EXPECT_FALSE(g.is_adjacent(0, 1));
}

TEST(Graph, csr_loader_handles_isolated_nodes) {
	std::istringstream in("5 2\n1 2\n2 1\n");
	auto g = Graph::from_stream(in);

	EXPECT_EQ(g.num_nodes(), 5);
	EXPECT_EQ(g.num_edges(), 2);
	EXPECT_EQ(g.degree(0), 0);
	EXPECT_EQ(g.degree(1), 1);
	EXPECT_EQ(g.degree(2), 1);
	EXPECT_EQ(g.degree(3), 0);
	EXPECT_EQ(g.degree(4), 0);
	EXPECT_TRUE(g.is_adjacent(1, 2));
	EXPECT_FALSE(g.is_adjacent(0, 1));
}

TEST(Graph, csr_loader_text_txt_offsets_match) {
	// 8-node graph from AAva/algorithms/sets/text.txt
	std::istringstream in(
		"8 18\n"
		"0 1\n1 0\n1 2\n1 5\n2 1\n2 3\n2 5\n"
		"3 2\n3 4\n4 3\n4 5\n4 6\n5 1\n5 2\n5 4\n6 4\n6 7\n7 6\n");
	auto g = Graph::from_stream(in);

	EXPECT_EQ(g.num_nodes(), 8);
	EXPECT_EQ(g.num_edges(), 18);
	EXPECT_EQ(g.degree(0), 1);
	EXPECT_EQ(g.degree(1), 3);
	EXPECT_EQ(g.degree(2), 3);
	EXPECT_EQ(g.degree(5), 3);
	EXPECT_TRUE(g.is_adjacent(1, 5));
	EXPECT_TRUE(g.is_adjacent(4, 6));
	EXPECT_FALSE(g.is_adjacent(0, 3));
}

TEST(Graph, edges_returns_each_undirected_edge_once) {
	auto g = make_triangle();
	auto edges = g.edges();
	ASSERT_EQ(edges.size(), 3);
	EXPECT_EQ(edges[0], std::make_pair(0, 1));
	EXPECT_EQ(edges[1], std::make_pair(0, 2));
	EXPECT_EQ(edges[2], std::make_pair(1, 2));
}

TEST(Graph, neighbours_of_triangle) {
	auto g = make_triangle();
	auto n0 = g.neighbours_of(0);
	EXPECT_EQ(n0.size(), 2);
	EXPECT_EQ(n0[0], 1);
	EXPECT_EQ(n0[1], 2);
}
