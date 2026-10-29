#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <omp.h>
using namespace std;
void oddEvenSerial(vector<int>& a) {
    int n = int(a.size());
    for (int phase = 0; phase < n; phase++) {
        // Even phase: (0,1), (2,3) ... Odd phase: (1,2), (3,4) ...
        for (int i = phase % 2; i + 1 < n; i += 2)
            if (a[i] > a[i+1]) swap(a[i], a[i+1]);
    }
}
void oddEvenParallel(vector<int>& a, int threads) {
    int n = int(a.size());
    for (int phase = 0; phase < n; phase++) {
        int first = phase % 2;
        // Pairs do not overlap, so swaps are safe. End-of-loop barrier separates phases.
        #pragma omp parallel for num_threads(threads)
        for (int i = first; i < n-1; i += 2)
            if (a[i] > a[i+1]) swap(a[i], a[i+1]);
    }
}
int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 1000;
    int threads = argc > 2 ? atoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) return 1;
    vector<int> serial(n), parallel(n);
    for (int i = 0; i < n; i++) serial[i] = n - i;
    parallel = serial;
    double t = omp_get_wtime(); oddEvenSerial(serial);
    double ts = omp_get_wtime()-t;
    t = omp_get_wtime(); oddEvenParallel(parallel,threads);
    double tp = omp_get_wtime()-t;
    bool ok = serial == parallel && is_sorted(parallel.begin(),parallel.end());
    cout << "Odd-even sort: serial=" << ts << " parallel=" << tp << " verification=" << (ok ? "PASS" : "FAIL") << '\n';
    return ok ? 0 : 1;
}
