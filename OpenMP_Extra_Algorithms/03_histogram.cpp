#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./03_histogram [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    const int N=1000000,BINS=16; vector<int> a(N);
    for(int i=0;i<N;i++) a[i]=(i*29)%BINS;
    vector<long long> serial(BINS,0),parallel(BINS,0);
    start=omp_get_wtime();
    for(int x:a) ++serial[x];
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    // A private histogram avoids an atomic update on every element.
    #pragma omp parallel
    {
        vector<long long> local(BINS,0);
        #pragma omp for nowait
        for(int i=0;i<N;i++) ++local[a[i]];
        // Merge partial histograms; each bin is updated safely.
        #pragma omp critical
        for(int b=0;b<BINS;b++) parallel[b]+=local[b];
    }
    parallelTime=omp_get_wtime()-start;
    pass=(serial==parallel);

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
