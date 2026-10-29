#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./10_prime_count [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    // Counting primes: each number can be tested without changing others.
    const int N=100000; long long serial=0,parallel=0;
    auto prime=[](int n){
        if(n<2) return false;
        for(int d=2;1LL*d*d<=n;d++) if(n%d==0) return false;
        return true;
    };
    start=omp_get_wtime();
    for(int n=2;n<=N;n++) if(prime(n)) serial++;
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    #pragma omp parallel for reduction(+:parallel) schedule(dynamic,100)
    for(int n=2;n<=N;n++) if(prime(n)) parallel++;
    parallelTime=omp_get_wtime()-start;
    pass=(serial==parallel && serial==9592);

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
