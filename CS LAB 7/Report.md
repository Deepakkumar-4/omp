# CS527 – Computer Systems
## Lab: Introduction to OpenMP — Arrays and Matrices

**Student Name:** Deepak kumar
**Entry No.:** 2026CSM1012
**Course:** CS527 – Computer Systems  
**Language:** C++17  
**Compiler:** `g++ -std=c++17 -O2 -Wall -Wextra -fopenmp`  
**Thread configuration:** Thread count passed as the second command-line argument (1, 2, 4, or 8); program output confirms these requested counts.  
**Machine / CPU / RAM / OS:** _Add system details before submission._  
**Timing method:** `omp_get_wtime()`  

> **Measurement note:** All timing numbers below are transcribed from the provided terminal output, not estimated. These are single-run measurements, with many very short intervals quantized near 0.001 s. Repeated trials and median times would make comparisons more reliable. A PASS means that the tested implementation matched the serial reference for that run. All Task 5 sizes (256, 512, 1024) were supplied.

## Aim and methodology

Implement serial and OpenMP versions of five array/matrix operations, verify correctness, and compare execution time, speedup and parallel efficiency. The programs were compiled with OpenMP enabled. Each executable was called as `./taskN SIZE THREADS`. The serial baseline is measured separately within each invocation. Speedup is `T_serial / T_variant`; efficiency is `speedup / thread_count`. For the optional matrix multiplication `i-k-j` implementations, the program's reported speedup uses the original `i-j-k` serial baseline, so this value combines algorithmic memory-layout gains with parallel gains.

## Task 1 — Vector addition

Compute `c[i] = a[i] + b[i]` with serial and `#pragma omp parallel for` loops. All reported results passed exact equality verification.

**Input: N = 10,000,000**

| Threads | Serial (s) | Parallel (s) | Speedup | Efficiency | Check |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.013000 | 0.013000 | 1.000 | 1.000 | PASS |
| 2 | 0.012000 | 0.010000 | 1.200 | 0.600 | PASS |
| 4 | 0.012000 | 0.009000 | 1.333 | 0.333 | PASS |
| 8 | 0.012000 | 0.010000 | 1.200 | 0.150 | PASS |

**Observations:** The best measured result was at four threads (about 1.33×). Eight threads did not improve on four, consistent with overhead, memory bandwidth limits, and system scheduling. Vector addition does little arithmetic per memory access, so additional threads can reach a memory-bandwidth limit quickly.

**Questions:**

- **When does parallelization pay off?** For the supplied `N=10^7` runs, it pays off at 2 and 4 threads. The crossover array size cannot be determined because the required `10^5`, `10^6`, and `10^8` measurements have not been provided. Small arrays often run faster serially because creating/scheduling parallel work costs time.
- **Does speedup increase linearly?** No. Measured speedup rose to 1.33× at four threads and fell to 1.20× at eight. Memory bandwidth, overhead, contention, and limited physical cores prevent linear scaling.

**Still required by lab:** `N = 10^5, 10^6, 10^8`, each at 1, 2, 4, 8 threads; largest-N speedup table/plot.

## Task 2 — Sum of array elements

The correct OpenMP version uses `#pragma omp parallel for reduction(+:sum)`. The deliberately broken version uses a shared `sum` without synchronization.

**Input: N = 10,000,000**

| Threads | Serial (s) | Reduction (s) | Speedup | Efficiency | Check |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.006000 | 0.004000 | 1.500 | 1.500 | PASS |
| 2 | 0.003000 | 0.002000 | 1.500 | 0.750 | PASS |
| 4 | 0.005000 | 0.001000 | 4.999 | 1.250 | PASS |
| 8 | 0.004000 | 0.001000 | 4.000 | 0.500 | PASS |

**Race-condition demonstration:** Expected sum in every run: `59,999,995`.

| Threads | Broken run 1 | Run 2 | Run 3 | Run 4 | Run 5 |
|---:|---:|---:|---:|---:|---:|
| 1 | 59999995 | 59999995 | 59999995 | 59999995 | 59999995 |
| 2 | 29999985 | 29999985 | 30000010 | 29999985 | 29999985 |
| 4 | 15000004 | 15000006 | 15000004 | 15000004 | 14999988 |
| 8 | 14999993 | 7499986 | 7499986 | 7499986 | 7499986 |

