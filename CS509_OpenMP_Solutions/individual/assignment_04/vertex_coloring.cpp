#include "../../common.hpp"
// Entry point for the Vertex Coloring assignment using Welsh-Powell ordering
int main(int argc, char **argv)
{
    int threads = getThreads(argc, argv);
    auto g = sampleGraph();
    int n = g.size();
    auto solve = [&](bool par)
    {
        vector<int> order(n), color(n, -1);
        iota(order.begin(), order.end(), 0);
        // Welsh-Powell degree ordering: compute degree of each vertex.
        // Parallel version distributes the degree computation across threads.
        // Each thread writes to a distinct entry in `degree`, so no data races.
        vector<int> degree(n);
        if (par)
        {
#pragma omp parallel for num_threads(threads)
            for (int i = 0; i < n; i++)
                degree[i] = g[i].size();
        }
        else
        {
            // Serial fallback computes degrees sequentially.
            for (int i = 0; i < n; i++)
                degree[i] = g[i].size();
        }
        sort(order.begin(), order.end(), [&](int a, int b)
             { return degree[a] != degree[b] ? degree[a] > degree[b] : a < b; });
        // Greedy coloring is intentionally serial: each choice needs earlier colors.
        for (int u : order)
        {
            vector<bool> used(n + 1);
            for (auto [v, w] : g[u])
                if (color[v] >= 0)
                    used[color[v]] = true;
            int c = 0;
            while (used[c])
                c++;
            color[u] = c;
        }
        return color;
    };
    double t = omp_get_wtime();
    auto a = solve(false);
    double st = omp_get_wtime() - t;
    t = omp_get_wtime();
    auto b = solve(true);
    double pt = omp_get_wtime() - t;
    bool valid = a == b;
    for (int u = 0; u < n; u++)
        for (auto [v, w] : g[u])
            if (b[u] == b[v])
                valid = false;
    for (int v : b)
        cout << v << ' ';
    cout << '\n';
    status(valid, st, pt, threads);
    return !valid;
}
