#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./06_matrix_transpose [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    const int N=700; vector<int> a(N*N),serial(N*N),parallel(N*N);
    for(int i=0;i<N*N;i++) a[i]=i;
    start=omp_get_wtime();
    for(int i=0;i<N;i++) for(int j=0;j<N;j++) serial[j*N+i]=a[i*N+j];
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    // Each (i,j) writes a different transposed location.
    #pragma omp parallel for collapse(2)
    for(int i=0;i<N;i++) for(int j=0;j<N;j++) parallel[j*N+i]=a[i*N+j];
    parallelTime=omp_get_wtime()-start;
    pass=(serial==parallel);

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
