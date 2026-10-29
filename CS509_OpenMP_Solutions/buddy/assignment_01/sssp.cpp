#include "../../common.hpp"
int main(int argc, char **argv)
{
    int threads = getThreads(argc, argv);
    auto g = sampleGraph();
    int n = g.size(), INF = 100000000;
    auto solve = [&](bool par)
    {
        vector<int> d(n, INF), used(n);
        d[0] = 0;
        for (int step = 0; step < n; step++)
        {
            int best = INF, u = -1;
            // Parallel scan chooses the next minimum unvisited vertex.
            if (par)
            {
#pragma omp parallel num_threads(threads)
                {
                    int local = INF, who = -1;
#pragma omp for nowait
                    for (int i = 0; i < n; i++)
                        if (!used[i] && d[i] < local)
                        {
                            local = d[i];
                            who = i;
                        }
#pragma omp critical(sssp_min)
                    if (who >= 0 && (local < best || (local == best && (u < 0 || who < u))))
                    {
                        best = local;
                        u = who;
                    }
                }
            }
            else
                for (int i = 0; i < n; i++)
                    if (!used[i] && d[i] < best)
                    {
                        best = d[i];
                        u = i;
                    }
            if (u < 0)
                break;
            used[u] = 1;
            // Distinct neighbors can be relaxed at once for this simple graph.
            if (par)
            {
#pragma omp parallel for num_threads(threads)
                for (int x = 0; x < (int)g[u].size(); x++)
                {
                    auto [v, w] = g[u][x];
                    if (!used[v])
                        d[v] = min(d[v], d[u] + w);
                }
            }
            else
                for (auto [v, w] : g[u])
                    if (!used[v])
                        d[v] = min(d[v], d[u] + w);
        }
        return d;
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
