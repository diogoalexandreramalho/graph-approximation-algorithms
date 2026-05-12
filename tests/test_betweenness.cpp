#include "graph_approx/betweenness.h"
#include "graph_approx/graph.h"

#include "fixtures.h"

#include <gtest/gtest.h>

#include <random>

using namespace graph_approx;

// ---- Brandes: deterministic, exact ----

TEST(Brandes, K4_all_zero) {
	auto bet = brandes(make_K4());
	for (double b : bet) EXPECT_DOUBLE_EQ(b, 0.0);
}

TEST(Brandes, path5_middle_has_highest) {
	auto bet = brandes(make_path(5));
	ASSERT_EQ(bet.size(), 5u);
	EXPECT_GT(bet[2], bet[1]);
	EXPECT_GT(bet[2], bet[3]);
	EXPECT_DOUBLE_EQ(bet[1], bet[3]);
	EXPECT_DOUBLE_EQ(bet[0], 0.0);
	EXPECT_DOUBLE_EQ(bet[4], 0.0);
}

// Brandes betweenness on path 0-1-2-3-4 with normalization /((N-1)(N-2)) = 1/12.
// Node 2 lies on 8 ordered shortest-path pairs (8/12 = 2/3); nodes 1 and 3 on
// 6 each (6/12 = 0.5); endpoints on none.
TEST(Brandes, path5_exact_values) {
	auto bet = brandes(make_path(5));
	ASSERT_EQ(bet.size(), 5u);
	EXPECT_DOUBLE_EQ(bet[0], 0.0);
	EXPECT_DOUBLE_EQ(bet[1], 0.5);
	EXPECT_NEAR(bet[2], 2.0 / 3.0, 1e-12);
	EXPECT_DOUBLE_EQ(bet[3], 0.5);
	EXPECT_DOUBLE_EQ(bet[4], 0.0);
}

// K4 has diameter 1; Riondato short-circuits and returns the all-zero vector.
TEST(Riondato, K4_all_zero) {
	auto g = make_K4();
	std::mt19937 rng(42);
	auto bet = riondato(g, 0.5, 0.5, 0.8, rng);
	for (double b : bet) EXPECT_DOUBLE_EQ(b, 0.0);
}
