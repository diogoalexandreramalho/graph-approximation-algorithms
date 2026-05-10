#include "../include/apl.h"
#include "../include/betweenness.h"
#include "../include/clustering.h"
#include "../include/graph.h"

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

int main() {
	/* N: number of nodes   ||   M: 2 * number of links */
	cin >> N >> M;

	neighbour = (int*) malloc(sizeof(int)*M);
	offset    = (int*) malloc(sizeof(int)*(N+1));

	int u = 0; int v = 0;
	int previous = 0;
	int shift = 0;
	offset[0] = 0;
	for (int i = 0; i < M; i++) {
		cin >> u >> v;
		if (u == previous) {
			neighbour[i] = v;
			shift++;
		}
		else {
			neighbour[i] = v;
			for (int j = previous; j < u; j++)
				offset[j+1] = shift;
			previous = u;
			shift++;
		}
	}
	offset[N] = M;

	cout << "GRAPH IN CSR FORMAT\n";
	printGraph(neighbour, offset);
	cout << "\n";

	cout << "CLUSTERING COEFFICIENT\n";
	cout << Clustering()    << endl;
	cout << UniformWedge(3) << endl;
	cout << ApproxClusteringNaive(4) << endl;
	cout << "\n";

	cout << "BETWEENNESS CENTRALITY\n";
	vector<double> betweenness_ = Riondato(0,0,0);
	for (int i = 0; i < (int)betweenness_.size(); i++) {
		cout << "Node " << i << " -> " << betweenness_[i] << endl;
	}
	cout << "\n";

	cout << "AVERAGE PATH LENGTH\n";
	cout << APL(4) << endl;
	cout << "\n";

	cout << "END\n";
}
