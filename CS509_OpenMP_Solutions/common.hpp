// common.hpp - utility functions and sample graph for OpenMP assignments
// Provides a sample undirected weighted graph, a status printer, and thread count getter.
// Used by various assignment solutions.
#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <queue>
#include <cmath>
#include <limits>
#include <random>
#include <tuple>
#include <string>
#include <omp.h>
using namespace std;
using ll = long long;
// Example graph; all sample programs can be run without external test files.
// Adjacency-list entries are (neighbor, weight). For undirected graphs add both directions.
// Graph is an adjacency list where each entry is a vector of (neighbor, weight) pairs.
using Graph = vector<vector<pair<int, int>>>;
// Returns a sample undirected weighted graph for testing.
inline Graph sampleGraph()
{
    Graph g(6);
    auto edge = [&](int a, int b, int w)
    {g[a].push_back({b,w});g[b].push_back({a,w}); };
    edge(0, 1, 2);
    edge(0, 2, 6);
    edge(1, 2, 3);
    edge(1, 3, 1);
    edge(2, 3, 2);
    edge(3, 4, 4);
    edge(4, 5, 2);
    edge(3, 5, 7);
    return g;
}
// Prints verification result, timing, speedup and efficiency for the assignment.
inline void status(bool ok, double serial, double parallel, int threads)
{
    cout << "Verification: " << (ok ? "PASS" : "FAIL") << "\n";
    cout << "Serial: " << serial << " s, Parallel: " << parallel << " s\n";
    if (parallel > 0)
    {
        double s = serial / parallel;
        cout << "Speedup: " << s << " Efficiency: " << 100 * s / threads << "%\n";
    }
}
inline int getThreads(int argc, char **argv) { return argc > 1 ? max(1, atoi(argv[1])) : 4; }
