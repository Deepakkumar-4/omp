#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <omp.h>

using namespace std;

// Compressed Sparse Row (CSR) Graph Representation
// row_ptr: where vertex u's neighbors start and end in col_idx
// col_idx: target neighbor vertices
// weights: edge weights
struct CSRGraph {
    int num_vertices;
    int num_edges;
    vector<int> row_ptr;
    vector<int> col_idx;
    vector<double> weights;

    CSRGraph(int n, int m) : num_vertices(n), num_edges(m),
                             row_ptr(n + 1, 0),
                             col_idx(m, 0),
                             weights(m, 1.0) {}

    int degree(int u) const {
        return row_ptr[u + 1] - row_ptr[u];
    }
};

// Simple beginner-friendly function to build CSR graph from an edge list
inline CSRGraph build_simple_graph(int n, const vector<pair<int, int>>& edge_list, const vector<double>& edge_weights) {
    int m = edge_list.size();
    CSRGraph g(n, m);

    // Step 1: Count out-degree of each vertex
    vector<int> deg(n, 0);
    for (int i = 0; i < m; i++) {
        deg[edge_list[i].first]++;
    }

    // Step 2: Prefix sum to calculate row_ptr
    g.row_ptr[0] = 0;
    for (int i = 0; i < n; i++) {
        g.row_ptr[i + 1] = g.row_ptr[i] + deg[i];
    }

    // Step 3: Fill neighbors and weights
    vector<int> current_pos = g.row_ptr;
    for (int i = 0; i < m; i++) {
        int u = edge_list[i].first;
        int slot = current_pos[u]++;
        g.col_idx[slot] = edge_list[i].second;
        g.weights[slot] = edge_weights[i];
    }

    return g;
}

// Generate a simple test graph (Erdos-Renyi random graph)
inline CSRGraph generate_simple_test_graph(int n, int avg_deg, int seed = 42) {
    srand(seed);
    vector<pair<int, int>> edges;
    vector<double> weights;

    for (int u = 0; u < n; u++) {
        for (int d = 0; d < avg_deg; d++) {
            int v = rand() % n;
            if (u != v) {
                edges.push_back({u, v});
                weights.push_back(1.0 + (rand() % 9));
            }
        }
    }

    return build_simple_graph(n, edges, weights);
}
