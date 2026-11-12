# Beginner-Friendly OpenMP Reference Guide & Handbook

This guide is designed to be **simple, practical, and intuitive for beginners** learning parallel programming with OpenMP in C and C++.

---

## 🌟 Beginner's Quick-Start: The 5 Directives You Need 90% of the Time

### 1. Parallel For Loop (Divide loop iterations across CPU cores)
```cpp
// Distributes i = 0 to N-1 across available threads
#pragma omp parallel for num_threads(4)
for (int i = 0; i < N; i++) {
    c[i] = a[i] + b[i];
}
```

### 2. Parallel Reduction (Summing, Min, Max without race conditions)
```cpp
long long total = 0;
// Each thread calculates its own local sum, then adds them up safely
#pragma omp parallel for num_threads(4) reduction(+:total)
for (int i = 0; i < N; i++) {
    total += a[i];
}
```

### 3. Nested 2D Loops (`collapse(2)`)
```cpp
// Parallelizes both rows and columns together
#pragma omp parallel for collapse(2) num_threads(4)
for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
        grid[r][c] = compute(r, c);
    }
}
```

### 4. Atomic Counter (`#pragma omp atomic`)
```cpp
// Super fast and lock-free: increments shared variable safely
#pragma omp atomic update
degree[u]++;

// Atomically fetch the current slot and increment (like in CSR)
int slot;
#pragma omp atomic capture
slot = current_pos[u]++;
```

### 5. Critical Section (`#pragma omp critical`)
```cpp
// Only ONE thread enters this block at a time (use for safe updates)
#pragma omp critical
{
    if (new_dist < dist[v]) {
        dist[v] = new_dist;
    }
}
```

---

## ⚠️ 3 Golden Rules for Beginners to Avoid Bugs

1. **Rule 1: Always declare loop index `int i` INSIDE the loop header:**
   - ✅ `for (int i = 0; i < N; i++)` $\rightarrow$ `i` is automatically private to each thread.
   - ❌ `int i; for (i = 0; ...)` $\rightarrow$ all threads share `i`, overwriting each other!
2. **Rule 2: Never let multiple threads write to the same variable without protection:**
   - Use `reduction(+:sum)` for totals.
   - Use `#pragma omp atomic` for counters.
   - Use `#pragma omp critical` for compound if-statements or complex updates.
3. **Rule 3: Always compile with `-fopenmp`:**
   ```bash
   g++ -O3 -fopenmp -std=c++17 -o program.exe program.cpp
   ./program.exe
   ```

---

