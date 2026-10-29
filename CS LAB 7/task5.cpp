#include "common.hpp"
// i-j-k: one local accumulator per C(i,j); no competing writes.
void serial(const vector<double> &a, const vector<double> &b, vector<double> &c, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
        {
            double sum = 0;
            for (size_t k = 0; k < n; ++k)
                sum += a[i * n + k] * b[k * n + j];
            c[i * n + j] = sum;
        }
}
void outer(const vector<double> &a, const vector<double> &b, vector<double> &c, size_t n)
{
#pragma omp parallel for schedule(static)
    for (long long i = 0; i < (long long)n; ++i)
        for (size_t j = 0; j < n; ++j)
        {
            double sum = 0;
            for (size_t k = 0; k < n; ++k)
                sum += a[i * n + k] * b[k * n + j];
            c[i * n + j] = sum;
        }
}
void collapsed(const vector<double> &a, const vector<double> &b, vector<double> &c, size_t n)
{
#pragma omp parallel for collapse(2) schedule(static)
    for (long long i = 0; i < (long long)n; ++i)
        for (long long j = 0; j < (long long)n; ++j)
        {
            double sum = 0;
            for (size_t k = 0; k < n; ++k)
                sum += a[i * n + k] * b[k * n + j];
            c[i * n + j] = sum;
        }
}
// Bonus: i-k-j improves contiguous access on B and C, but changes sum order.
void ikj(const vector<double> &a, const vector<double> &b, vector<double> &c, size_t n)
{
    fill(c.begin(), c.end(), 0.0);
    for (size_t i = 0; i < n; ++i)
        for (size_t k = 0; k < n; ++k)
        {
            double v = a[i * n + k];
            for (size_t j = 0; j < n; ++j)
                c[i * n + j] += v * b[k * n + j];
        }
}
void ikj_parallel(const vector<double> &a, const vector<double> &b, vector<double> &c, size_t n)
{
    fill(c.begin(), c.end(), 0.0);
#pragma omp parallel for schedule(static)
    for (long long i = 0; i < (long long)n; ++i)
        for (size_t k = 0; k < n; ++k)
        {
            double v = a[i * n + k];
            for (size_t j = 0; j < n; ++j)
                c[i * n + j] += v * b[k * n + j];
        }
}
bool close(const vector<double> &a, const vector<double> &b)
{
    for (size_t i = 0; i < a.size(); ++i)
        if (abs(a[i] - b[i]) > 1e-9 + 1e-9 * abs(a[i]))
            return false;
    return true;
}
int main(int argc, char **argv)
{
    try
    {
        auto x = parse(argc, argv, 256);
        if (x.n > SIZE_MAX / x.n)
            throw invalid_argument("matrix size overflow");
        size_t len = x.n * x.n;
        vector<double> a(len), b(len), cs(len), co(len), cc(len), ci(len), cip(len);
        for (size_t i = 0; i < len; ++i)
        {
            a[i] = double(i % 13) / 13;
            b[i] = double(i % 17) / 17;
        }
        double ts = benchmark([&]
                              { serial(a, b, cs, x.n); }, x.repeats);
        double to = benchmark([&]
                              { outer(a, b, co, x.n); }, x.repeats);
        double tc = benchmark([&]
                              { collapsed(a, b, cc, x.n); }, x.repeats);
        double ti = benchmark([&]
                              { ikj(a, b, ci, x.n); }, x.repeats);
        double tip = benchmark([&]
                               { ikj_parallel(a, b, cip, x.n); }, x.repeats);
        bool o = close(cs, co), c = close(cs, cc), i = close(cs, ci), ip = close(cs, cip);
        
        output("task5", "serial", x.n, x.threads, ts, true, ts);
        output("task5", "outer", x.n, x.threads, to, o, ts);
        output("task5", "collapse2", x.n, x.threads, tc, c, ts);
        output("task5", "ikj_serial", x.n, x.threads, ti, i, ts);
        output("task5", "ikj_parallel", x.n, x.threads, tip, ip, ts);
        return o && c && i && ip ? 0 : 2;
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
}
