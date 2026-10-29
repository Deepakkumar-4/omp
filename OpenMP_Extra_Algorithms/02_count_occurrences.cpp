#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./02_count_occurrences [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    const int N=1000000,target=7; vector<int> a(N);
    for(int i=0;i<N;i++) a[i]=i%13;
    long long serial=0,parallel=0;
    start=omp_get_wtime();
    for(int x:a) if(x==target) ++serial;
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    // Without reduction, parallel++ would be a data race.
    #pragma omp parallel for reduction(+:parallel)
    for(int i=0;i<N;i++) if(a[i]==target) ++parallel;
    parallelTime=omp_get_wtime()-start;
    pass=(serial==parallel);

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
