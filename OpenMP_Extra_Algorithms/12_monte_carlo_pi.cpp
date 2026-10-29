#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./12_monte_carlo_pi [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    // Reproducible samples: each i generates the same point in both modes.
    const int N=1000000; long long serial=0,parallel=0;
    auto sample=[](unsigned long long x){
        x+=0x9e3779b97f4a7c15ULL;
        x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
        x=(x^(x>>27))*0x94d049bb133111ebULL;
        return x^(x>>31);
    };
    auto inside=[&](int i){
        double x=(sample(2ULL*i)>>11)*(1.0/9007199254740992.0);
        double y=(sample(2ULL*i+1)>>11)*(1.0/9007199254740992.0);
        return x*x+y*y<=1.0;
    };
    start=omp_get_wtime();
    for(int i=0;i<N;i++) if(inside(i)) serial++;
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    #pragma omp parallel for reduction(+:parallel)
    for(int i=0;i<N;i++) if(inside(i)) parallel++;
    parallelTime=omp_get_wtime()-start;
    pass=(serial==parallel);
    cout << "pi estimate=" << 4.0*parallel/N << '\n';

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