**Questions:**

- **Why is the naive sum wrong?** `sum += a[i]` reads, modifies and writes the same shared variable. Multiple threads can read an old value and overwrite each other's updates: a data race. With one thread there is no concurrent conflict; with multiple threads the result is incorrect and can vary between runs.
- **Would `critical` fix correctness?** Yes, a `critical` section around each update would serialize updates and remove this race, but would usually be much slower because all threads compete for the lock for every element. `reduction` gives each thread a private partial sum and combines the sums at the end, reducing synchronization.

**Observation:** The 4-thread speedup is about 5×, exceeding the ideal 4× for this baseline. This is not evidence of superlinear parallel scaling by itself: short runs, different serial baselines, compiler optimizations and timing resolution can distort results. Repeat measurements for a robust comparison.

**Still required:** `N=10^8` for all four thread counts.

## Task 3 — Maximum and minimum values and indices

The implementations compare a serial scan, manual per-thread `(value, index)` combination, a critical-section method, and value-only `min/max` reductions. All supplied runs passed.

**Input: N = 10,000,000**

| Threads | Serial (s) | Manual (s) | Critical (s) | Value reduction (s) |
|---:|---:|---:|---:|---:|
| 1 | 0.012000 | 0.009000 | 0.009000 | 0.007000 |
| 2 | 0.012000 | 0.004000 | 0.004000 | 0.004000 |
| 4 | 0.015000 | 0.004000 | 0.004000 | 0.003000 |
| 8 | 0.012000 | 0.002000 | 0.002000 | 0.002000 |

| Threads | Manual speedup / efficiency | Critical speedup / efficiency | Value reduction speedup / efficiency |
|---:|---:|---:|---:|
| 1 | 1.333 / 1.333 | 1.333 / 1.333 | 1.714 / 1.714 |
| 2 | 3.000 / 1.500 | 3.000 / 1.500 | 3.000 / 1.500 |
| 4 | 3.750 / 0.938 | 3.750 / 0.938 | 5.000 / 1.250 |
| 8 | 6.000 / 0.750 | 6.000 / 0.750 | 6.000 / 0.750 |

**Verified values:** Minimum `-99999` at index `5000000`; maximum `99999` at index `3333333` (same for all supplied runs).

**Questions:**

- **Why not `reduction(max:best_idx)`?** The largest index is not necessarily the index of the largest value. The comparison must preserve a linked `(value, index)` pair and a tie-breaking rule.
- **Critical vs manual reduction:** In the supplied results they have nearly identical times. A manual per-thread reduction usually scales better because threads compute locally and only combine once. A critical section around every element would cause severe lock contention; a critical section used only once per thread can be competitive. The implementation's exact critical-section placement matters, so no universal speed advantage can be inferred from these rounded timings.

**Still required:** `N=10^8` measurements. A direct conclusion about which index-finding method is faster needs repeated runs.

## Task 4 — Matrix addition

Compute `C[i][j]=A[i][j]+B[i][j]` with serial nested loops, an OpenMP parallel outer loop, and `collapse(2)`. All checks passed.

**N = 8 (8×8):** Every reported serial, outer and collapsed timing was `0` seconds at 1, 2, 4 and 8 threads. The program reported speedup and efficiency as `0` for these cases; **these are not meaningful performance ratios** because the denominator was measured as zero. Run many repetitions inside each timed section to resolve sub-millisecond differences.

**N = 4096 (4096×4096)**

| Threads | Serial (s) | Outer (s) | Collapse(2) (s) | Outer speedup / efficiency | Collapse speedup / efficiency |
|---:|---:|---:|---:|---:|---:|
| 1 | 0.020000 | 0.021000 | 0.021000 | 0.952 / 0.952 | 0.952 / 0.952 |
| 2 | 0.020000 | 0.018000 | 0.018000 | 1.111 / 0.556 | 1.111 / 0.556 |
| 4 | 0.021000 | 0.016000 | 0.015000 | 1.312 / 0.328 | 1.400 / 0.350 |
| 8 | 0.020000 | 0.016000 | 0.016000 | 1.250 / 0.156 | 1.250 / 0.156 |

**Questions:**

