#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;
struct Counts { long long even = 0, odd = 0; };
Counts serialCount(const vector<int>& a) {
    Counts c;
    for (int x : a) if (x % 2 == 0) ++c.even; else ++c.odd;
    return c;
}
Counts reductionCount(const vector<int>& a) {
    long long even = 0, odd = 0;
    #pragma omp parallel for reduction(+:even, odd)
    for (long long i = 0; i < (long long)a.size(); ++i)
        if (a[i] % 2 == 0) ++even; else ++odd;
    return {even, odd};
}
Counts atomicCount(const vector<int>& a) {
    long long even = 0, odd = 0;
    #pragma omp parallel for
    for (long long i = 0; i < (long long)a.size(); ++i) {
        if (a[i] % 2 == 0) {
            #pragma omp atomic update
            ++even;
        } else {
            #pragma omp atomic update
            ++odd;
        }
    }
    return {even, odd};
}
int main(int argc, char** argv) {
    long long n = argc > 1 ? atoll(argv[1]) : 100000;
    int threads = argc > 2 ? atoi(argv[2]) : 4;
    if (n <= 0 || threads <= 0) return 1;
    vector<int> a(n);
    for (long long i = 0; i < n; ++i) a[i] = (int)(i % 1000);
    omp_set_dynamic(0);
    omp_set_num_threads(threads);
    double t = omp_get_wtime();
    Counts s = serialCount(a);
    double ts = omp_get_wtime() - t;
    cout << fixed << setprecision(6);
    for (int mode = 0; mode < 2; ++mode) {
        t = omp_get_wtime();
        Counts p = mode == 0 ? reductionCount(a) : atomicCount(a);
        double tp = omp_get_wtime() - t;
        bool pass = (p.even == s.even && p.odd == s.odd && p.even + p.odd == n);
        cout << "Task 3 | " << (mode == 0 ? "reduction" : "atomic") << " N=" << n
             << " threads=" << threads << " even=" << p.even << " odd=" << p.odd
             << " serial=" << ts << "s parallel=" << tp << "s";
        if (tp > 0) cout << " speedup=" << ts / tp << " efficiency=" << 100 * ts / (tp * threads) << "%";
        cout << " verification=" << (pass ? "PASS" : "FAIL") << '\n';
        if (!pass) return 1;
    }
    return 0;
}
