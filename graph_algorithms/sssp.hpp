#pragma once

#include "csr_graph.hpp"
#include <vector>
#include <queue>
#include <cmath>
#include <omp.h>

using namespace std;

const double INF_DIST = 1e9;

// ============================================================================
// 1. SERIAL SSSP (Dijkstra's Algorithm)
// ============================================================================
inline vector<double> sssp_serial(const CSRGraph& g, int source) {
    int n = g.num_vertices;
    vector<double> dist(n, INF_DIST);

    // Min-heap priority queue: pair of (distance, vertex)
    using PDI = pair<double, int>;
    priority_queue<PDI, vector<PDI>, greater<PDI>> pq;

    dist[source] = 0.0;
    pq.push({0.0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;

        int start = g.row_ptr[u];
        int end = g.row_ptr[u + 1];
        for (int k = start; k < end; k++) {
            int v = g.col_idx[k];
            double w = g.weights[k];
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    return dist;
}

// ============================================================================
// 2. PARALLEL SSSP (Parallel Edge Relaxation / Bellman-Ford)
// ============================================================================
// In each step, every thread relaxes edges from known vertices in parallel.
inline vector<double> sssp_parallel(const CSRGraph& g, int source, int threads) {
    int n = g.num_vertices;
    vector<double> dist(n, INF_DIST);
    dist[source] = 0.0;

    bool changed = true;
    while (changed) {
        changed = false;

        #pragma omp parallel for num_threads(threads) schedule(guided)
        for (int u = 0; u < n; u++) {
            if (dist[u] >= INF_DIST) continue;

            int start = g.row_ptr[u];
            int end = g.row_ptr[u + 1];
            for (int k = start; k < end; k++) {
                int v = g.col_idx[k];
                double w = g.weights[k];
                double new_dist = dist[u] + w;

                if (new_dist < dist[v]) {
                    #pragma omp critical
                    {
                        if (new_dist < dist[v]) {
                            dist[v] = new_dist;
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    return dist;
}