- **Small N:** Parallelizing only the outer loop provides 8 row iterations; `collapse(2)` provides 64 independent cell iterations. This can distribute work more evenly when there are many threads, but the 8×8 workload is so small that scheduling overhead can dominate. The provided zero timings cannot show whether it helped.
- **Large N:** There are already 4096 independent outer-loop iterations, enough for 1–8 threads. `collapse(2)` adds little useful parallelism. The measured 4-thread case was slightly faster with collapse, while the 8-thread case was equal at displayed precision.

**Observation:** Both strategies show limited scaling at 4096, consistent with a memory-bandwidth-bound element-wise operation.

## Task 5 — Naive matrix multiplication

Compute `C=A×B` with serial `i-j-k`, parallel outer `i`, `collapse(2)` on `(i,j)`, and the optional memory-friendly `i-k-j` loop order. All supplied checks passed.

### N = 256

| Threads | Serial (s) | Outer (s) | Collapse(2) (s) | ikj serial (s) | ikj parallel (s) |
|---:|---:|---:|---:|---:|---:|
| 1 | 0.011000 | 0.011000 | 0.011000 | 0.007000 | 0.008000 |
| 2 | 0.012000 | 0.006000 | 0.005000 | 0.007000 | 0.004000 |
| 4 | 0.014000 | 0.003000 | 0.003000 | 0.007000 | 0.002000 |
| 8 | 0.012000 | 0.004000 | 0.004000 | 0.007000 | 0.002000 |

| Threads | Outer speedup / efficiency | Collapse speedup / efficiency | ikj parallel speedup / efficiency* |
|---:|---:|---:|---:|
| 1 | 1.000 / 1.000 | 1.000 / 1.000 | 1.375 / 1.375 |
| 2 | 2.000 / 1.000 | 2.400 / 1.200 | 3.000 / 1.500 |
| 4 | 4.667 / 1.167 | 4.667 / 1.167 | 7.000 / 1.750 |
| 8 | 3.000 / 0.375 | 3.000 / 0.375 | 6.000 / 0.750 |

### N = 512

| Threads | Serial (s) | Outer (s) | Collapse(2) (s) | ikj serial (s) | ikj parallel (s) |
|---:|---:|---:|---:|---:|---:|
| 1 | 0.437000 | 0.435000 | 0.438000 | 0.072000 | 0.081000 |
| 2 | 0.394000 | 0.188000 | 0.187000 | 0.059000 | 0.030000 |
| 4 | 0.394000 | 0.097000 | 0.096000 | 0.060000 | 0.015000 |
| 8 | 0.388000 | 0.049000 | 0.049000 | 0.060000 | 0.011000 |

| Threads | Outer speedup / efficiency | Collapse speedup / efficiency | ikj parallel speedup / efficiency* |
|---:|---:|---:|---:|
| 1 | 1.005 / 1.005 | 0.998 / 0.998 | 5.395 / 5.395 |
| 2 | 2.096 / 1.048 | 2.107 / 1.053 | 13.133 / 6.567 |
| 4 | 4.062 / 1.015 | 4.104 / 1.026 | 26.266 / 6.567 |
| 8 | 7.918 / 0.990 | 7.918 / 0.990 | 35.273 / 4.409 |

\* **Important:** These `ikj parallel` speedups/efficiencies use the original `i-j-k` serial time, not the `ikj serial` time. They measure the combined effect of loop reordering and threading; they **must not** be interpreted as pure parallel efficiency. For a fair parallel-only comparison, calculate `T_ikj_serial / T_ikj_parallel`.

### N = 1024

All variants passed correctness verification.

| Threads | Serial (s) | Outer (s) | Collapse(2) (s) | ikj serial (s) | ikj parallel (s) |
|---:|---:|---:|---:|---:|---:|
| 1 | 4.386000 | 5.900000 | 4.371000 | 0.570000 | 0.552000 |
| 2 | 3.718000 | 1.638000 | 1.574000 | 0.486000 | 0.304000 |
| 4 | 3.229000 | 0.798000 | 0.794000 | 0.494000 | 0.127000 |
| 8 | 3.259000 | 0.578000 | 0.566000 | 0.483000 | 0.085000 |

