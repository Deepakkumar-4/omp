#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <omp.h>
using namespace std;
// One binary search depends on earlier comparisons, so keep it serial.
int binarySearch(const vector<int>& a, int target) {
    int l = 0, r = int(a.size()) - 1;
    while (l <= r) {
        int m = l + (r - l) / 2;
        if (a[m] == target) return m;
        if (a[m] < target) l = m + 1; else r = m - 1;
    }
    return -1;
}
int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 100000;
    int threads = argc > 2 ? atoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) return 1;
    vector<int> a(n), queries(n), serial(n), parallel(n);
    for (int i = 0; i < n; i++) { a[i] = 2*i; queries[i] = i; }
    double t = omp_get_wtime();
    for (int i = 0; i < n; i++) serial[i] = binarySearch(a, queries[i]);
    double ts = omp_get_wtime()-t;
    t = omp_get_wtime();
    // Queries are independent: assign one binary search to each iteration.
    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < n; i++) parallel[i] = binarySearch(a, queries[i]);
    double tp = omp_get_wtime()-t;
    bool ok = serial == parallel;
    cout << "Binary queries: serial=" << ts << " parallel=" << tp << " verification=" << (ok ? "PASS" : "FAIL") << '\n';
    return ok ? 0 : 1;
}
