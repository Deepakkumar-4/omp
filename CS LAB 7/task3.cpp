#include "common.hpp"
struct Result
{
    int lo, hi;
    size_t li, hi_idx;
};
bool same(Result a, Result b) { return a.lo == b.lo && a.hi == b.hi && a.li == b.li && a.hi_idx == b.hi_idx; }
// Tie policy: return the FIRST index of the minimum/maximum.
Result serial(const vector<int> &a)
{
    Result r{a[0], a[0], 0, 0};
    for (size_t i = 1; i < a.size(); ++i)
    {
        if (a[i] < r.lo)
        {
            r.lo = a[i];
            r.li = i;
        }
        if (a[i] > r.hi)
        {
            r.hi = a[i];
            r.hi_idx = i;
        }
    }
    return r;
}
Result manual(const vector<int> &a)
{
    int nt = omp_get_max_threads();
    vector<Result> partial(nt);
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        Result r{a[0], a[0], 0, 0};
#pragma omp for schedule(static)
        for (long long i = 0; i < (long long)a.size(); ++i)
        {
            int v = a[i];
            size_t k = (size_t)i;
            if (v < r.lo || (v == r.lo && k < r.li))
            {
                r.lo = v;
                r.li = k;
            }
            if (v > r.hi || (v == r.hi && k < r.hi_idx))
            {
                r.hi = v;
                r.hi_idx = k;
            }
        }
        partial[tid] = r;
    }
    Result best = partial[0];
    for (int t = 1; t < nt; ++t)
    {
        auto r = partial[t];
        if (r.lo < best.lo || (r.lo == best.lo && r.li < best.li))
        {
            best.lo = r.lo;
            best.li = r.li;
        }
        if (r.hi > best.hi || (r.hi == best.hi && r.hi_idx < best.hi_idx))
        {
            best.hi = r.hi;
            best.hi_idx = r.hi_idx;
        }
    }
    return best;
}
// One critical section per thread, not per element.
Result critical_method(const vector<int> &a)
{
    Result best{a[0], a[0], 0, 0};
#pragma omp parallel
    {
        Result r{a[0], a[0], 0, 0};
#pragma omp for schedule(static)
        for (long long i = 0; i < (long long)a.size(); ++i)
        {
            int v = a[i];
            size_t k = (size_t)i;
            if (v < r.lo || (v == r.lo && k < r.li))
            {
                r.lo = v;
                r.li = k;
            }
            if (v > r.hi || (v == r.hi && k < r.hi_idx))
            {
                r.hi = v;
                r.hi_idx = k;
            }
        }
#pragma omp critical
        {
            if (r.lo < best.lo || (r.lo == best.lo && r.li < best.li))
            {
                best.lo = r.lo;
                best.li = r.li;
            }
            if (r.hi > best.hi || (r.hi == best.hi && r.hi_idx < best.hi_idx))
            {
                best.hi = r.hi;
                best.hi_idx = r.hi_idx;
            }
        }
    }
    return best;
}
// Built-in OpenMP max/min reduction finds VALUES, not their indices.
pair<int, int> value_reduction(const vector<int> &a)
{
    int lo = INT32_MAX, hi = INT32_MIN;
#pragma omp parallel for reduction(min : lo) reduction(max : hi)
    for (long long i = 0; i < (long long)a.size(); ++i)
    {
        lo = min(lo, a[i]);
        hi = max(hi, a[i]);
    }
    return {lo, hi};
}
int main(int argc, char **argv)
{
    try
    {
        auto x = parse(argc, argv, 10000000);
        vector<int> a(x.n);
        for (size_t i = 0; i < x.n; ++i)
            a[i] = int(i % 997) - 498;
        a[x.n / 3] = 99999;
        a[x.n / 2] = -99999;
        Result base = serial(a), m{}, c{};
        pair<int, int> v{};
        double ts = benchmark([&]
                              { base = serial(a); }, x.repeats);
        double tm = benchmark([&]
                              { m = manual(a); }, x.repeats);
        double tc = benchmark([&]
                              { c = critical_method(a); }, x.repeats);
        double tv = benchmark([&]
                              { v = value_reduction(a); }, x.repeats);
        bool okm = same(base, m), okc = same(base, c), okv = v.first == base.lo && v.second == base.hi;
        output("task3", "serial", x.n, x.threads, ts, true, ts);
        output("task3", "manual", x.n, x.threads, tm, okm, ts);
        output("task3", "critical", x.n, x.threads, tc, okc, ts);
        output("task3", "value_reduction", x.n, x.threads, tv, okv, ts);
        cerr << "min=" << base.lo << " min_index=" << base.li << " max=" << base.hi << " max_index=" << base.hi_idx << '\n';
        return okm && okc && okv ? 0 : 2;
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        return 1;
    }
}