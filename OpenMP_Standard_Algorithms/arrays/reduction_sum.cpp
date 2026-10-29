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
    for (int i = 0; i < n; i++) a[i] = i % 10;
    long long serial = 0, parallel = 0;
    double t = omp_get_wtime();
    for (int value : a) serial += value;
    double ts = omp_get_wtime()-t;
    t = omp_get_wtime();
    // Reduction gives each thread a private partial sum, then combines sums.
    #pragma omp parallel for num_threads(threads) reduction(+:parallel)
    for (int i = 0; i < n; i++) parallel += a[i];
    double tp = omp_get_wtime()-t;
    bool ok = serial == parallel;
    cout << "Array sum=" << parallel << " serial=" << ts << " parallel=" << tp << " verification=" << (ok ? "PASS" : "FAIL") << '\n';
    return ok ? 0 : 1;
}
