#include "graph_approx/apl.h"
#include "graph_approx/graph.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

static bool flag = true;

static void printVec(std::vector< std::vector<unsigned long int> >& vec) {
	for (int i = 0; i < (int)vec.size(); i++) {
		cout << "Vertex = " << i << " ";
		for (int j = 0; j < (int)vec[i].size(); j++) {
			cout << vec[i][j];
		}
		cout << "\n";
	}
	cout << "\n";
}

static void printNeighbourhoods(int max_distance, vector<unsigned long int>& N_Function){
	cout << "\n";
	for (int i = 0; i < max_distance; i++)
		cout << "N(" << i << ") = " << N_Function[i] << "\n";
}

static long jenkins(long x, long seed) {
	unsigned long a, b, c;

	a = seed + x;
	b = seed;
	c = 0x9e3779b97f4a7c13L; //the golden ratio; an arbitrary value

	a -= b; a -= c; a ^= (c >> 43);
	b -= c; b -= a; b ^= (a << 9);
	c -= a; c -= b; c ^= (b >> 8);
	a -= b; a -= c; a ^= (c >> 38);
	b -= c; b -= a; b ^= (a << 23);
	c -= a; c -= b; c ^= (b >> 5);
	a -= b; a -= c; a ^= (c >> 35);
	b -= c; b -= a; b ^= (a << 49);
	c -= a; c -= b; c ^= (b >> 11);
	a -= b; a -= c; a ^= (c >> 12);
	b -= c; b -= a; b ^= (a << 18);
	c -= a; c -= b; c ^= (b >> 22);

	return c;
}

static int countLeadingZeros(int64_t x) {
	int total_bits = sizeof(x) * 8;

	int res = 0;
	while ( !(x & (static_cast<uint64_t>(1) << (total_bits - 1))) ) {
		x = (x << 1);
		res++;
	}
	return res;
}

static double computeAlpha(uint64_t m) {
	double alpha;
	switch (m) {
		case 16:
			alpha = 0.673;
			break;
		case 32:
			alpha = 0.697;
			break;
		case 64:
			alpha = 0.709;
			break;
		default:
			alpha = 0.7213 / (1.0 + 1.079 / m);
			break;
	}
	return alpha;
}

static void updateCounter(std::vector< std::vector<int> >& c, std::vector< std::vector<int> >& temp) {
	for(int i = 0; i < (int)temp.size(); i++){
		int v = temp[i][0];
		int n = temp[i][1];
		int value = temp[i][2];
		if (value > c[v][n]){
			c[v][n] = value;
		}
	}
}

static void add(std::vector< std::vector<int> >& c, long v, int b, long seed) {
	int64_t x = jenkins(v, seed);
	int j = static_cast<uint64_t>(x) >> (64-b);
	int w = countLeadingZeros(x << b) + 1;

	c[v][j] = max(w,c[v][j]);
}

static uint64_t size(vector< vector<int> >& c, uint64_t m, double alpha, long v) {
	long double denominator = 0;
	for(uint64_t i=0; i<m; i++){
		denominator += pow(2, - c[v][i]);
	}
	long double z = 1/denominator;
	uint64_t e = alpha * pow(m,2) * z;
	return e;
}

static void Union(std::vector< std::vector<int> >& c,  long u, long v, uint64_t m, std::vector< std::vector<int> >& temp) {
	vector<int> entry;
	for(int i=0; i<(int)m; i++){
		if(c[v][i] > c[u][i]){
			flag = true;
			entry.push_back(u);
			entry.push_back(i);
			entry.push_back(c[v][i]);
			temp.push_back(entry);
		}
	}
}

static void computeNeighbourhoodFunction(vector< vector<int> >& c, vector<unsigned long int>& N_Function, uint64_t m, double alpha) {
	uint64_t size_node;
	uint64_t total_size = 0;
	for(uint64_t v = 0; v < (uint64_t)N; v++) {
		size_node = size(c, m, alpha, v);
		total_size += size_node;
	}
	N_Function.push_back(total_size);
}

static long double computeAverageDistance(int max_distance, vector<unsigned long int>& N_Function) {
	long double average_distance = 0.0;
	for (int i = 0; i < max_distance-1; i++){
		unsigned long int N_difference = N_Function[i+1] - N_Function[i];
		average_distance += N_difference * (i+1);
	}

	average_distance /= N_Function[max_distance-1];
	return average_distance;
}

long double APL(int b) {
	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	uint64_t m = pow(2,b);

	flag = true;
	double alpha = computeAlpha(m);
	vector<vector<int> > c( N, vector<int> (m, 0));

	// initialize counters with the nodes id
	for(uint64_t v = 0; v < (uint64_t)N; v++) {
		add(c, v, b, seed);
	}

	vector<unsigned long int> N_Function;
	vector<vector<int> > temp;
	int t = 0;
	while(flag) {
		flag = false;
		temp.clear();
		vector<vector<int> >(temp).swap(temp);

		// do the Union for each edge in the graph
		for(uint64_t v = 0; v < (uint64_t)N; v++){
			vector<int> a = c[v];
			for(int j = offset[v]; j < offset[v+1]; j++){
				int w = neighbour[j];
				Union(c, v, w, m, temp);						// B(v, t+1)
			}
		}

		updateCounter(c, temp);
		computeNeighbourhoodFunction(c, N_Function, m, alpha);
		t += 1;
	}

	int max_distance = N_Function.size();
	//printNeighbourhoods(max_distance, N_Function);
	long double avg_distance = computeAverageDistance(max_distance, N_Function);

	return avg_distance;
}
