#include "graph_approx/clustering.h"
#include "graph_approx/graph.h"
#include <cassert>
#include <cstdlib>

using namespace std;

static int BinarySearch(int r, int AccWedgeCount[]) {
	int L = 0;
	int R = N-1;
	int guess;
	int res  = -1;
	int itvl = -1;

	while (L <= R)
	{
		guess = L + (R-L)/2;
		itvl  = guess;
		if (AccWedgeCount[guess] == r)
		{
			res = guess;
			R   = guess - 1;
		}
		else if (AccWedgeCount[guess] < r) { L = guess + 1; itvl++; }
		else if (AccWedgeCount[guess] > r) { R = guess - 1; itvl--; }
	}
	return res != -1 ? res: itvl;
}

long double Clustering() {
	long long int triangles=0;

	for (int i = 0; i < N; ++i)
		for (int j = i+1; j < N; ++j)
			for (int k = j+1; k < N; ++k)
				if (IsAdjacent(i,j) && IsAdjacent(j,k) && IsAdjacent(k,i))
					triangles++;
	return 6.0*triangles / (N*(N-1)*(N-2));
}

long double UniformWedge(int sample_sz) {
	int TW = 0;
	int AccWedgeCount[N];
	int degree;

	for (int i = 0; i < N; i++)
	{
		AccWedgeCount[i] = TW;
		degree = offset[i+1]-offset[i];
		TW += degree*(degree-1) / 2;
	}

	long long int sum = 0;
	int center, v1, v2, wedge;

	for (int i = 0; i < sample_sz; i++)
	{
		wedge = (rand() % (TW+1));
		center = BinarySearch(wedge, AccWedgeCount);

		degree = offset[center+1] - offset[center];
		if (degree<2) continue;

		v1, v2 = degree;
		do
		{
			v1 = rand() % degree;
			v2 = rand() % degree;
		} while (v1 == v2);

		v1 = neighbour[offset[center]+v1];
		v2 = neighbour[offset[center]+v2];
		sum += IsAdjacent(v1, v2) ? 1 : 0;
	}

	return sum / (3.0*sample_sz);
}

double ApproxClusteringNaive(int s) {
	assert(s>3);

	int u; int v1; int v2; int degree; int ct=0;
	for (int i = 0; i < s; i++)
	{
		u  = rand() % N;
		degree = offset[u+1]-offset[u];
		if (degree<2) continue;
		do
		{
			v1 = rand() % degree;
			v2 = rand() % degree;
		} while (v1 == v2);
		v1 = neighbour[offset[u]+v1];
		v2 = neighbour[offset[u]+v2];
		if (IsAdjacent(v1,v2)) ct++;
	}
	return 1.0*ct/s;
}
