#pragma once

#include "csr_graph.hpp"
#include <vector>
#include <cmath>
#include <omp.h>

using namespace std;

// ============================================================================
// 1. SERIAL PAGERANK (Beginner Friendly)
// ============================================================================
inline vector<double> pagerank_serial(const CSRGraph& g, double damping = 0.85, int iters = 20) {
    int n = g.num_vertices;
    vector<double> pr(n, 1.0 / n);
    vector<double> next_pr(n, 0.0);

    double base_score = (1.0 - damping) / n;

    for (int step = 0; step < iters; step++) {
        for (int u = 0; u < n; u++) {
            double sum = 0.0;
            int start = g.row_ptr[u];
            int end = g.row_ptr[u + 1];

            for (int k = start; k < end; k++) {
                int v = g.col_idx[k];
                int deg_v = g.degree(v);
                if (deg_v > 0) {
                    sum += pr[v] / deg_v;
                }
            }
            next_pr[u] = base_score + damping * sum;
        }
        pr = next_pr;
    }

    return pr;
}

// ============================================================================
// 2. PARALLEL PAGERANK (OpenMP Beginner Friendly)
// ============================================================================
inline vector<double> pagerank_parallel(const CSRGraph& g, double damping = 0.85, int iters = 20, int threads = 4) {
    int n = g.num_vertices;
    vector<double> pr(n, 1.0 / n);
    vector<double> next_pr(n, 0.0);

    double base_score = (1.0 - damping) / n;

    for (int step = 0; step < iters; step++) {
        // Parallelize over all vertices: each thread calculates PageRank for different vertices
        #pragma omp parallel for num_threads(threads) schedule(guided)
        for (int u = 0; u < n; u++) {
            double sum = 0.0;
            int start = g.row_ptr[u];
            int end = g.row_ptr[u + 1];

            for (int k = start; k < end; k++) {
                int v = g.col_idx[k];
                int deg_v = g.degree(v);
                if (deg_v > 0) {
                    sum += pr[v] / deg_v;
                }
            }
            next_pr[u] = base_score + damping * sum;
        }
        pr = next_pr;
    }

    return pr;
}
