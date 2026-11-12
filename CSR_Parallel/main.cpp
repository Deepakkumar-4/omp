#include "parallel_csr.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <omp.h>

using namespace std;

int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 20000;       // number of vertices / rows
    int nnz = argc > 2 ? atoi(argv[2]) : 500000;    // number of edges / non-zeros
    int max_threads = argc > 3 ? atoi(argv[3]) : 8; // max thread count
    if (n <= 0 || nnz <= 0 || max_threads <= 0) return 1;

    cout << "========================================================================\n";
    cout << "   PARALLEL CSR CREATION: TESTING SAME INPUT ACROSS DIFFERENT THREADS   \n";
    cout << "========================================================================\n";
    cout << "Vertices: " << n << ", Edges: " << nnz << "\n";
    cout << "------------------------------------------------------------------------\n";

    // STEP 1: Generate the exact same input edge list ONCE
    vector<Edge> edges(nnz);
    for (int i = 0; i < nnz; i++) {
        edges[i].u = (i * 37 + 13) % n;
        edges[i].v = (i * 73 + 17) % n;
        edges[i].w = 1.0 + (i % 10);
    }

    // STEP 2: Measure serial baseline time on this input
    double t = omp_get_wtime();
    CSR serial_csr = create_csr_serial(n, edges);
    double ts = omp_get_wtime() - t;
    cout << "Serial Baseline Time: " << fixed << setprecision(5) << ts << " s\n\n";

    // STEP 3: Test the EXACT SAME input across different thread counts
    vector<int> test_threads;
    for (int th = 1; th <= max_threads; th *= 2) {
        test_threads.push_back(th);
    }
    if (test_threads.back() != max_threads) {
        test_threads.push_back(max_threads);
    }

    cout << "Threads | Time (s) | Speedup | Efficiency | Verification\n";
    cout << "--------+----------+---------+------------+-------------\n";
    for (int threads : test_threads) {
        t = omp_get_wtime();
        // Exact same 'edges' input passed to parallel creation!
        CSR parallel_csr = create_csr_parallel(n, edges, threads);
        double tp = omp_get_wtime() - t;

        // Verify: check if parallel output matches serial output
        bool ok = true;
        if (serial_csr.row_ptr != parallel_csr.row_ptr) ok = false;
        if (serial_csr.col_idx != parallel_csr.col_idx) ok = false;
        for (int i = 0; i < nnz; i++) {
            if (fabs(serial_csr.values[i] - parallel_csr.values[i]) > 1e-6) {
                ok = false;
                break;
            }
        }

        // Speedup = Ts / Tp
        double speedup = (tp > 0) ? (ts / tp) : 0.0;
        // Efficiency = (Speedup / threads) * 100%
        double efficiency = (speedup / threads) * 100.0;

        cout << setw(7) << threads << " | "
             << setw(9) << fixed << setprecision(5) << tp << " | "
             << setw(6) << fixed << setprecision(2) << speedup << "x | "
             << setw(9) << fixed << setprecision(1) << efficiency << "% | "
             << (ok ? "PASS" : "FAIL") << "\n";
    }

    cout << "========================================================================\n";
    return 0;
}
