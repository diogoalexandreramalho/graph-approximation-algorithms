#include "graph_approx/generators.h"
#include "graph_approx/graph.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

static bool mysort(const pair<int,int> &a, const pair<int,int> &b) {
	if (a.first < b.first) return true;
	else if (a.first == b.first) return a.second < b.second;
	else return false;
}

void ER_Generator(double p) {
	srand (time(NULL));
	vector<int> neighbour;
	bool connect;
	int offset[N+1];
	int shift = 0;
	for (int i = 0; i < N; i++) {
		offset[i] = shift;
		for (int j = i+1; j < N; ++j) {
			connect = (rand() % 100) + 1 < (p*100);
			if (connect) {
				neighbour.push_back(j+1);
				shift++;
			}
		}
		//connect with the previous ones
		for (int k = 0; k < i; k++)
			for (int m = offset[k]; m < offset[k+1]; ++m)
				if (neighbour[m] == i+1) {
					neighbour.push_back(k+1);
					shift++;
				}
	}
	offset[N] = neighbour.size();
	for (int i = 0; i < (int)neighbour.size(); ++i) cout << neighbour[i] << " ";
	cout << "\n";
	for (int i = 0; i < N; ++i) cout << offset[i] << " ";
	cout << "\n";
}

void link_selection_model() {
	vector<pair<int,int> > edgelist;
	edgelist.push_back(pair<int,int>(0, 1));
	edgelist.push_back(pair<int,int>(1, 0));
	edgelist.push_back(pair<int,int>(1, 2));
	edgelist.push_back(pair<int,int>(2, 1));
	edgelist.push_back(pair<int,int>(2, 0));
	edgelist.push_back(pair<int,int>(0, 2));
	int size = edgelist.size();
	pair<int,int> random_edge;
	srand(time(NULL));

	int nodes = 103;

	for (int i=3; i<nodes; i++) {
		random_edge = edgelist[rand() % size++];
		edgelist.push_back(pair<int,int>(i, random_edge.first));
		edgelist.push_back(pair<int,int>(random_edge.first, i));
		edgelist.push_back(pair<int,int>(random_edge.second, i));
		edgelist.push_back(pair<int,int>(i, random_edge.second));
	}

	sort(edgelist.begin(), edgelist.end(), mysort);

	int* neighbour2 = (int*) malloc(sizeof(int)*edgelist.size());
	int* offset2    = (int*) malloc(sizeof(int)*(nodes+1));

	int u = 0; int v = 0;
	int previous = 0;
	int shift = 0;
	offset2[0] = 0;
	for (int i = 0; i < (int)edgelist.size(); i++) {
		u = edgelist[i].first;
		v = edgelist[i].second;
		if (u == previous) {
			neighbour2[i] = v;
			shift++;
		}
		else {
			neighbour2[i] = v;
			for (int j = previous; j < u; j++)
				offset2[j+1] = shift;
			previous = u;
			shift++;
		}
	}
	offset2[nodes] = edgelist.size();

	return;
}
