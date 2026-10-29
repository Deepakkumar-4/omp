#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./11_vector_operations [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    const int N=1000000; vector<int> a(N),b(N),serial(N),parallel(N);
    for(int i=0;i<N;i++){a[i]=i%97;b[i]=i%41;}
    start=omp_get_wtime();
    for(int i=0;i<N;i++) serial[i]=3*a[i]+2*b[i];
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    #pragma omp parallel for
    for(int i=0;i<N;i++) parallel[i]=3*a[i]+2*b[i];
    parallelTime=omp_get_wtime()-start;
    pass=(serial==parallel);

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
