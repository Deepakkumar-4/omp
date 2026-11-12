# Conway's Game of Life (Beginner Friendly)

## Simple Rules
1. **Live cell with 2 or 3 neighbors** survives.
2. **Dead cell with exactly 3 neighbors** becomes alive.
3. **All other cells** die or stay dead.

---

## Beginner-Friendly OpenMP Implementation

### 1. Counting Neighbors (Toroidal Grid with `%`)
```cpp
int count_neighbors(const vector<int>& grid, int r, int c, int rows, int cols) {
    int live = 0;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue; // Skip cell itself
            int nr = (r + dr + rows) % rows;   // Wrap around edge
            int nc = (c + dc + cols) % cols;
            live += grid[nr * cols + nc];
        }
    }
    return live;
}
```

### 2. Parallelizing the Generation Loop
Each thread computes the next state for different rows in parallel:
```cpp
#pragma omp parallel for num_threads(threads) schedule(static)
for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
        int neighbors = count_neighbors(grid, r, c, rows, cols);
        next_grid[r * cols + c] = get_next_state(grid[r * cols + c], neighbors);
    }
}
grid = next_grid;
```

---

## How to Compile and Run

```bash
g++ -O3 -fopenmp -std=c++17 -o game_of_life.exe main.cpp
./game_of_life.exe 512 20 4
```
