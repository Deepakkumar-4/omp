#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

// Some blocks have more work than others. Every schedule uses the same input.
double compute(double x, int i) {
    int work = (i % 1000) * 10;
    for (int j = 0; j < work; ++j) x = x * 1.0000001 + 0.0000001;
    return x;
}
int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 100000;
    int threads = argc > 2 ? atoi(argv[2]) : 4;
    int chunk = argc > 3 ? atoi(argv[3]) : 100;
    if (n <= 0 || threads <= 0 || chunk <= 0) return 1;
    vector<double> input(n), serial(n), output(n);
    for (int i = 0; i < n; ++i) input[i] = i * 0.001;
    double t = omp_get_wtime();
    for (int i = 0; i < n; ++i) serial[i] = compute(input[i], i);
    double ts = omp_get_wtime() - t;
    omp_set_dynamic(0);
    omp_set_num_threads(threads);
    cout << fixed << setprecision(6);
    for (int mode = 0; mode < 3; ++mode) {
        t = omp_get_wtime();
        if (mode == 0) {
            #pragma omp parallel for schedule(static)
            for (int i = 0; i < n; ++i) output[i] = compute(input[i], i);
        } else if (mode == 1) {
            #pragma omp parallel for schedule(dynamic, chunk)
            for (int i = 0; i < n; ++i) output[i] = compute(input[i], i);
        } else {
            #pragma omp parallel for schedule(guided)
            for (int i = 0; i < n; ++i) output[i] = compute(input[i], i);
        }
        double tp = omp_get_wtime() - t;
        bool pass = true;
        for (int i = 0; i < n; ++i) if (output[i] != serial[i]) { pass = false; break; }
        const char* name = mode == 0 ? "static" : mode == 1 ? "dynamic" : "guided";
        cout << "Task 2 | " << name << " N=" << n << " threads=" << threads
             << " serial=" << ts << "s parallel=" << tp << "s";
        if (tp > 0) cout << " speedup=" << ts / tp << " efficiency=" << 100 * ts / (tp * threads) << "%";
        cout << " verification=" << (pass ? "PASS" : "FAIL") << '\n';
        if (!pass) return 1;
    }
    return 0;
}
