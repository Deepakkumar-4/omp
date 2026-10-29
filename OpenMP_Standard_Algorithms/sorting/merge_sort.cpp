#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <omp.h>
using namespace std;
void mergeHalves(vector<int>& a, vector<int>& temp, int l, int m, int r) {
    int i=l,j=m+1,k=l;
    while (i<=m && j<=r) temp[k++] = (a[i]<=a[j]) ? a[i++] : a[j++];
    while (i<=m) temp[k++]=a[i++];
    while (j<=r) temp[k++]=a[j++];
    for (int x=l;x<=r;x++) a[x]=temp[x];
}
void mergeSerial(vector<int>& a,vector<int>& temp,int l,int r) {
    if(l>=r) return;
    int m=l+(r-l)/2;
    mergeSerial(a,temp,l,m); mergeSerial(a,temp,m+1,r);
    mergeHalves(a,temp,l,m,r);
}
void mergeParallel(vector<int>& a,vector<int>& temp,int l,int r,int cutoff) {
    if(l>=r) return;
    if(r-l+1<=cutoff) { mergeSerial(a,temp,l,r); return; }
    int m=l+(r-l)/2;
    // Left and right tasks write disjoint array ranges.
    #pragma omp task shared(a,temp) firstprivate(l,m,cutoff)
    mergeParallel(a,temp,l,m,cutoff);
    #pragma omp task shared(a,temp) firstprivate(m,r,cutoff)
    mergeParallel(a,temp,m+1,r,cutoff);
    // Merge only after BOTH halves have been sorted.
    #pragma omp taskwait
    mergeHalves(a,temp,l,m,r);
}
int main(int argc,char** argv) {
    int n=argc>1?atoi(argv[1]):100000;
    int threads=argc>2?atoi(argv[2]):4;
    int cutoff=argc>3?atoi(argv[3]):2048;
    if(n<=0||threads<=0||cutoff<=0)return 1;
    mt19937 rng(42);
    vector<int> serial(n),parallel,temp1(n),temp2(n);
    for(int& x:serial)x=int(rng()%1000000);
    parallel=serial;
    double t=omp_get_wtime(); mergeSerial(serial,temp1,0,n-1);
    double ts=omp_get_wtime()-t;
    t=omp_get_wtime();
    #pragma omp parallel num_threads(threads)
    {
        #pragma omp single
        mergeParallel(parallel,temp2,0,n-1,cutoff);
    }
    double tp=omp_get_wtime()-t;
    bool ok=serial==parallel&&is_sorted(parallel.begin(),parallel.end());
    cout<<"Merge sort: serial="<<ts<<" parallel="<<tp<<" verification="<<(ok?"PASS":"FAIL")<<'\n';
    return ok?0:1;
}
