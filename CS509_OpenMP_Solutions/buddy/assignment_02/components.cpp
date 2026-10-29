#include "../../common.hpp"
int main(int argc, char **argv)
{
    int threads = getThreads(argc, argv);
    auto g = sampleGraph();
    int n = g.size();
    auto solve = [&](bool par)
    {
        vector<int> label(n);
        iota(label.begin(), label.end(), 0);
        // Repeatedly propagate the smallest label across edges.
        for (int it = 0; it < n; it++)
        {
            vector<int> next = label;
            if (par)
            {
#pragma omp parallel for num_threads(threads)
                for (int u = 0; u < n; u++)
                    for (auto [v, w] : g[u])
                        next[u] = min(next[u], label[v]);
            }
            else
                for (int u = 0; u < n; u++)
                    for (auto [v, w] : g[u])
                        next[u] = min(next[u], label[v]);
            if (next == label)
                break;
            label.swap(next);
        }
        return label;
    };
    double t = omp_get_wtime();
    auto a = solve(false);
    double st = omp_get_wtime() - t;
    t = omp_get_wtime();
    auto b = solve(true);
    double pt = omp_get_wtime() - t;
    for (int x : b)
        cout << x << ' ';
    cout << '\n';
    status(a == b, st, pt, threads);
    return a != b;
}
