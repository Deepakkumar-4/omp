#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./01_min_max [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    // Reduction gives each worker its own minimum and maximum.
    const int N=1000000; vector<int> a(N);
    for (int i=0;i<N;i++) a[i]=(i*17)%100009-50000;
    int smin=INT_MAX,smax=INT_MIN,pmin=INT_MAX,pmax=INT_MIN;
    start=omp_get_wtime();
    for(int x:a){smin=min(smin,x);smax=max(smax,x);}
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    #pragma omp parallel for reduction(min:pmin) reduction(max:pmax)
    for(int i=0;i<N;i++){pmin=min(pmin,a[i]);pmax=max(pmax,a[i]);}
    parallelTime=omp_get_wtime()-start;
    pass=(smin==pmin && smax==pmax);

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
