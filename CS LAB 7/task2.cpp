#include "common.hpp"
long long serial(const vector<int> &a)
{
    long long sum = 0;
    for (int v : a)
        sum += v;
    return sum;
}
// Deliberately incorrect: shared sum is modified without synchronization.
// A data race means behavior is undefined; do not use in production.
long long broken(const vector<int> &a)
{
    long long sum = 0;
#pragma omp parallel for schedule(static)
    for (long long i = 0; i < (long long)a.size(); ++i)
        sum += a[i];
    return sum;
}
long long reduction_sum(const vector<int> &a)
{
    long long sum = 0;
#pragma omp parallel for reduction(+ : sum) schedule(static)
    for (long long i = 0; i < (long long)a.size(); ++i)
        sum += a[i];
    return sum;
}
int main(int argc, char **argv)
{
    try
    {
        auto x = parse(argc, argv, 10000000);
        vector<int> a(x.n);
        for (size_t i = 0; i < x.n; ++i)
            a[i] = int(i % 11) + 1;
        long long expected = serial(a);
        double ts = benchmark([&]
                              {volatile long long v=serial(a);(void)v; }, x.repeats);
        long long good = 0;
        double tp = benchmark([&]
                              { good = reduction_sum(a); }, x.repeats);
        output("task2", "serial", x.n, x.threads, ts, true, ts);
        output("task2", "reduction", x.n, x.threads, tp, good == expected, ts);
        cout << "Race demonstration is intentionally separate from timed correct implementation." << endl;
        for (int r = 0; r < 5; ++r)
        {
            long long bad = broken(a);
            cerr << "BROKEN_RUN " << r + 1 << " expected=" << expected << " observed=" << bad << "\n";
        }
        return good == expected ? 0 : 2;
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
}