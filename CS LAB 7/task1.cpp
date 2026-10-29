#include "common.hpp"
// Each iteration writes a different output cell: no shared-write race.
void serial(const vector<double> &a, const vector<double> &b, vector<double> &c)
{
    for (size_t i = 0; i < a.size(); ++i)
        c[i] = a[i] + b[i];
}
void parallel(const vector<double> &a, const vector<double> &b, vector<double> &c)
{
#pragma omp parallel for schedule(static)
    for (long long i = 0; i < (long long)a.size(); ++i)
        c[i] = a[i] + b[i];
}
int main(int argc, char **argv)
{
    try
    {
        auto x = parse(argc, argv, 10000000);
        vector<double> a(x.n), b(x.n), cs(x.n), cp(x.n);
        for (size_t i = 0; i < x.n; ++i)
        {
            a[i] = double(i % 1024);
            b[i] = double(i % 512);
        } // exact integer-valued doubles
        double ts = benchmark([&]
                              { serial(a, b, cs); }, x.repeats);
        double tp = benchmark([&]
                              { parallel(a, b, cp); }, x.repeats);
        bool ok = equal(cs.begin(), cs.end(), cp.begin());
        output("task1", "serial", x.n, x.threads, ts, true, ts);
        output("task1", "parallel", x.n, x.threads, tp, ok, ts);
        return ok ? 0 : 2;
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
}
