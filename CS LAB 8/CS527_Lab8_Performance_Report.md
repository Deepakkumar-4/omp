# CS527 – Computer Systems
## Lab 8: OpenMP — Arrays and Matrices

### Objective
Implement five OpenMP programs for arrays and matrices, verify each parallel result against its serial reference, and compare execution time for different thread counts.

### Build and execution

```bash
make
make run1
make run2
make run3
make run4
make run5
```

Compiler flags used:

```make
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp
```

The supplied terminal log shows that all five tasks compiled and ran, with `Verification: PASS` in every recorded run. Compiler diagnostics were not reported.

### Measurement method

- **Speedup (serial reference):** `T_serial / T_parallel` for Tasks 1, 3, 4, and 5.
- **Parallel efficiency:** `Speedup / thread_count × 100%`.
- **Task 2 speedup:** `T_odd-even(1 thread) / T_odd-even(p threads)`; this is a like-for-like comparison. The `std::sort` time is listed separately, because it uses a different algorithm.
- **N/A** means the recorded time was zero or insufficient to calculate a meaningful ratio. Very short tests are sensitive to timer resolution and runtime noise.
- Times and speedups below are from the user's supplied execution log; no additional benchmark runs are claimed.

---

## Task 1: Parallel Array Search

**Method:** Divide the array among OpenMP threads using `parallel for`. A `critical` section protects updates to the shared matching index. The program returns the smallest matching index.

**Input:** 10,000,000 elements; target `9999999`.

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency | Verification |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.00500011 | 0.00599980 | 0.8334× | 83.34% | PASS |
| 2 | 0.00699997 | 0.01300000 | 0.5385× | 26.92% | PASS |
| 4 | 0.00500011 | 0.00300002 | 1.6667× | 41.67% | PASS |
| 8 | 0.00399995 | 0.00300002 | 1.3333× | 16.67% | PASS |

**Correctness:** All runs returned index `9999999`, matching serial search.

**Observation:** The four-thread run had the best recorded speedup (1.67×). The two-thread run was slower than serial execution. Thread startup, scheduling, and short execution times can affect the results.

---

## Task 2: Parallel Sorting

**Method:** Odd-even transposition sort. In each even or odd phase, independent neighboring pairs are compared in parallel. The implicit barrier after `omp parallel for` completes a phase before the next phase starts. No `critical` section is needed because pairs do not overlap within a phase.

**Input:** 10,000 integers in reverse order.

| Threads | Serial `std::sort` (s) | Parallel odd-even sort (s) | Odd-even speedup vs 1 thread | Efficiency | Verification |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0 (reported) | 0.104 | 1.000× | 100.00% | PASS |
| 2 | 0 (reported) | 0.406 | 0.256× | 12.81% | PASS |
| 4 | 0 (reported) | 0.609 | 0.171× | 4.27% | PASS |
| 8 | 0 (reported) | 0.849 | 0.122× | 1.53% | PASS |

**Correctness:** Every run returned a sorted sequence starting `1 2 3 4 5 6 7 8 9 10` and `Verification: PASS` against `std::sort`.

**Observation:** More threads made this implementation slower. Odd-even sort requires many phases and a synchronization barrier after each phase. For `n = 10000`, barrier overhead dominates much of the benefit of parallel pair comparisons. The reported zero for serial `std::sort` is not evidence that it took literally no time; the measurement was too small for the displayed timer resolution.

**Note:** Because serial `std::sort` and odd-even sort have different work complexity, the speedup and efficiency columns above use the **one-thread odd-even implementation** as baseline rather than `std::sort`.

---

## Task 3: Histogram Calculation

**Method:** Each thread counts values 0–9 in its private ten-element array. A short `critical` section combines local counters into the global histogram, avoiding race conditions during counting.

**Input:** 10,000,000 values from 0 to 9, repeated equally.

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency | Verification |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.00600004 | 0.00800014 | 0.7500× | 75.00% | PASS |
| 2 | 0.00799990 | 0.00500011 | 1.5999× | 80.00% | PASS |
| 4 | 0.00800014 | 0.00399995 | 2.0001× | 50.00% | PASS |
| 8 | 0.00600004 | 0.00300002 | 2.0000× | 25.00% | PASS |

**Correctness:** Each bin from `0` through `9` had a frequency of `1,000,000`. All four runs passed verification.

**Observation:** The best reported parallel time was about 0.003 s using eight threads, although the four-thread run achieved nearly the same speedup (2.00×). The speedup did not continue to scale with thread count, likely because of memory access costs and fixed parallel overhead.

---

## Task 4: Matrix Transpose

**Method:** Use `parallel for` across input matrix rows, assigning each matrix element `B[j][i] = A[i][j]`. Each output cell is written once, so no `critical` section is required.

### Small input: 8 × 8

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency | Verification |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0 | 0 | N/A | N/A | PASS |
| 2 | 0.000999928 | 0 | N/A | N/A | PASS |
| 4 | 0 | 0.000999928 | N/A | N/A | PASS |
| 8 | 0 | 0.000999928 | N/A | N/A | PASS |