| Threads | Outer speedup / efficiency | Collapse speedup / efficiency | ikj parallel speedup / efficiency* |
|---:|---:|---:|---:|
| 1 | 0.743 / 0.743 | 1.003 / 1.003 | 7.946 / 7.946 |
| 2 | 2.270 / 1.135 | 2.362 / 1.181 | 12.230 / 6.115 |
| 4 | 4.046 / 1.012 | 4.067 / 1.017 | 25.425 / 6.356 |
| 8 | 5.638 / 0.705 | 5.758 / 0.720 | 38.341 / 4.793 |

\* The `ikj parallel` speedup is relative to original `i-j-k` serial, combining loop-order and thread effects. At 8 threads, pure `i-k-j` parallel speedup relative to `ikj_serial` is 0.483000 / 0.085000 ≈ 5.68×, efficiency ≈ 0.710.

**Observation:** At N=1024, 8-thread collapse(2) takes 0.566 s versus 0.578 s for outer-only, a small improvement. The `i-k-j` order provides much greater benefit through contiguous row-major accesses. The original serial baseline varies across runs, so speedup values should be interpreted cautiously.

**Questions:**

- **Why is outer `i` safe?** Each thread owns separate rows of `C`. Even though `k` accumulates a sum, only one thread updates any given `C[i][j]` in this decomposition.
- **Why is a plain parallel `k` unsafe?** Multiple threads would add to the same `C[i][j]` simultaneously. This causes lost updates and nondeterministic results unless the partial sums are reduced or synchronized.
- **When is `collapse(2)` useful?** It increases available tasks from `N` rows to `N²` output cells. It matters most when `N` is small relative to thread count or rows are imbalanced. With `N=256`, `512`, or `1024` and at most eight threads, the outer loop already offers enough work; results show only small differences between outer and collapsed loops.
- **Why does `i-k-j` help?** In row-major arrays, the `j` loop traverses contiguous elements of `B[k][j]` and `C[i][j]`, improving locality and cache usage. At `N=512`, `ikj_serial` took about 0.059–0.072 s versus 0.388–0.437 s for the original serial loop. At `N=1024`, `ikj_serial` took 0.483–0.570 s versus 3.229–4.386 s, demonstrating improved memory locality.

**Task 5 benchmark coverage:** Complete for the specified sizes and thread counts.

## Overall conclusions

1. **Correctness:** Every supplied correct serial/parallel implementation reported `PASS`. The broken shared-sum implementation gave incorrect and variable results for multiple threads, confirming the race-condition demonstration.
2. **Scaling:** More threads do not always mean less time. Vector and matrix addition show modest gains because of low computation per memory access; matrix multiplication benefits more from parallel work.
3. **Reductions:** `reduction` is preferable to unsynchronized shared updates and generally avoids per-element lock contention.
4. **Loop selection:** Parallelizing independent output rows/cells is safe. Parallelizing an accumulation dimension without a reduction is unsafe.
5. **Memory locality:** The `i-k-j` matrix multiplication order substantially improves performance compared with naive `i-j-k`, especially at `N=512` and `N=1024`.
6. **Timing caution:** Several measured intervals are ~1 ms or zero; apparent efficiencies above 1 should not be treated as conclusive without repeated trials and more precise timing.

## Commands used

```powershell
make
./task1 10000000 1
./task1 10000000 2
./task1 10000000 4
./task1 10000000 8
# The same thread counts were used for task2 and task3 at N=10000000,
# task4 at N=8 and N=4096, and task5 at N=256, N=512 and N=1024.
```

## Submission checklist

- [x] Correctness outputs shown for the tested configurations of all five tasks
- [x] Tables for all supplied thread-count measurements
- [x] Speedup and efficiency for measured nonzero-time cases
- [x] Answers to all theory-question blocks
- [ ] Attach all five `.cpp` files, shared header, and Makefile
- [ ] Add CPU model, core count, RAM, OS and compiler version
- [ ] Run missing Task 1 sizes: `10^5`, `10^6`, `10^8`
- [ ] Run missing Task 2 and 3 sizes: `10^8`
- [x] Task 5 `N=1024` for threads 1, 2, 4, 8
- [ ] Repeat very short measurements (especially Task 4 `N=8`) for meaningful timing
- [ ] Add largest-N vector speedup plot or table
- [ ] Submit individually on Google Classroom
