#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <omp.h>
using namespace std;
int main(int argc,char** argv) {
    int n=argc>1?atoi(argv[1]):128;
    int threads=argc>2?atoi(argv[2]):4;
    if(n<=0||threads<=0)return 1;
    vector<double> A(n*n),B(n*n),serial(n*n,0),parallel(n*n,0);
    for(int i=0;i<n*n;i++){A[i]=i%7;B[i]=i%5;}
    double t=omp_get_wtime();
    // Each (i,j) output has its own accumulation over k.
    for(int i=0;i<n;i++)for(int j=0;j<n;j++) {
        double sum=0;
        for(int k=0;k<n;k++)sum+=A[i*n+k]*B[k*n+j];
        serial[i*n+j]=sum;
    }
    double ts=omp_get_wtime()-t;
    t=omp_get_wtime();
    // collapse(2) distributes output cells (i,j), never the k sum.
    #pragma omp parallel for collapse(2) num_threads(threads)
    for(int i=0;i<n;i++)for(int j=0;j<n;j++) {
        double sum=0;
        for(int k=0;k<n;k++)sum+=A[i*n+k]*B[k*n+j];
        parallel[i*n+j]=sum;
    }
    double tp=omp_get_wtime()-t;
    bool ok=true;
    for(int i=0;i<n*n;i++)if(fabs(serial[i]-parallel[i])>1e-9){ok=false;break;}
    cout<<"Matrix multiplication: serial="<<ts<<" parallel="<<tp<<" verification="<<(ok?"PASS":"FAIL")<<'\n';
    return ok?0:1;
}
