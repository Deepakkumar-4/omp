# CSR Graph Algorithms (Beginner Friendly)

## Overview
This folder provides 4 standard graph algorithms implemented in clear, easy-to-understand C++ with OpenMP parallelization on **Compressed Sparse Row (CSR)** graphs.

---

## The 4 Algorithms Explained Simply

### 1. Breadth-First Search (BFS)
- **Serial**: Queue-based traversal starting from source node.
- **Parallel (Level by Level)**: For distance $d$, all nodes at level $d$ check and visit their unvisited neighbors in parallel using `#pragma omp parallel for`.

### 2. PageRank
- Iterative rank calculation for web pages/nodes.
- Formula: $PR(u) = \frac{1 - d}{N} + d \sum_{v \in Nbrs(u)} \frac{PR(v)}{\text{Degree}(v)}$
- **Parallel**: Each thread updates a separate slice of vertices in parallel.

### 3. Single-Source Shortest Path (SSSP)
- **Serial**: Dijkstra's algorithm with min-heap priority queue.
- **Parallel**: Edge relaxation in parallel. When a shorter distance is found, update `dist[v]` safely using `#pragma omp critical`.

### 4. Connected Components (CC)
- **Serial**: Standard BFS traversal labeling components.
- **Parallel (Label Propagation)**: Each node starts with its own ID `comp[u] = u`. In parallel, nodes spread the smaller component ID to their neighbors until no IDs change.

---

## How to Compile and Run

```bash
g++ -O3 -fopenmp -std=c++17 -o graph_benchmark.exe main.cpp
./graph_benchmark.exe 5000 10 4
```
