#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <climits>
#include <omp.h>
using namespace std;

// Run: ./07_convolution_2d [number_of_threads]. Compare serial and OpenMP results.
int main(int argc, char** argv) {
    int threads = argc > 1 ? max(1, atoi(argv[1])) : 4;
    omp_set_num_threads(threads);
    double start = 0, serialTime = 0, parallelTime = 0;
    bool pass = false;
    const int N=400; vector<int> a(N*N),serial(N*N,0),parallel(N*N,0);
    for(int i=0;i<N*N;i++) a[i]=i%19;
    int kernel[3][3]={{1,0,-1},{1,0,-1},{1,0,-1}};
    start=omp_get_wtime();
    for(int i=1;i<N-1;i++) for(int j=1;j<N-1;j++){
        int value=0;
        for(int di=-1;di<=1;di++) for(int dj=-1;dj<=1;dj++) value+=a[(i+di)*N+(j+dj)]*kernel[di+1][dj+1];
        serial[i*N+j]=value;
    }
    serialTime=omp_get_wtime()-start;
    start=omp_get_wtime();
    // Every output pixel can be calculated independently.
    #pragma omp parallel for collapse(2)
    for(int i=1;i<N-1;i++) for(int j=1;j<N-1;j++){
        int value=0; // Private accumulator for this pixel.
        for(int di=-1;di<=1;di++) for(int dj=-1;dj<=1;dj++) value+=a[(i+di)*N+(j+dj)]*kernel[di+1][dj+1];
        parallel[i*N+j]=value;
    }
    parallelTime=omp_get_wtime()-start;
    pass=(serial==parallel);

    cout << "threads=" << threads << " serial=" << serialTime
         << "s parallel=" << parallelTime << "s verification="
         << (pass ? "PASS" : "FAIL") << '\n';
    if (parallelTime > 0) cout << "speedup=" << serialTime / parallelTime << '\n';
    return pass ? 0 : 1;
}
