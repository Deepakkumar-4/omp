/*
Bellman-Ford Assignment
Computes shortest paths from source vertex 0 to all others.
Parallel version parallelizes the per-vertex relaxation step across threads.
*/
#include "../../common.hpp"
int main(int argc, char **argv)
{
  int threads = getThreads(argc, argv);
  Graph g = sampleGraph();
  int n = g.size();
  const int INF = 100000000;
  // Read-only distances for this iteration prevent write/write races.
  auto solve = [&](bool parallel)
  {
    vector<int> d(n, INF);
    d[0] = 0;
    for (int it = 0; it < n - 1; it++)
    {
      auto old = d;
      vector<int> next(n, INF);
      if (parallel)
      {
#pragma omp parallel for num_threads(threads)
        for (int v = 0; v < n; v++)
        {
          int best = old[v];
          for (int u = 0; u < n; u++)
            for (auto [to, w] : g[u])
              if (to == v && old[u] != INF)
                best = min(best, old[u] + w);
          next[v] = best;
        }
      }
      else
        for (int v = 0; v < n; v++)
        {
          int best = old[v];
          for (int u = 0; u < n; u++)
            for (auto [to, w] : g[u])
              if (to == v && old[u] != INF)
                best = min(best, old[u] + w);
          next[v] = best;
        }
      d.swap(next);
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
