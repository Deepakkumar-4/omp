#include <omp.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;


struct Args
{
    size_t n;
    int threads;
    int repeats;
};


Args parse(int argc, char **argv, size_t default_n)
{
    Args x{default_n, 4, 3};
    if (argc > 1)
        x.n = stoull(argv[1]);
    if (argc > 2)
        x.threads = stoi(argv[2]);
    if (argc > 3)
        x.repeats = stoi(argv[3]);
    if (x.n == 0 || x.threads < 1 || x.repeats < 1)
        throw invalid_argument("n, threads, repeats must be positive");
    omp_set_dynamic(0);
    omp_set_num_threads(x.threads);
    return x;
}
template <class F>
double benchmark(F f, int repeats)
{
    double best = numeric_limits<double>::infinity();
    for (int r = 0; r < repeats; ++r)
    {
        double t = omp_get_wtime();
        f();
        best = min(best, omp_get_wtime() - t);
    }
    return best;
}
void output(const string &task, const string &variant, size_t n, int threads, double seconds, bool valid, double serial)
{
    cout << setprecision(10) << task << ", " << setw(15) << variant << ", " << n << ", " << threads << ", " << seconds << ", " << (valid ? "PASS" : "FAIL") << ", " << (seconds > 0 ? serial / seconds : 0) << ", " << (seconds > 0 ? serial / seconds / threads : 0) << "\n";
}
