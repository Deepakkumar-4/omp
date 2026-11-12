# Parallel Compressed Sparse Row (CSR) Creation (Beginner Friendly)

## What is CSR?
Compressed Sparse Row (CSR) is an efficient way to store sparse matrices or graphs using 3 simple 1D arrays:
1. `row_ptr`: Tells you where each row starts and ends in `col_idx`.
2. `col_idx`: The destination column (or neighbor vertex) of each edge.
3. `values`: The actual value or weight of each edge.

---

## 4 Easy Steps to Create CSR in Parallel

### Step 1: Count Row Degrees in Parallel
Each edge `(u, v)` belongs to row `u`. We increment `degree[u]` using OpenMP atomic update:
```cpp
#pragma omp parallel for num_threads(threads)
for (int i = 0; i < nnz; i++) {
    int u = edges[i].u;
    #pragma omp atomic update
    degree[u]++;
}
```

### Step 2: Prefix Sum to Build `row_ptr`
`row_ptr[0] = 0`, and each subsequent entry is the sum of previous degrees:
```cpp
csr.row_ptr[0] = 0;
for (int i = 0; i < n; i++) {
    csr.row_ptr[i + 1] = csr.row_ptr[i] + degree[i];
}
```

### Step 3: Insert Edges in Parallel
Using `#pragma omp atomic capture`, each thread atomically gets a unique slot index without collisions:
```cpp
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
```

### Step 4: Sort Columns within each Row in Parallel
```cpp
#pragma omp parallel for num_threads(threads) schedule(dynamic, 64)
for (int i = 0; i < n; i++) {
    sort_row_elements(csr, csr.row_ptr[i], csr.row_ptr[i + 1]);
}
```

---

## How to Compile and Run

```bash
g++ -O3 -fopenmp -std=c++17 -o csr_benchmark.exe main.cpp
./csr_benchmark.exe 10000 200000 4
```
