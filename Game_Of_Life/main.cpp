#include "game_of_life.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <omp.h>

using namespace std;

int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 512;          // Grid size: n x n
    int steps = argc > 2 ? atoi(argv[2]) : 30;       // Number of generations
    int max_threads = argc > 3 ? atoi(argv[3]) : 8; // Max OpenMP threads
    if (n <= 0 || steps <= 0 || max_threads <= 0) return 1;

    cout << "========================================================================\n";
    cout << "  GAME OF LIFE: TESTING SAME INITIAL GRID ACROSS DIFFERENT THREADS      \n";
    cout << "========================================================================\n";
    cout << "Grid: " << n << "x" << n << ", Generations: " << steps << "\n";
    cout << "------------------------------------------------------------------------\n";

    // STEP 1: Create the EXACT SAME initial grid ONCE
    vector<int> initial_grid(n * n, 0);
    for (int i = 0; i < n * n; i++) {
        initial_grid[i] = (i % 4 == 0) ? 1 : 0;
    }

    // STEP 2: Serial baseline simulation on this initial grid
    vector<int> serial_grid = initial_grid;
    double t = omp_get_wtime();
    game_of_life_serial(serial_grid, n, n, steps);
    double ts = omp_get_wtime() - t;
    cout << "Serial Baseline Time: " << fixed << setprecision(5) << ts << " s\n\n";

    // STEP 3: Test different thread counts (1, 2, 4, 8) on a fresh copy of the initial grid
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
        // IMPORTANT: Start from a fresh copy of initial_grid for fair comparison!
        vector<int> parallel_grid = initial_grid;

        t = omp_get_wtime();
        game_of_life_parallel(parallel_grid, n, n, steps, threads);
        double tp = omp_get_wtime() - t;

        // Verify: Does parallel final grid match serial final grid?
        bool ok = (serial_grid == parallel_grid);

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
