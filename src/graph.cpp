#include "../include/graph.h"
#include <algorithm>
#include <iostream>

using namespace std;

int N;
int M;
int* neighbour;
int* offset;

void printGraph(int *n, int *o) {
	for (int i = 0; i < M; ++i)   cout << n[i] << " ";
	cout << "\n";
	for (int i = 0; i < N+1; ++i) cout << o[i]    << " ";
	cout << "\n";
}

bool IsAdjacent(int u, int v) {
	int degree_u = offset[u+1] - offset[u];
	int degree_v = offset[v+1] - offset[v];
	if (degree_u > degree_v) swap(u,v);
	for (int i = offset[u]; i < offset[u+1]; i++) if (neighbour[i] == v) return true;
	return false;
}

bool IsAdjacentd(int u, int v) {
	for (int i = offset[u]; i < offset[u+1]; i++) if (neighbour[i] == v) return true;
	return false;
}
