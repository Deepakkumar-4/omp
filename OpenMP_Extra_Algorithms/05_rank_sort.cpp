#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./05_rank_sort [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    // Rank sort is simple to parallelize: each output rank is independent.
    // Break duplicate ties by original index to give every item a unique slot.
    const int N=900; vector<int> a(N),serial(N),parallel(N);
    for(int i=0;i<N;i++) a[i]=(i*37)%101;
    start=omp_get_wtime();
    for(int i=0;i<N;i++){
        int rank=0;
        for(int j=0;j<N;j++) if(a[j]<a[i]||(a[j]==a[i]&&j<i)) rank++;
        serial[rank]=a[i];
    }
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    #pragma omp parallel for schedule(static)
    for(int i=0;i<N;i++){
        int rank=0;
        for(int j=0;j<N;j++) if(a[j]<a[i]||(a[j]==a[i]&&j<i)) rank++;
        parallel[rank]=a[i]; // Unique rank -> no race.
    }
    parallelTime=omp_get_wtime()-start;
    pass=(serial==parallel && is_sorted(parallel.begin(),parallel.end()));

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
