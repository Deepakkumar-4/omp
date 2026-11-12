#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <omp.h>

using namespace std;

// An edge in COO (Coordinate) format: (source row, target column, value)
struct Edge {
    int u;      // source vertex (row)
    int v;      // destination vertex (column)
    double w;   // weight (value)
};

// Compressed Sparse Row (CSR) representation:
// 1. row_ptr: where each row starts and ends in col_idx/values
// 2. col_idx: the column of each non-zero element
// 3. values:  the actual numerical value/weight
struct CSR {
    int n;                      // Number of rows/vertices
    int nnz;                    // Number of non-zero entries (edges)
    vector<int> row_ptr;        // Size: n + 1
    vector<int> col_idx;        // Size: nnz
    vector<double> values;      // Size: nnz

    CSR(int rows, int edges) : n(rows), nnz(edges),
                               row_ptr(rows + 1, 0),
                               col_idx(edges, 0),
                               values(edges, 0.0) {}
};

// Helper function to sort column indices within each row
inline void sort_row_elements(CSR& csr, int start, int end) {
    if (end - start <= 1) return;
    vector<pair<int, double>> row_items;
    for (int k = start; k < end; k++) {
        row_items.push_back({csr.col_idx[k], csr.values[k]});
    }
    sort(row_items.begin(), row_items.end());
    for (int k = 0; k < (int)row_items.size(); k++) {
        csr.col_idx[start + k] = row_items[k].first;
        csr.values[start + k] = row_items[k].second;
    }
}

// ============================================================================
// 1. SERIAL CSR CREATION (Step-by-step beginner logic)
// ============================================================================
inline CSR create_csr_serial(int n, const vector<Edge>& edges) {
    int nnz = edges.size();
    CSR csr(n, nnz);

    // Step 1: Count how many elements are in each row
    vector<int> degree(n, 0);
    for (int i = 0; i < nnz; i++) {
        int u = edges[i].u;
        degree[u]++;
    }

    // Step 2: Prefix sum - row_ptr[i+1] = row_ptr[i] + degree[i]
    csr.row_ptr[0] = 0;
    for (int i = 0; i < n; i++) {
        csr.row_ptr[i + 1] = csr.row_ptr[i] + degree[i];
    }

    // Step 3: Put each edge into its row's position
    vector<int> current_pos = csr.row_ptr;
    for (int i = 0; i < nnz; i++) {
        int u = edges[i].u;
        int slot = current_pos[u]++;
        csr.col_idx[slot] = edges[i].v;
        csr.values[slot] = edges[i].w;
    }

    // Step 4: Sort columns inside each row
    for (int i = 0; i < n; i++) {
        sort_row_elements(csr, csr.row_ptr[i], csr.row_ptr[i + 1]);
    }

    return csr;
}

// ============================================================================
// 2. PARALLEL CSR CREATION (Beginner Logic with OpenMP)
// ============================================================================
inline CSR create_csr_parallel(int n, const vector<Edge>& edges, int threads) {
    int nnz = edges.size();
    CSR csr(n, nnz);

    // Step 1: Count degrees in parallel using atomic update
    vector<int> degree(n, 0);
    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < nnz; i++) {
        int u = edges[i].u;
        #pragma omp atomic update
        degree[u]++;
    }

    // Step 2: Prefix sum to compute row_ptr (sequential prefix sum is fast and easy to understand)
    csr.row_ptr[0] = 0;
    for (int i = 0; i < n; i++) {
        csr.row_ptr[i + 1] = csr.row_ptr[i] + degree[i];
    }

    // Step 3: Put edges into their slots in parallel using atomic capture
    vector<int> current_pos = csr.row_ptr;
    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < nnz; i++) {
        int u = edges[i].u;
        int slot;
        #pragma omp atomic capture
        slot = current_pos[u]++;

        csr.col_idx[slot] = edges[i].v;
        csr.values[slot] = edges[i].w;
    }

    // Step 4: Sort each row's columns in parallel
    #pragma omp parallel for num_threads(threads) schedule(dynamic, 64)
    for (int i = 0; i < n; i++) {
        sort_row_elements(csr, csr.row_ptr[i], csr.row_ptr[i + 1]);
    }

    return csr;
}

// ============================================================================
// 3. SPARSE MATRIX-VECTOR MULTIPLICATION: y = A * x
// ============================================================================
inline vector<double> spmv_serial(const CSR& A, const vector<double>& x) {
    vector<double> y(A.n, 0.0);
    for (int i = 0; i < A.n; i++) {
        double sum = 0.0;
        int start = A.row_ptr[i];
        int end = A.row_ptr[i + 1];
        for (int k = start; k < end; k++) {
            sum += A.values[k] * x[A.col_idx[k]];
        }
        y[i] = sum;
    }
    return y;
}

inline vector<double> spmv_parallel(const CSR& A, const vector<double>& x, int threads) {
    vector<double> y(A.n, 0.0);
    // Each thread calculates independent rows in parallel
    #pragma omp parallel for num_threads(threads) schedule(guided)
    for (int i = 0; i < A.n; i++) {
        double sum = 0.0;
        int start = A.row_ptr[i];
        int end = A.row_ptr[i + 1];
        for (int k = start; k < end; k++) {
            sum += A.values[k] * x[A.col_idx[k]];
        }
        y[i] = sum;
    }
    return y;
}
