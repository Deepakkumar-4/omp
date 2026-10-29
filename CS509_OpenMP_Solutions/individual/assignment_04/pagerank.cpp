#include "../../common.hpp"
int main(int argc, char **argv)
{
    int threads = getThreads(argc, argv);
    auto g = sampleGraph();
    int n = g.size();
    auto solve = [&](bool par)
    {
        vector<double> rank(n, 1.0 / n), next(n);
        const double damping = .85;
        for (int iter = 0; iter < 50; iter++)
        {
            // Each destination computes its own rank; no shared floating-point writes.
            if (par)
            {
#pragma omp parallel for num_threads(threads)
                for (int v = 0; v < n; v++)
                {
                    double sum = 0;
                    for (int u = 0; u < n; u++)
                        for (auto [to, w] : g[u])
                            if (to == v)
                                sum += rank[u] / g[u].size();
                    next[v] = (1 - damping) / n + damping * sum;
                }
            }
            else
                for (int v = 0; v < n; v++)
                {
                    double sum = 0;
                    for (int u = 0; u < n; u++)
                        for (auto [to, w] : g[u])
                            if (to == v)
                                sum += rank[u] / g[u].size();
                    next[v] = (1 - damping) / n + damping * sum;
                }
            // Stop between iterations; the whole next vector must be complete.
            rank.swap(next);
        }
        return rank;
    };
    double t = omp_get_wtime();
    auto a = solve(false);
    double st = omp_get_wtime() - t;
    t = omp_get_wtime();
    auto b = solve(true);
    double pt = omp_get_wtime() - t;
    bool ok = true;
    for (int i = 0; i < n; i++)
        if (abs(a[i] - b[i]) > 1e-10)
            ok = false;
    for (double v : b)
        cout << v << ' ';
    cout << '\n';
    status(ok, st, pt, threads);
    return !ok;
}