**Correctness:** The transposed 8×8 matrix was printed and all runs passed. For example, its first output row was `1 9 17 25 33 41 49 57`.

**Observation:** An 8×8 matrix is too small for a reliable timing comparison in the given log. Values printed as zero should not be used to infer infinite speedup or zero performance.

### Large input: 4096 × 4096

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency | Verification |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.258 | 0.255 | 1.0118× | 101.18% | PASS |
| 2 | 0.257 | 0.156 | 1.6474× | 82.37% | PASS |
| 4 | 0.310 | 0.199 | 1.5578× | 38.94% | PASS |
| 8 | 0.360 | 0.204 | 1.7647× | 22.06% | PASS |

**Correctness:** All large-matrix runs returned `Verification: PASS`.

**Observation:** The eight-thread run had the largest measured speedup relative to the serial time from its corresponding run (1.76×), while the two-thread run had the fastest absolute parallel time (0.156 s). This difference occurs because the serial baseline also varied between runs. A transpose can be limited by memory access patterns and cache behavior. Efficiency slightly above 100% at one thread reflects measurement variation rather than a physically meaningful superlinear gain.

---

## Task 5: 2D Convolution

**Method:** Apply a 3×3 averaging filter to each valid position of the image. The output dimensions are `(N - 2) × (N - 2)` with no padding. Use `parallel for` on output rows. Each thread computes independent output elements, so no `critical` section is needed.

### Input: 256 × 256 (output 254 × 254)

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency | Verification |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0 | 0 | N/A | N/A | PASS |
| 2 | 0 | 0.00100017 | N/A | N/A | PASS |
| 4 | 0.000999928 | 0.000999928 | 1.000× | 25.00% | PASS |
| 8 | 0 | 0.00300002 | N/A | N/A | PASS |

### Input: 512 × 512 (output 510 × 510)

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency | Verification |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.00300002 | 0.00300002 | 1.000× | 100.00% | PASS |
| 2 | 0.000999928 | 0.00300002 | 0.3333× | 16.67% | PASS |
| 4 | 0.00300002 | 0.00300002 | 1.000× | 25.00% | PASS |
| 8 | 0.00199986 | 0.00200009 | 0.9999× | 12.50% | PASS |

### Input: 1024 × 1024 (output 1022 × 1022)

| Threads | Serial time (s) | Parallel time (s) | Speedup | Efficiency | Verification |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.00800014 | 0.00699997 | 1.1429× | 114.29% | PASS |
| 2 | 0.00999999 | 0.00500011 | 2.0000× | 100.00% | PASS |
| 4 | 0.00699997 | 0.00300002 | 2.3333× | 58.33% | PASS |
| 8 | 0.00900006 | 0.00499988 | 1.8001× | 22.50% | PASS |

**Correctness:** Every convolution run passed comparison against the serial output.

**Observation:** For 1024×1024, four threads achieved the best reported parallel time (0.003 s) and speedup (2.33×). The eight-thread run was slower, possibly because of overhead and memory contention. At smaller image sizes, execution times were too short for stable comparisons. Reported efficiencies above 100% reflect noise or differences in execution conditions, not guaranteed scaling.

---

## Combined findings

| Task | Largest tested input | Best recorded thread count by speedup | Best recorded speedup | Correctness |
|---|---|---:|---:|:---:|
| Array search | 10,000,000 elements | 4 | 1.67× | PASS |
| Odd-even sorting | 10,000 elements | 1 | 1.00× (odd-even baseline) | PASS |
| Histogram | 10,000,000 elements | 4 | 2.00× | PASS |
| Matrix transpose | 4096×4096 | 8 | 1.76× | PASS |
| 2D convolution | 1024×1024 | 4 | 2.33× | PASS |

**Interpretation:** Parallel execution helped in the larger histogram, transpose, and convolution runs, but performance was not proportional to thread count. Sorting was slowed by repeated synchronization. Timing data for very small matrices must be treated with care. For stronger results, repeat each test multiple times, report the median, and use higher-resolution timing measurements.

---

## Submission checklist

- [x] **Source files:** `task1.cpp`, `task2.cpp`, `task3.cpp`, `task4.cpp`, `task5.cpp` supplied separately in the project; build commands include `-fopenmp -Wall` (and `-Wextra`). The supplied log shows successful builds.
- [x] **Correctness verification:** Serial-versus-parallel checks are included for all five tasks; every run in the supplied terminal output reports `Verification: PASS`.
- [x] **Timing tables:** Thread counts, serial time, and parallel time are recorded for every supplied test size.
- [x] **Speedup and efficiency:** Calculated where times permit meaningful division. Task 2 uses its one-thread odd-even implementation as the reference.

### Files to submit together

```text
CS527_Lab8/
├── Makefile
├── common.hpp                  # If used by project dependencies
├── task1.cpp
├── task2.cpp
├── task3.cpp
├── task4.cpp
├── task5.cpp
└── CS527_Lab8_Performance_Report.md
```

*All performance values in this report are taken from the supplied runs. The report does not claim independent remeasurement.*
