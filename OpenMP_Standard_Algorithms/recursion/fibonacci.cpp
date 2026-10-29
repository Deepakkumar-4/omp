#include <iostream>
#include <cstdlib>
#include <omp.h>
using namespace std;
long long fibSerial(int n) {
    if(n<=1)return n;
    return fibSerial(n-1)+fibSerial(n-2);
}
long long fibParallel(int n,int cutoff) {
    if(n<=1)return n;
    if(n<=cutoff)return fibSerial(n); // Avoid creating tiny tasks.
    long long left=0,right=0;
    #pragma omp task shared(left) firstprivate(n,cutoff)
    left=fibParallel(n-1,cutoff);
    #pragma omp task shared(right) firstprivate(n,cutoff)
    right=fibParallel(n-2,cutoff);
    #pragma omp taskwait
    return left+right;
}
int main(int argc,char** argv) {
    int n=argc>1?atoi(argv[1]):25;
    int threads=argc>2?atoi(argv[2]):4;
    int cutoff=argc>3?atoi(argv[3]):18;
    if(n<0||n>40||threads<=0||cutoff<1)return 1;
    double t=omp_get_wtime();long long serial=fibSerial(n);
    double ts=omp_get_wtime()-t;
    long long parallel=0;
    t=omp_get_wtime();
    #pragma omp parallel num_threads(threads)
    {
        #pragma omp single
        parallel=fibParallel(n,cutoff);
    }
    double tp=omp_get_wtime()-t;
    bool ok=serial==parallel;
    cout<<"Fibonacci("<<n<<")="<<parallel<<" serial="<<ts<<" parallel="<<tp<<" verification="<<(ok?"PASS":"FAIL")<<'\n';
    return ok?0:1;
}
