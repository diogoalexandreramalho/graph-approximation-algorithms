#pragma once

extern int N;
extern int M;
extern int* neighbour;
extern int* offset;

void printGraph(int* n, int* o);
bool IsAdjacent(int u, int v);
bool IsAdjacentd(int u, int v);
