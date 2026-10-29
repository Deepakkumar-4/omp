#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./09_numerical_integration [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    // Midpoint rule integrates x*x on [0,1]; correct answer is 1/3.
    const long long N=2000000; double serial=0,parallel=0;
    double width=1.0/N;
    start=omp_get_wtime();
    for(long long i=0;i<N;i++){double x=(i+0.5)*width;serial+=x*x*width;}
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    #pragma omp parallel for reduction(+:parallel)
    for(long long i=0;i<N;i++){double x=(i+0.5)*width;parallel+=x*x*width;}
    parallelTime=omp_get_wtime()-start;
    pass=(fabs(serial-parallel)<1e-10 && fabs(parallel-1.0/3)<1e-8);

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
