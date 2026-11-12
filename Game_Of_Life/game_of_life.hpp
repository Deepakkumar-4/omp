#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

using namespace std;

// Conway's Game of Life rules:
// 1. Live cell with 2 or 3 neighbors survives.
// 2. Dead cell with exactly 3 neighbors becomes alive.
// 3. All other cells die or stay dead.

// Count live neighbors for cell at (r, c) with wrap-around (toroidal grid)
inline int count_neighbors(const vector<int>& grid, int r, int c, int rows, int cols) {
    int live = 0;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue; // Skip the cell itself
            int nr = (r + dr + rows) % rows;   // Wrap row boundary
            int nc = (c + dc + cols) % cols;   // Wrap column boundary
            live += grid[nr * cols + nc];
        }
    }
    return live;
}

// Compute the next cell state based on Conway's rules
inline int get_next_state(int current_state, int neighbors) {
    if (current_state == 1 && (neighbors == 2 || neighbors == 3)) {
        return 1; // Survives
    }
    if (current_state == 0 && neighbors == 3) {
        return 1; // Reproduction
    }
    return 0; // Dies or remains dead
}

// ============================================================================
// 1. SERIAL GAME OF LIFE
// ============================================================================
inline void game_of_life_serial(vector<int>& grid, int rows, int cols, int steps) {
    vector<int> next_grid(rows * cols, 0);

    for (int step = 0; step < steps; step++) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int neighbors = count_neighbors(grid, r, c, rows, cols);
                next_grid[r * cols + c] = get_next_state(grid[r * cols + c], neighbors);
            }
        }
        grid = next_grid; // Move to next generation
    }
}

// ============================================================================
// 2. PARALLEL GAME OF LIFE (OpenMP Beginner Logic)
// ============================================================================
inline void game_of_life_parallel(vector<int>& grid, int rows, int cols, int steps, int threads) {
    vector<int> next_grid(rows * cols, 0);

    for (int step = 0; step < steps; step++) {
        // Parallelize the row loop across threads: each thread updates distinct rows
        #pragma omp parallel for num_threads(threads) schedule(static)
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int neighbors = count_neighbors(grid, r, c, rows, cols);
                next_grid[r * cols + c] = get_next_state(grid[r * cols + c], neighbors);
            }
        }
        grid = next_grid; // Move to next generation
    }
}
