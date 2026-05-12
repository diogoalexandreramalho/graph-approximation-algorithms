#include "graph_approx/clustering.h"
#include "graph_approx/graph.h"

#include "fixtures.h"

#include <gtest/gtest.h>

using namespace graph_approx;

// Exact clustering coefficient — values from 6*T / (N*(N-1)*(N-2)).

TEST(Clustering, K3_exact_is_one) {
	EXPECT_DOUBLE_EQ(static_cast<double>(clustering(make_triangle())), 1.0);
}

TEST(Clustering, K4_exact_is_one) {
	EXPECT_DOUBLE_EQ(static_cast<double>(clustering(make_K4())), 1.0);
}

TEST(Clustering, C4_exact_is_zero) {
	EXPECT_DOUBLE_EQ(static_cast<double>(clustering(make_C4())), 0.0);
}

TEST(Clustering, bowtie_exact_is_point_two) {
	EXPECT_DOUBLE_EQ(static_cast<double>(clustering(make_bowtie())), 0.2);
}
