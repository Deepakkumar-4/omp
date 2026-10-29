# Extra OpenMP Practice Algorithms (C++17)

These 12 programs **extend** the first eight-program ZIP. Each includes serial and parallel code, comments, a correctness check and timing. They are standalone examples (not optimized libraries).

## Compile all

    make
    make test

## Run one

    ./03_histogram 4

Windows PowerShell (MinGW):

    g++ -std=c++17 -O2 -Wall -Wextra -fopenmp 03_histogram.cpp -o 03_histogram.exe
    .\03_histogram.exe 4

## Topics

1. min/max (min/max reductions)
2. count occurrences (sum reduction)
3. histogram (thread-local bins + short critical merge)
4. prefix sum (chunk sums, offsets, barriers)
5. rank sort (independent ranks; simple O(N^2) method)
6. matrix transpose (collapse(2))
7. 2D convolution (collapse(2))
8. dot product (reduction)
9. numerical integration (reduction)
10. prime counting (dynamic scheduling + reduction)
11. vector operations (parallel for)
12. Monte Carlo pi (deterministic sampling + reduction)

The small examples may run slower with OpenMP because creating and coordinating workers costs time.
Always check correctness first. Floating-point reductions can differ slightly due to addition order.
