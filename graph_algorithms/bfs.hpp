#pragma once

#include "csr_graph.hpp"
#include <vector>
#include <queue>
#include <omp.h>

using namespace std;

// ============================================================================
// 1. SERIAL BREADTH-FIRST SEARCH (BFS)
// ============================================================================
inline vector<int> bfs_serial(const CSRGraph& g, int source) {
    int n = g.num_vertices;
    vector<int> dist(n, -1);
    queue<int> q;

    dist[source] = 0;
    q.push(source);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        int start = g.row_ptr[u];
        int end = g.row_ptr[u + 1];
        for (int i = start; i < end; i++) {
            int v = g.col_idx[i];
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }

    return dist;
}

// ============================================================================
// 2. PARALLEL BREADTH-FIRST SEARCH (BFS - Level by Level)
// ============================================================================
// In each step, we look at all vertices at the current distance 'd'
// and visit all their unvisited neighbors in parallel.
inline vector<int> bfs_parallel(const CSRGraph& g, int source, int threads) {
    int n = g.num_vertices;
    vector<int> dist(n, -1);
    dist[source] = 0;

    bool changed = true;
    for (int d = 0; changed; d++) {
        changed = false;

        // Check all vertices in parallel
        #pragma omp parallel for num_threads(threads) schedule(guided)
        for (int u = 0; u < n; u++) {
            // If vertex u was reached at the current level 'd'
            if (dist[u] == d) {
                int start = g.row_ptr[u];
                int end = g.row_ptr[u + 1];
                for (int i = start; i < end; i++) {
                    int v = g.col_idx[i];
                    // If neighbor v is not visited yet, set distance
                    if (dist[v] == -1) {
                        dist[v] = d + 1;
                        changed = true;
                    }
                }
            }
        }
    }

    return dist;
}
