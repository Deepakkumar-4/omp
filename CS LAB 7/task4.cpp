#include "common.hpp"
// Row-major flattened N x N matrix: element (i,j) is at i*n+j.
void serial(const vector<double> &a, const vector<double> &b, vector<double> &c, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            c[i * n + j] = a[i * n + j] + b[i * n + j];
}
void outer(const vector<double> &a, const vector<double> &b, vector<double> &c, size_t n)
{
#pragma omp parallel for schedule(static)
    for (long long i = 0; i < (long long)n; ++i)
        for (size_t j = 0; j < n; ++j)
            c[i * n + j] = a[i * n + j] + b[i * n + j];
}
void collapsed(const vector<double> &a, const vector<double> &b, vector<double> &c, size_t n)
{
#pragma omp parallel for collapse(2) schedule(static)
    for (long long i = 0; i < (long long)n; ++i)
        for (long long j = 0; j < (long long)n; ++j)
            c[i * n + j] = a[i * n + j] + b[i * n + j];
}
int main(int argc, char **argv)
{
    try
    {
        auto x = parse(argc, argv, 4096);
        if (x.n > SIZE_MAX / x.n)
            throw invalid_argument("matrix size overflow");
        size_t len = x.n * x.n;
        vector<double> a(len), b(len), cs(len), co(len), cc(len);
        for (size_t i = 0; i < len; ++i)
        {
            a[i] = double(i % 101);
            b[i] = double(i % 67);
        }
        double ts = benchmark([&]
                              { serial(a, b, cs, x.n); }, x.repeats);
        double to = benchmark([&]
                              { outer(a, b, co, x.n); }, x.repeats);
        double tc = benchmark([&]
                              { collapsed(a, b, cc, x.n); }, x.repeats);
        bool o = equal(cs.begin(), cs.end(), co.begin()), c = equal(cs.begin(), cs.end(), cc.begin());
        output("task4", "serial", x.n, x.threads, ts, true, ts);
        output("task4", "outer", x.n, x.threads, to, o, ts);
        output("task4", "collapse2", x.n, x.threads, tc, c, ts);
        return o && c ? 0 : 2;
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
}
