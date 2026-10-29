/*
Prim's Minimum Spanning Tree Assignment
Computes the MST of a weighted undirected graph using Prim's algorithm.
The serial version scans all vertices to pick the minimum key each step.
The parallel version parallelizes the minimum‑selection and key‑updates across threads.
*/
#include "../../common.hpp"
int main(int argc, char **argv)
{
    int threads = getThreads(argc, argv);
    auto g = sampleGraph();
    int n = g.size();
    auto solve = [&](bool par)
    {
        vector<int> key(n, 100000000), used(n);
        key[0] = 0;
        ll weight = 0;
        for (int step = 0; step < n; step++)
        {
            // Parallel minimum selection: each worker searches its subset.
            int chosen = -1, best = 100000000;
            if (par)
            {
#pragma omp parallel num_threads(threads)
                {
                    int local = -1, val = 100000000;
#pragma omp for nowait
                    for (int i = 0; i < n; i++)
                        if (!used[i] && (key[i] < val || (key[i] == val && (local < 0 || i < local))))
                        {
                            val = key[i];
                            local = i;
                        }
#pragma omp critical
                    if (local >= 0 && (val < best || (val == best && (chosen < 0 || local < chosen))))
                    {
                        best = val;
                        chosen = local;
                    }
                }
            }
            else
                for (int i = 0; i < n; i++)
                    if (!used[i] && (key[i] < best || (key[i] == best && (chosen < 0 || i < chosen))))
                    {
                        best = key[i];
                        chosen = i;
                    }
            if (chosen < 0 || best == 100000000)
                return (ll)-1;
            used[chosen] = 1;
            weight += best;
            // Updating neighbor keys is independent because the graph has no duplicate edges.
            if (par)
            {
#pragma omp parallel for num_threads(threads)
                for (int e = 0; e < (int)g[chosen].size(); e++)
                {
                    auto [v, w] = g[chosen][e];
                    if (!used[v])
                        key[v] = min(key[v], w);
                }
            }
            else
                for (auto [v, w] : g[chosen])
                    if (!used[v])
                        key[v] = min(key[v], w);
        }
        return weight;
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
