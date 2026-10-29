#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <omp.h>
using namespace std;

void mergeParts(vector<int>& a, vector<int>& tmp, int l, int m, int r) {
    int i = l, j = m + 1, k = l;
    while (i <= m && j <= r) tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i <= m) tmp[k++] = a[i++];
    while (j <= r) tmp[k++] = a[j++];
    for (int x = l; x <= r; ++x) a[x] = tmp[x];
}
void serialSort(vector<int>& a, vector<int>& tmp, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    serialSort(a, tmp, l, m);
    serialSort(a, tmp, m + 1, r);
    mergeParts(a, tmp, l, m, r);
}
void parallelSort(vector<int>& a, vector<int>& tmp, int l, int r, int cutoff) {
    if (l >= r) return;
    if (r - l + 1 <= cutoff) {
        serialSort(a, tmp, l, r);
        return;
    }
    int m = l + (r - l) / 2;
    // Each task edits a different part of a and tmp.
    #pragma omp task shared(a, tmp) firstprivate(l, m, cutoff)
    parallelSort(a, tmp, l, m, cutoff);
    #pragma omp task shared(a, tmp) firstprivate(m, r, cutoff)
    parallelSort(a, tmp, m + 1, r, cutoff);
    #pragma omp taskwait
    mergeParts(a, tmp, l, m, r);
}
int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 100000;
    int threads = argc > 2 ? atoi(argv[2]) : 4;
    int cutoff = argc > 3 ? atoi(argv[3]) : 2048;
    if (n <= 0 || threads <= 0 || cutoff <= 0) return 1;
    vector<int> input(n), serial(n), parallel(n), temp1(n), temp2(n);
    mt19937 rng(42);
    uniform_int_distribution<int> dist(0, 1000000);
    for (int& v : input) v = dist(rng);
    serial = parallel = input;
    double t = omp_get_wtime();
    serialSort(serial, temp1, 0, n - 1);
    double ts = omp_get_wtime() - t;
    omp_set_dynamic(0);
    omp_set_num_threads(threads);
    t = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        parallelSort(parallel, temp2, 0, n - 1, cutoff);
    }
    double tp = omp_get_wtime() - t;
    bool pass = is_sorted(parallel.begin(), parallel.end()) && serial == parallel;
    cout << fixed << setprecision(6);
    cout << "Task 1 | N=" << n << " threads=" << threads << " cutoff=" << cutoff
         << " serial=" << ts << "s parallel=" << tp << "s";
    if (tp > 0) cout << " speedup=" << ts / tp << " efficiency=" << 100 * ts / (tp * threads) << "%";
    cout << " verification=" << (pass ? "PASS" : "FAIL") << '\n';
    return pass ? 0 : 1;
}
