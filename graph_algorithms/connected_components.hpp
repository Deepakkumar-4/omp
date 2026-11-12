#pragma once

#include "csr_graph.hpp"
#include <vector>
#include <queue>
#include <algorithm>
#include <omp.h>

using namespace std;

// ============================================================================
// 1. SERIAL CONNECTED COMPONENTS (Using standard BFS)
// ============================================================================
inline vector<int> connected_components_serial(const CSRGraph& g) {
    int n = g.num_vertices;
    vector<int> comp(n, -1);
    queue<int> q;

    for (int i = 0; i < n; i++) {
        if (comp[i] != -1) continue;

        // Start a new component with ID = i
        comp[i] = i;
        q.push(i);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            int start = g.row_ptr[u];
            int end = g.row_ptr[u + 1];
            for (int k = start; k < end; k++) {
                int v = g.col_idx[k];
                if (comp[v] == -1) {
                    comp[v] = i;
                    q.push(v);
                }
            }
        }
    }

    return comp;
}

// ============================================================================
// 2. PARALLEL CONNECTED COMPONENTS (Parallel Label Propagation)
// ============================================================================
// 1. Each vertex starts with its own ID: comp[i] = i.
// 2. Each edge (u, v) tries to spread the smaller component ID to its neighbor.
// 3. Repeat until no IDs change.
inline vector<int> connected_components_parallel(const CSRGraph& g, int threads) {
    int n = g.num_vertices;
    vector<int> comp(n);

    // Initialize in parallel: each node is its own component
    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < n; i++) {
        comp[i] = i;
    }

    bool changed = true;
    while (changed) {
        changed = false;

        // Spread smaller component IDs in parallel
        #pragma omp parallel for num_threads(threads) schedule(guided)
        for (int u = 0; u < n; u++) {
            int start = g.row_ptr[u];
            int end = g.row_ptr[u + 1];

            for (int k = start; k < end; k++) {
                int v = g.col_idx[k];
                if (comp[u] < comp[v]) {
                    #pragma omp critical
                    {
                        if (comp[u] < comp[v]) {
                            comp[v] = comp[u];
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    return comp;
}

// Count number of unique connected components
inline int count_components(const vector<int>& comp) {
    vector<int> unique_ids = comp;
    sort(unique_ids.begin(), unique_ids.end());
    auto it = unique(unique_ids.begin(), unique_ids.end());
    return distance(unique_ids.begin(), it);
}
