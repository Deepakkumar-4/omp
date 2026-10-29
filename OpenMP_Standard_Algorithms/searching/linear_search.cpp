#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;
int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 1000000;
    int threads = argc > 2 ? atoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) return 1;
    vector<int> a(n);
    for (int i = 0; i < n; i++) a[i] = i % 1000;
    int target = 777;
    int serialIndex = -1;
    double t = omp_get_wtime();
    // Serial: stop at the first match.
    for (int i = 0; i < n; i++) if (a[i] == target) { serialIndex = i; break; }
    double ts = omp_get_wtime() - t;
    int parallelIndex = n;  // n means not found; all valid indices are smaller.
    t = omp_get_wtime();
    // Each thread searches different indices. Reduction keeps minimum match.
    #pragma omp parallel for num_threads(threads) reduction(min:parallelIndex)
    for (int i = 0; i < n; i++) if (a[i] == target && i < parallelIndex) parallelIndex = i;
    double tp = omp_get_wtime() - t;
    if (parallelIndex == n) parallelIndex = -1;
    bool ok = parallelIndex == serialIndex;
    cout << "Linear search: index=" << parallelIndex << " serial=" << ts << " parallel=" << tp << " verification=" << (ok ? "PASS" : "FAIL") << '\n';
    return ok ? 0 : 1;
}
