#include "graph_approx/graph.h"
#include "graph_approx/mincut.h"

#include "fixtures.h"

#include <gtest/gtest.h>

#include <random>

using namespace graph_approx;

TEST(Karger, K3_min_cut_is_2) {
	auto g = make_triangle();
	std::mt19937 rng(42);
	EXPECT_EQ(karger(g, 20, rng), 2u);
}

TEST(Karger, K4_min_cut_is_3) {
	auto g = make_K4();
	std::mt19937 rng(42);
	EXPECT_EQ(karger(g, 20, rng), 3u);
}

TEST(Karger, bridge_min_cut_is_1) {
	auto g = make_bridge();
	std::mt19937 rng(42);
	EXPECT_EQ(karger(g, 50, rng), 1u);
}