## Table of Contents
1. [The OpenMP Execution & Memory Model](#1-the-openmp-execution--memory-model)
2. [Core Directives & Work-Sharing](#2-core-directives--work-sharing)
3. [Data-Sharing Clauses](#3-data-sharing-clauses)
4. [Synchronization & Mutual Exclusion](#4-synchronization--mutual-exclusion)
5. [Reductions (Built-in and Custom)](#5-reductions-built-in-and-custom)
6. [Loop Scheduling Strategies](#6-loop-scheduling-strategies)
7. [Task-Based Parallelism](#7-task-based-parallelism)
8. [Critical Performance Pitfalls & Solutions](#8-critical-performance-pitfalls--solutions)
9. [OpenMP Runtime API Reference](#9-openmp-runtime-api-reference)
10. [Compilation, Flags & Environment Variables](#10-compilation-flags--environment-variables)
11. [Canonical High-Performance Design Patterns](#11-canonical-high-performance-design-patterns)

---

## 1. The OpenMP Execution & Memory Model

### Fork-Join Execution Model
OpenMP programs start as a single thread of execution, termed the **Master Thread** (thread `0`). When the master thread encounters a `#pragma omp parallel` construct:
1. **Fork**: A team of worker threads is created (or retrieved from an internal idle thread pool).
2. **Execute**: The statements enclosed inside the parallel region are executed concurrently by every thread in the team.
3. **Join**: At the end of the parallel block, an implicit barrier synchronizes all threads. Worker threads suspend or return to the pool, and only the master thread continues sequentially.

```
Master Thread ───────────────────► [Fork] ──► Thread 0 (Master) ──► [Join / Barrier] ──► Master Thread
                                       ├──► Thread 1 ─────────┤
                                       ├──► Thread 2 ─────────┤
                                       └──► Thread 3 ─────────┘
```

### Shared-Memory Architecture
- **Shared Memory**: All threads in a team share a single common address space. Any thread can read or write variables declared in the enclosing shared scope.
- **Private Memory**: Each thread has its own stack. Variables declared inside a parallel region or marked with private clauses are local to that thread's stack.
- **Cache Coherency**: Multi-core CPUs maintain hardware cache coherence (e.g. MESI protocol). Writes to shared variables by one core invalidate or update cache lines in other cores. Unmanaged writes lead to **data races** and **false sharing**.

---

## 2. Core Directives & Work-Sharing

Directives use the format: `#pragma omp <directive-name> [clauses...]`

### 2.1 `#pragma omp parallel`
Creates a parallel region. Every thread executes the identical block of code.
```cpp
#pragma omp parallel num_threads(4)
{
    int tid = omp_get_thread_num();
    int total = omp_get_num_threads();
    std::cout << "Thread " << tid << " of " << total << "\n";
}
```

### 2.2 `#pragma omp for` and `#pragma omp parallel for`
Splits loop iterations across the thread team.
- The loop control variable is implicitly private to each thread.
- Loops must be in **canonical form** (e.g. `for (int i = start; i < end; i += step)` where bounds are invariant).
```cpp
#pragma omp parallel for schedule(static)
for (int i = 0; i < N; ++i) {
    c[i] = a[i] + b[i];
}
```

### 2.3 `#pragma omp sections`
Divides non-iterative, independent blocks of work among available threads:
```cpp
#pragma omp parallel sections
{
    #pragma omp section
    { compute_phase_A(); }

    #pragma omp section
    { compute_phase_B(); }
}
```

### 2.4 `#pragma omp single` vs `#pragma omp master` / `masked`
- `#pragma omp single`: Executed by the **first thread** that reaches it. All other threads wait at an implicit barrier (unless `nowait` is specified).
- `#pragma omp master` (or `masked` in OpenMP 5.1): Executed **strictly by Thread 0**. Has **NO implicit barrier** at the end.

```cpp
#pragma omp parallel
{
    do_work();

    #pragma omp single
    {
        // Safe place to perform I/O, allocate shared memory, or swap pointers
        swap_buffers();
    } // Implicit barrier ensures all threads see updated buffers before continuing

    do_next_work();
}
```

---

## 3. Data-Sharing Clauses

Selecting the correct data-sharing attribute is vital to avoid data races.

| Clause | Description | Initialization | Final Value Saved to Master? |
| :--- | :--- | :--- | :--- |
| `shared(x)` | Single shared memory address for all threads. | Uses pre-existing value. | Retains modifications. |
| `private(x)` | Local uninitialized variable on each thread's stack. | **Uninitialized** (garbage value). | Discarded after region. |
| `firstprivate(x)` | Thread-private copy initialized with original value. | Initialized from master's variable. | Discarded after region. |
| `lastprivate(x)` | Thread-private copy copied back to master after loop. | Same as private (or firstprivate). | Value from sequentially last iteration copied back. |
| `default(none)` | Forces explicit qualification of all variables. | N/A | N/A |

### Best Practice: Always Use `default(none)`
In production C++, omitting default scoping frequently introduces silent race conditions on counters and temporary variables:
```cpp
// BEST PRACTICE: Enforces compile-time checking of every variable
#pragma omp parallel for default(none) shared(a, b, c, N) private(temp) schedule(static)
for (int i = 0; i < N; ++i) {
    double temp = a[i] * 2.0;
    c[i] = temp + b[i];
}
```

---

## 4. Synchronization & Mutual Exclusion

### 4.1 Critical Section (`#pragma omp critical`)
Guarantees mutual exclusion: only one thread executes the block at a time.
```cpp
#pragma omp critical (optional_name)
{
    shared_counter++;
}
```
> [!CAUTION]
> Critical sections serialize execution and have heavy OS/hardware locking overhead. Do not place critical sections inside tight loops!

### 4.2 Atomic Operations (`#pragma omp atomic`)
Hardware-accelerated lock-free updates. Uses atomic CPU instructions (e.g. `LOCK XADD` on x86-64), typically **10x to 50x faster** than a critical section.

Supported atomic clauses:
- `read`: Atomically load a variable: `#pragma omp atomic read \n v = x;`
- `write`: Atomically store a variable: `#pragma omp atomic write \n x = expr;`
- `update` (default): Atomically modify a variable (`x++`, `x += val`, etc.).
- `capture`: Atomically update and fetch previous or new value:
```cpp
int slot;
#pragma omp atomic capture
slot = current_index++; // Safely acquires unique slot without locks
```

### 4.3 Barriers (`#pragma omp barrier`)
Synchronizes all threads in the enclosing team. No thread can proceed past the barrier until all threads have reached it.

### 4.4 Disabling Barriers with `nowait`
Many work-sharing constructs have an implicit barrier at the closing brace. Use `nowait` to eliminate the barrier when subsequent code is independent:
```cpp
#pragma omp parallel
{
    #pragma omp for nowait
    for (int i = 0; i < N; ++i) { work_A(i); }

    #pragma omp for
    for (int i = 0; i < N; ++i) { work_B(i); }
}
```

### 4.5 Low-Level Locks (`omp_lock_t`)
Used for complex synchronization structures (e.g. fine-grained bucket locks, graphs):
```cpp
omp_lock_t lock;
omp_init_lock(&lock);

#pragma omp parallel
{
    omp_set_lock(&lock);
    // Protected shared resource manipulation
    omp_unset_lock(&lock);
}

omp_destroy_lock(&lock);
```

---

## 5. Reductions (Built-in and Custom)

The `reduction(op: var)` clause enables race-free parallel aggregation. OpenMP allocates a thread-private accumulator initialized to the operator's identity (e.g. `0` for `+`, `1` for `*`, `INF` for `min`), accumulates locally, and merges results at the end.

### 5.1 Standard Reduction
```cpp
double total_sum = 0.0;
double min_val = 1e9;

#pragma omp parallel for reduction(+:total_sum) reduction(min:min_val)
for (int i = 0; i < N; ++i) {
    total_sum += array[i];
    if (array[i] < min_val) min_val = array[i];
}
```

### 5.2 OpenMP 5.0 Inscan Reduction (Parallel Prefix Sum)
OpenMP 5.0 introduced native hardware scan loops:
```cpp
int running_sum = 0;
#pragma omp parallel for reduction(inscan, +:running_sum)
for (int i = 0; i < N; ++i) {
    prefix_arr[i] = running_sum;
    #pragma omp scan exclusive(running_sum)
    running_sum += input_arr[i];
}
```

---

## 6. Loop Scheduling Strategies

Loop iterations are assigned to threads based on the `schedule` clause: `schedule(kind [, chunk_size])`.

```
STATIC:     [ T0 (chunk) ][ T1 (chunk) ][ T2 (chunk) ][ T3 (chunk) ][ T0 ]...
DYNAMIC:    Threads request chunks from shared queue as they finish work.
GUIDED:     Chunk sizes start large and exponentially decrease: [ 1000 ][ 500 ][ 250 ]...
```

| Schedule | Mechanics | Overhead | Best Use Case |
| :--- | :--- | :--- | :--- |
| `static` | Iterations divided evenly upfront (block or round-robin chunk). | **Lowest** (deterministic, zero runtime coordination). | Uniform workloads where every iteration takes identical CPU cycles (e.g. dense matrix math, Game of Life). |
| `dynamic` | Work queue: idle threads grab next `chunk` when finished. | **Higher** (atomic dispatch on global queue). | Unbalanced / irregular workloads (e.g. Mandelbrot set, ray tracing, power-law graphs). |
| `guided` | Work queue with exponentially shrinking chunks down to `chunk_size`. | **Moderate** (fewer queue accesses than dynamic). | Non-uniform loops where initial large chunks amortize synchronization. |
| `runtime` | Configured at launch via `OMP_SCHEDULE` env variable. | Configurable. | Performance tuning and testing without recompilation. |
| `auto` | Compiler / runtime selects heuristic. | Variable. | General optimization. |

### Nested Loops and `collapse(N)`
Combines $N$ tightly nested loops into a single linear iteration space, dramatically increasing parallelism:
```cpp
#pragma omp parallel for collapse(2) schedule(static)
for (int i = 0; i < Rows; ++i) {
    for (int j = 0; j < Cols; ++j) {
        matrix[i][j] = compute(i, j);
    }
}
```

---

## 7. Task-Based Parallelism

When parallelism is irregular, recursive, or cannot be expressed as a count-controlled `for` loop, OpenMP **tasks** provide dynamic work-stealing parallelism.

### 7.1 Recursive Fibonacci with Tasks
```cpp
int fib(int n) {
    if (n < 2) return n;
    // Sequential cutoff to prevent task explosion
    if (n < 20) return fib_seq(n);

    int x, y;
    #pragma omp task shared(x)
    x = fib(n - 1);

    #pragma omp task shared(y)
    y = fib(n - 2);

    #pragma omp taskwait // Wait for child tasks to finish
    return x + y;
}

int main() {
    int res = 0;
    #pragma omp parallel
    {
        #pragma omp single
        res = fib(35);
    }
}
```

---

## 8. Critical Performance Pitfalls & Solutions

### 8.1 False Sharing (The #1 Silent Performance Killer)
- **Problem**: Modern CPUs load and invalidate cache in 64-byte chunks (**Cache Lines**). If Thread 0 writes to `arr[0]` and Thread 1 writes to `arr[1]`, both variables share the same 64-byte cache line. The CPU cores continually bounce the cache line between their L1 caches, destroying parallel scaling.
- **Solution A**: Use thread-private local scalar variables, then write to shared memory once.
- **Solution B**: Pad array structures to 64 bytes (`alignas(64)`).

```cpp
// BAD (Severe False Sharing):
int count[NUM_THREADS];
#pragma omp parallel
{
    int tid = omp_get_thread_num();
    for (int i = 0; i < N; ++i) count[tid]++; // Rapid L1 cache-line bouncing!
}

// GOOD (Thread-local variable):
#pragma omp parallel
{
    int local_count = 0; // Stored in private register / stack
    #pragma omp for
    for (int i = 0; i < N; ++i) local_count++;

    #pragma omp atomic
    global_total += local_count;
}
```

### 8.2 Persistent Parallel Regions vs Fine-Grained Fork-Join
- **Problem**: Calling `#pragma omp parallel for` repeatedly inside a tight outer loop (e.g. 1000 generations in Game of Life) forces the runtime to fork/join threads 1000 times.
- **Solution**: Create the parallel region **outside** the outer loop once. Use `#pragma omp for` and `#pragma omp single` internally.

### 8.3 Thread Contention on Graph/Sparse Updates
- **Problem**: In sparse matrix or graph construction, thousands of edges target arbitrary rows. Doing `#pragma omp atomic` on popular nodes (hubs) creates high atomic stall cycles.
- **Solution**: Use **Thread-Private Histograms** + parallel reduction + parallel prefix sum (as implemented in our `CSR_Parallel` module).

---

## 9. OpenMP Runtime API Reference

Include header: `#include <omp.h>`

| Function | Signature | Purpose |
| :--- | :--- | :--- |
| `omp_get_thread_num()` | `int ()` | Returns unique thread ID ($0 \dots T-1$). |
| `omp_get_num_threads()` | `int ()` | Returns total active threads in current team. |
| `omp_get_max_threads()` | `int ()` | Returns maximum thread capacity configured. |
| `omp_set_num_threads(n)`| `void (int)` | Dynamically overrides default thread count. |
| `omp_get_wtime()` | `double ()` | High-precision wall-clock timer in seconds. |
| `omp_get_wtick()` | `double ()` | Resolution of `omp_get_wtime()`. |
| `omp_in_parallel()` | `int ()` | Returns non-zero if called inside parallel region. |
| `omp_set_dynamic(flag)` | `void (int)` | Enables/disables runtime dynamic thread sizing. |

---

## 10. Compilation, Flags & Environment Variables

### Compiler Flags
- **GCC / MinGW**:
  ```bash
  g++ -O3 -fopenmp -std=c++17 -Wall -o app.exe main.cpp
  ```
- **Clang / LLVM**:
  ```bash
  clang++ -O3 -fopenmp -std=c++17 -o app.exe main.cpp
  ```
- **MSVC (Visual C++)**:
  ```cmd
  cl.exe /O2 /openmp /EHsc main.cpp
  ```

### Runtime Environment Variables
- `OMP_NUM_THREADS=8`: Sets the default number of threads.
- `OMP_SCHEDULE="dynamic,64"`: Sets default loop scheduling.
- `OMP_PROC_BIND=true` (or `close`, `spread`): Binds threads to physical CPU cores, preventing OS context thread migration.
- `OMP_PLACES=cores`: Maps threads to physical CPU cores.
- `OMP_DYNAMIC=false`: Prevents the OpenMP runtime from reducing threads under system load.

---

## 11. Canonical High-Performance Design Patterns

### Pattern 1: Parallel Exclusive Prefix Sum (Block Scan)
Used in CSR construction, stream compaction, and partitioning:
```cpp
void parallel_prefix_sum(const std::vector<int>& in, std::vector<int>& out, int threads) {
    int n = in.size();
    out.resize(n + 1, 0);
    std::vector<int> chunk_sums(threads, 0), chunk_offsets(threads, 0);

    // Step 1: Thread-local scan
    #pragma omp parallel num_threads(threads)
    {
        int tid = omp_get_thread_num();
        int chunk = (n + threads - 1) / threads;
        int s = std::min(tid * chunk, n);
        int e = std::min(s + chunk, n);
        int acc = 0;
        for (int i = s; i < e; ++i) {
            out[i] = acc;
            acc += in[i];
        }
        chunk_sums[tid] = acc;
    }

    // Step 2: Sequential scan over chunk totals
    int run = 0;
    for (int t = 0; t < threads; ++t) {
        chunk_offsets[t] = run;
        run += chunk_sums[t];
    }
    out[n] = run;

    // Step 3: Add offsets in parallel
    #pragma omp parallel num_threads(threads)
    {
        int tid = omp_get_thread_num();
        int chunk = (n + threads - 1) / threads;
        int s = std::min(tid * chunk, n);
        int e = std::min(s + chunk, n);
        int off = chunk_offsets[tid];
        for (int i = s; i < e; ++i) out[i] += off;
    }
}
```

### Pattern 2: Persistent Thread Pool for 2D Stencils (Game of Life)
```cpp
#pragma omp parallel num_threads(threads)
{
    for (int gen = 0; gen < generations; ++gen) {
        #pragma omp for schedule(static)
        for (int r = 0; r < Rows; ++r) {
            compute_stencil_row(r, curr_buffer, next_buffer);
        }

        #pragma omp single
        {
            std::swap(curr_buffer, next_buffer);
        }
    }
}
```

### Pattern 3: Lock-Free Atomic Compare-And-Swap (CAS) Claiming
```cpp
// Atomically mark unvisited node (-1 -> new_dist)
int expected = -1;
if (__atomic_compare_exchange_n(&dist[v], &expected, new_dist,
                                 false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
    // Successfully claimed: add to frontier
}
```

