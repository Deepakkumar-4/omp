# CS509 Assignments: OpenMP Learning Solutions

These are **standalone teaching implementations**, based on the algorithm topics present in the two linked GitHub repositories. They are not drop-in replacements for the repositories' existing drivers, file formats or grading harnesses. No source repository was changed.

## Build and run

```sh
make
make test
make run_individual_assignment_01_gemm
./buddy_assignment_04_kmeans 4 10000
```

On Windows PowerShell, executable names may need `.exe` and `./` replaced by `.\`.

Every program uses a small built-in example or synthetic dataset, runs a serial version and a parallel version, compares the outputs, and prints timing and speedup. Supply the thread count as the first argument (default 4). Some matrix and clustering examples also accept problem size.

## Directory map

| Folder | Algorithms | OpenMP idea |
|---|---|---|
| individual/assignment_01 | GEMM, blocked GEMM | `parallel for`, `collapse(2)` |
| individual/assignment_02 | Bellman-Ford, Floyd-Warshall | per-round snapshot, parallel independent destinations |
| individual/assignment_03 | Prim, Kruskal | parallel minimum selection, task mergesort of edges |
| individual/assignment_04 | Welsh-Powell coloring, PageRank | degree calculation, destination-based rank updates |
| buddy/assignment_01 | BFS, DFS, SSSP | level frontier, recursive tasks, safe selection |
| buddy/assignment_02 | triangle counting, components, betweenness | reduction, label propagation, parallel sources |
| buddy/assignment_03 | gradient descent, Dinic maxflow | derivative reduction; residual flow kept serial |
| buddy/assignment_04 | k-means, FastMap | point assignment, independent projection/distance updates |

## Important limitations

- Programs use small demonstration graphs/data; they **do not parse the repositories' exact assignment input formats**.
- Some algorithms have intrinsically sequential stages. Welsh-Powell vertex coloring, Kruskal union-find, and Dinic residual augmentations are kept serial for correctness. The parallel parts are explicitly commented.
- These are **learning examples, not optimized parallel versions**. A high overhead may make them slower than serial code on small inputs.
- SSSP and Prim demonstrations assume no parallel duplicate-edge updates; for arbitrary multigraphs, process neighbor updates with local minima or synchronization.
- The synthetic graph used for PageRank is undirected with no dangling vertices. The original PageRank assignment has directed graphs and dangling vertices; extend the demonstration before using its full dataset.
- The FastMap demo uses an in-memory Euclidean distance matrix and should not be used to allocate very large N without checking memory (N^2 storage).
- Run `make test` and check PASS on your own compiler before using any approach in a submission.

## How to adapt to the original assignments

1. Keep the original `driver/`, input parser, and expected-output comparison files.
2. Move only the relevant parallel algorithm logic into the existing implementation.
3. Do not use these synthetic data generators as grading test input.
4. Verify small cases against the original serial implementation and then benchmark 1, 2, 4, and 8 threads.
5. In graph algorithms, keep a barrier between rounds/levels. Use task cutoff for recursive algorithms.

## Simple execution examples

```sh
make
./individual_assignment_01_blocked_gemm 4 256 32
./individual_assignment_02_floyd_warshall 4
./buddy_assignment_01_bfs 4
./buddy_assignment_02_triangle_counting 8
./buddy_assignment_04_fastmap 4 100
```

**Complexity note:** Each solution is intentionally kept easy to read, rather than maximally optimized. For Bellman-Ford and PageRank, scanning incoming neighbors directly is simple but can be slower than a CSR-based implementation.
