#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./04_prefix_sum [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    // Inclusive scan: output[i] = a[0] + ... + a[i].
    const int N=100000; vector<long long> a(N,1),serial(N),parallel(N);
    start=omp_get_wtime();
    long long sum=0;
    for(int i=0;i<N;i++){sum+=a[i];serial[i]=sum;}
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    // Chunk-level scan: first compute a sum for each chunk.
    int T=threads; vector<long long> totals(T,0),offset(T,0);
    #pragma omp parallel for schedule(static)
    for(int t=0;t<T;t++){
        int begin=(long long)t*N/T,end=(long long)(t+1)*N/T;
        long long part=0;
        for(int i=begin;i<end;i++){part+=a[i];parallel[i]=part;}
        totals[t]=part;
    }
    // Prefix of chunk totals is small; compute it sequentially.
    long long running=0;
    for(int t=0;t<T;t++){offset[t]=running;running+=totals[t];}
    #pragma omp parallel for schedule(static)
    for(int t=0;t<T;t++){
        int begin=(long long)t*N/T,end=(long long)(t+1)*N/T;
        for(int i=begin;i<end;i++) parallel[i]+=offset[t];
    }
    parallelTime=omp_get_wtime()-start;
    pass=(serial==parallel);

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
