# OpenMP Standard Algorithms — Serial and Parallel C++

Every file contains a serial version, an OpenMP version, comments, timers, and a correctness check. The code is intended for learning, not maximum performance.

## Build and test

```bash
make
make test
```

Compile any one file on Windows PowerShell using:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -fopenmp sorting/merge_sort.cpp -o merge_sort.exe
.\merge_sort.exe 100000 4 2048
```

Most programs accept `size threads`; recursive sorting also accepts `cutoff`. Fibonacci accepts `n threads cutoff`. Avoid large values for odd-even sorting or naive Fibonacci.

## Contents

- `searching/linear_search.cpp`: reduction to find first matching index.
- `searching/binary_search_queries.cpp`: parallel independent binary-search queries.
- `arrays/reduction_sum.cpp`: summation with reduction.
- `sorting/odd_even_sort.cpp`: independent swap pairs and phase barriers.
- `sorting/merge_sort.cpp`: recursive tasks with `taskwait` before merge.
- `sorting/quick_sort.cpp`: partition then recursive tasks.
- `recursion/fibonacci.cpp`: a teaching example of recursive task creation.
- `matrices/matrix_multiplication.cpp`: nested-loop `collapse(2)`.

OpenMP scheduling is nondeterministic; speedup is not guaranteed. Compare serial and parallel times on your own hardware. All examples use C++17 and GCC OpenMP.
