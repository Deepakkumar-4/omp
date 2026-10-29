#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <omp.h>
using namespace std;
// Lomuto partition: pivot is last element; return its final position.
int partitionArray(vector<int>& a,int l,int r) {
    int pivot=a[r],i=l-1;
    for(int j=l;j<r;j++) if(a[j]<=pivot) swap(a[++i],a[j]);
    swap(a[i+1],a[r]);
    return i+1;
}
void quickSerial(vector<int>& a,int l,int r) {
    if(l>=r)return;
    int p=partitionArray(a,l,r);
    quickSerial(a,l,p-1); quickSerial(a,p+1,r);
}
void quickParallel(vector<int>& a,int l,int r,int cutoff) {
    if(l>=r)return;
    if(r-l+1<=cutoff){quickSerial(a,l,r);return;}
    int p=partitionArray(a,l,r);
    // Partition first, then left/right sides can run as separate tasks.
    #pragma omp task shared(a) firstprivate(l,p,cutoff)
    quickParallel(a,l,p-1,cutoff);
    #pragma omp task shared(a) firstprivate(p,r,cutoff)
    quickParallel(a,p+1,r,cutoff);
    #pragma omp taskwait
}
int main(int argc,char** argv) {
    int n=argc>1?atoi(argv[1]):100000;
    int threads=argc>2?atoi(argv[2]):4;
    int cutoff=argc>3?atoi(argv[3]):2048;
    if(n<=0||threads<=0||cutoff<=0)return 1;
    mt19937 rng(42);
    vector<int> serial(n),parallel;
    for(int& x:serial)x=int(rng()%1000000);
    parallel=serial;
    double t=omp_get_wtime();quickSerial(serial,0,n-1);
    double ts=omp_get_wtime()-t;
    t=omp_get_wtime();
    #pragma omp parallel num_threads(threads)
    {
        #pragma omp single
        quickParallel(parallel,0,n-1,cutoff);
    }
    double tp=omp_get_wtime()-t;
    bool ok=serial==parallel&&is_sorted(parallel.begin(),parallel.end());
    cout<<"Quick sort: serial="<<ts<<" parallel="<<tp<<" verification="<<(ok?"PASS":"FAIL")<<'\n';
    return ok?0:1;
}
