/*
Kruskal's Minimum Spanning Tree Assignment
Implements Kruskal's algorithm using a Union‑Find (Disjoint Set Union) data structure.
Edges are sorted; the parallel version uses a task‑based parallel mergesort to sort edges concurrently.
The DSU operations remain serial to avoid data races.
*/
#include "../../common.hpp"
struct Edge
{
    int u, v, w;
};
struct DSU
{
    vector<int> p;
    DSU(int n) : p(n) { iota(p.begin(), p.end(), 0); }
    int find(int x) { return p[x] == x ? x : p[x] = find(p[x]); }
    bool join(int a, int b)
    {
        a = find(a);
        b = find(b);
        if (a == b)
            return false;
        p[b] = a;
        return true;
    }
};
int main(int argc, char **argv)
{
    int threads = getThreads(argc, argv);
    auto g = sampleGraph();
    int n = g.size();
    vector<Edge> ed;
    for (int u = 0; u < n; u++)
        for (auto [v, w] : g[u])
            if (u < v)
                ed.push_back({u, v, w});
    auto solve = [&](bool parallel)
    {
        auto e = ed;
        // Sorting edges is parallelized with OpenMP tasks in a divide-and-conquer merge sort.
        vector<Edge> temp(e.size());
        auto cmp = [](Edge a, Edge b)
        { return tie(a.w, a.u, a.v) < tie(b.w, b.u, b.v); };
        auto mergesort = [&](auto &&self, int lo, int hi, int cutoff) -> void
        {
            if (hi - lo <= 1)
                return;
            if (hi - lo <= cutoff)
            {
                sort(e.begin() + lo, e.begin() + hi, cmp);
                return;
            }
            int mid = (lo + hi) / 2;
#pragma omp task shared(e, temp) firstprivate(lo, mid, cutoff)
            self(self, lo, mid, cutoff);
#pragma omp task shared(e, temp) firstprivate(mid, hi, cutoff)
            self(self, mid, hi, cutoff);
#pragma omp taskwait
            merge(e.begin() + lo, e.begin() + mid, e.begin() + mid, e.begin() + hi, temp.begin() + lo, cmp);
            copy(temp.begin() + lo, temp.begin() + hi, e.begin() + lo);
        };
        if (parallel)
        {
#pragma omp parallel num_threads(threads)
            {
#pragma omp single
                mergesort(mergesort, 0, e.size(), 2);
            }
        }
        else
            sort(e.begin(), e.end(), cmp);
        // Union-find remains serial: concurrent joins could corrupt the shared DSU.
        DSU d(n);
        ll total = 0;
        int edges = 0;
        for (auto x : e)
            if (d.join(x.u, x.v))
            {
                total += x.w;
                edges++;
            }
        return edges == n - 1 ? total : -1LL;
    };
    double t = omp_get_wtime();
    auto a = solve(false);
    double st = omp_get_wtime() - t;
    t = omp_get_wtime();
    auto b = solve(true);
    double pt = omp_get_wtime() - t;
    cout << "MST weight: " << b << '\n';
    status(a == b, st, pt, threads);
    return a != b;
}
