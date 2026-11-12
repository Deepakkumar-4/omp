#include "csr_graph.hpp"
#include "bfs.hpp"
#include "pagerank.hpp"
#include "sssp.hpp"
#include "connected_components.hpp"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <omp.h>

using namespace std;

// This program demonstrates:
// 1. Creating the EXACT SAME input graph once.
// 2. Running serial baseline on that input.
// 3. Testing the same input across DIFFERENT thread counts (1, 2, 4, 8).
// 4. Calculating Speedup = (Serial Time / Parallel Time)
// 5. Calculating Efficiency = (Speedup / Threads) * 100%

int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 20000;       // number of vertices
    int avg_deg = argc > 2 ? atoi(argv[2]) : 15;   // edges per vertex
    int max_threads = argc > 3 ? atoi(argv[3]) : 8;// maximum threads to test
    if (n <= 0 || avg_deg <= 0 || max_threads <= 0) return 1;

    cout << "========================================================================\n";
    cout << "     TESTING SAME INPUT GRAPH ACROSS DIFFERENT THREAD COUNTS            \n";
    cout << "========================================================================\n";
    cout << "Graph Size:  " << n << " vertices, ~" << (n * avg_deg) << " edges\n";
    cout << "------------------------------------------------------------------------\n";

    // STEP 1: Generate the input graph ONCE using a fixed seed (42).
    // The exact same 'g' will be shared across all serial and parallel tests.
    CSRGraph g = generate_simple_test_graph(n, avg_deg, 42);

    // List of thread counts to test on the SAME input: e.g. 1, 2, 4, 8
    vector<int> test_threads;
    for (int t = 1; t <= max_threads; t *= 2) {
        test_threads.push_back(t);
    }
    if (test_threads.back() != max_threads) {
        test_threads.push_back(max_threads);
    }

    // ========================================================================
    // ALGORITHM 1: Breadth-First Search (BFS)
    // ========================================================================
    cout << "\n[1] BREADTH-FIRST SEARCH (BFS)\n";
    // 1. Compute Serial Baseline Time
    double t = omp_get_wtime();
    vector<int> bfs_s = bfs_serial(g, 0);
    double ts_bfs = omp_get_wtime() - t;
    cout << "Serial Time (1 core): " << fixed << setprecision(5) << ts_bfs << " s\n";

    // 2. Test the SAME input 'g' with different thread counts
    cout << "Threads | Time (s) | Speedup | Efficiency | Verification\n";
    cout << "--------+----------+---------+------------+-------------\n";
    for (int threads : test_threads) {
        t = omp_get_wtime();
        // Exact same input 'g' passed to parallel function!
        vector<int> bfs_p = bfs_parallel(g, 0, threads);
        double tp = omp_get_wtime() - t;

        // Verify: Does parallel output match serial output?
        bool pass = (bfs_s == bfs_p);

        // Speedup = how many times faster parallel is compared to serial
        double speedup = (tp > 0) ? (ts_bfs / tp) : 0.0;
        // Efficiency = how well each thread is utilized (100% is perfect)
        double efficiency = (speedup / threads) * 100.0;

        cout << setw(7) << threads << " | "
             << setw(9) << fixed << setprecision(5) << tp << " | "
             << setw(6) << fixed << setprecision(2) << speedup << "x | "
             << setw(9) << fixed << setprecision(1) << efficiency << "% | "
             << (pass ? "PASS" : "FAIL") << "\n";
    }

    // ========================================================================
    // ALGORITHM 2: PageRank
    // ========================================================================
    cout << "\n[2] PAGERANK (20 iterations)\n";
    t = omp_get_wtime();
    vector<double> pr_s = pagerank_serial(g, 0.85, 20);
    double ts_pr = omp_get_wtime() - t;
    cout << "Serial Time (1 core): " << fixed << setprecision(5) << ts_pr << " s\n";

    cout << "Threads | Time (s) | Speedup | Efficiency | Verification\n";
    cout << "--------+----------+---------+------------+-------------\n";
    for (int threads : test_threads) {
        t = omp_get_wtime();
        // Exact same input 'g' passed to parallel PageRank
        vector<double> pr_p = pagerank_parallel(g, 0.85, 20, threads);
        double tp = omp_get_wtime() - t;

        // Verify max difference < 1e-6
        bool pass = true;
        for (int i = 0; i < n; i++) {
            if (fabs(pr_s[i] - pr_p[i]) > 1e-6) { pass = false; break; }
        }

        double speedup = (tp > 0) ? (ts_pr / tp) : 0.0;
        double efficiency = (speedup / threads) * 100.0;

        cout << setw(7) << threads << " | "
             << setw(9) << fixed << setprecision(5) << tp << " | "
             << setw(6) << fixed << setprecision(2) << speedup << "x | "
             << setw(9) << fixed << setprecision(1) << efficiency << "% | "
             << (pass ? "PASS" : "FAIL") << "\n";
    }

    // ========================================================================
    // ALGORITHM 3: Single-Source Shortest Path (SSSP)
    // ========================================================================
    cout << "\n[3] SINGLE-SOURCE SHORTEST PATH (SSSP)\n";
    t = omp_get_wtime();
    vector<double> sssp_s = sssp_serial(g, 0);
    double ts_sssp = omp_get_wtime() - t;
    cout << "Serial Time (1 core): " << fixed << setprecision(5) << ts_sssp << " s\n";

    cout << "Threads | Time (s) | Speedup | Efficiency | Verification\n";
    cout << "--------+----------+---------+------------+-------------\n";
    for (int threads : test_threads) {
        t = omp_get_wtime();
        // Exact same input 'g' passed to parallel SSSP
        vector<double> sssp_p = sssp_parallel(g, 0, threads);
        double tp = omp_get_wtime() - t;

        bool pass = true;
        for (int i = 0; i < n; i++) {
            if (fabs(sssp_s[i] - sssp_p[i]) > 1e-4) { pass = false; break; }
        }

        double speedup = (tp > 0) ? (ts_sssp / tp) : 0.0;
        double efficiency = (speedup / threads) * 100.0;

        cout << setw(7) << threads << " | "
             << setw(9) << fixed << setprecision(5) << tp << " | "
             << setw(6) << fixed << setprecision(2) << speedup << "x | "
             << setw(9) << fixed << setprecision(1) << efficiency << "% | "
             << (pass ? "PASS" : "FAIL") << "\n";
    }

    // ========================================================================
    // ALGORITHM 4: Connected Components (CC)
    // ========================================================================
    cout << "\n[4] CONNECTED COMPONENTS (CC)\n";
    t = omp_get_wtime();
    vector<int> cc_s = connected_components_serial(g);
    double ts_cc = omp_get_wtime() - t;
    int comp_count_s = count_components(cc_s);
    cout << "Serial Time (1 core): " << fixed << setprecision(5) << ts_cc << " s (Components: " << comp_count_s << ")\n";

    cout << "Threads | Time (s) | Speedup | Efficiency | Verification\n";
    cout << "--------+----------+---------+------------+-------------\n";
    for (int threads : test_threads) {
        t = omp_get_wtime();
        // Exact same input 'g' passed to parallel CC
        vector<int> cc_p = connected_components_parallel(g, threads);
        double tp = omp_get_wtime() - t;

        bool pass = (comp_count_s == count_components(cc_p));
        double speedup = (tp > 0) ? (ts_cc / tp) : 0.0;
        double efficiency = (speedup / threads) * 100.0;

        cout << setw(7) << threads << " | "
             << setw(9) << fixed << setprecision(5) << tp << " | "
             << setw(6) << fixed << setprecision(2) << speedup << "x | "
             << setw(9) << fixed << setprecision(1) << efficiency << "% | "
             << (pass ? "PASS" : "FAIL") << "\n";
    }

    cout << "\n========================================================================\n";
    cout << " All tests completed on the same graph input.\n";
    cout << "========================================================================\n";
    return 0;
}
