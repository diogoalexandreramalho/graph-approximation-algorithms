#include "../include/betweenness.h"
#include "../include/graph.h"
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <random>
#include <stack>
#include <utility>
#include <vector>

using namespace std;

static pair< vector<vector <int> >, vector<int> > AllShortestPaths(int source, int target) {
	queue<int> q; q.push(source);

	vector<int>  sigma_u(N,0);      sigma_u[source]  = 1;
	vector<int>  distance(N,-1);    distance[source] = 0;
	vector<bool> enqueued(N,false); enqueued[source] = true;

	vector<vector<int> > predecessors(N);

	int neighb;

	while (q.front() != target)
	{
		source = q.front(); q.pop();
		enqueued[source] = false;

		for (int i = offset[source]; i < offset[source+1]; i++)
		{
			neighb = neighbour[i];
			if (distance[neighb] == -1) distance[neighb] = distance[source] + 1;
			if (distance[neighb] == distance[source] + 1)
			{
				predecessors[neighb].push_back(source);
				if (!enqueued[neighb])
				{
					q.push(neighb);
					enqueued[neighb] = true;
				}
				sigma_u[neighb] += sigma_u[source];
			}
		}
	}
	pair< vector<vector <int> >, vector<int> > x(predecessors, sigma_u);
	return x;
}

vector<double> Brandes() {
	vector<double> c(N,0);
	for(int s = 0; s < N; s++)
	{
		queue<int> Q;
		stack<int> S;
		vector<int> distance(N,-1);    distance[s] = 0;
		vector<vector<int> > predecessors(N);
		vector<double> sigma(N,0); sigma[s] = 1;

		Q.push(s);
		while(!Q.empty())
		{
			int v = Q.front(); Q.pop();
			S.push(v);
			for (int i = offset[v]; i < offset[v+1]; i++)
			{
				int w = neighbour[i];
				if (distance[w] < 0)
				{
					Q.push(w);
					distance[w] = distance[v] + 1;
				}
				if (distance[w] == distance[v] + 1)
				{
					sigma[w] = sigma[w] + sigma[v];
					predecessors[w].push_back(v);
				}
			}
		}

		vector<double> delta(N,0);
		while(!S.empty())
		{
			int w = S.top(); S.pop();
			for(int i=0; i < (int)predecessors[w].size(); i++)
			{
				int v = predecessors[w][i];
				delta[v] = delta[v] + sigma[v] / sigma[w] * (1 + delta[w]);
			}
			if (w != s)c[w] = c[w] + delta[w];
		}
	}

	for(int i=0; i < N; i++)
		c[i] = c[i] / ((N-1)*(N-2)); // normalization for undirected graph

	return c;
}

int VertexDiameter2Approx() {
	queue<int> q;
	srand(time(NULL));
	int v = rand() % N;
	q.push(v);
	vector<bool> visited(N,false); visited[v] = true;
	vector<int>  distance(N,0);
	int source;  int neighb;
	int s = 0;   int s1 = 0; int s2 = 0;
	while (!q.empty())
	{
		source = q.front(); q.pop();
		for (int i = offset[source]; i < offset[source+1]; i++)
		{
			neighb = neighbour[i];
			if (!visited[neighb])
			{
				s = distance[neighb] = distance[source] + 1;
				visited[neighb] = true;
				q.push(neighb);

				if (s > s1) { s2 = s1; s1 = s; }
				else if (s > s2) s2 = s;
			}
		}
	}
	return s1+s2;
}

vector<double> Riondato(double c, double eps, double delta) {
	assert(c<=1 && c>=0 && eps<=1 && eps>=0);

	unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator (seed);
	srand (time(NULL));
	c     = 0.5;
	eps   = 0.5;
	delta = 0.8;
	vector<double> betweenness(N,0);
	int source = 0;
	int target = 0;
	int diameter = VertexDiameter2Approx();
	if (diameter < 2) return betweenness;
	double r 	 = (c / (eps*eps)) * ( floor(log2(diameter-2)) + log(1/delta));


	for (int i = 0; i < r; i++)
	{
		do
		{
			source = rand() % N;
			target = rand() % N;
		} while (source == target);

		pair< vector<vector <int> >, vector<int> > x = AllShortestPaths(source,target);
		vector<vector <int> > predecessors = x.first;
		vector<int> 		  sigma_u      = x.second;

		int t = target; int parents = 0; int z = 0;
		while (t != source)
		{
			parents = predecessors[t].size();

			vector<double> a(parents);
			for (int i = 0; i < parents; i++) a[i] = 1.0*sigma_u[predecessors[t][i]]/sigma_u[t];
			discrete_distribution<int> distribution(a.begin(),a.end());
			z = distribution(generator);
			z = predecessors[t][z];

			if (z != source) betweenness[z] = betweenness[z] + 1.0/r;

			t = z;
		}
	}

	return betweenness;
}
