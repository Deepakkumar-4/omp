#include "../../common.hpp"
int main(int argc, char **argv)
{
  int threads = getThreads(argc, argv);
  auto g = sampleGraph();
  int n = g.size();
  auto solve = [&](bool par)
  {
    vector<int> seen(n, 0);
    auto dfs = [&](auto &&self, int u, int depth) -> void
    {
      bool first = false;
// A shared visited array needs synchronization.
#pragma omp critical(dfs_seen)
      {
        if (!seen[u])
        {
          seen[u] = 1;
          first = true;
        }
      }
      if (!first)
        return;
      for (auto [v, w] : g[u])
      {
        if (par && depth < 3)
        {
#pragma omp task firstprivate(v, depth)
          self(self, v, depth + 1);
        }
        else
          self(self, v, depth + 1);
      }
#pragma omp taskwait
    };
    if (par)
    {
#pragma omp parallel num_threads(threads)
      {
#pragma omp single
        dfs(dfs, 0, 0);
      }
    }
    else
      dfs(dfs, 0, 0);
    return seen;
  };
  double t = omp_get_wtime();
  auto a = solve(false);
  double st = omp_get_wtime() - t;
  t = omp_get_wtime();
  auto b = solve(true);
  double pt = omp_get_wtime() - t;
  status(a == b, st, pt, threads);
  return a != b;
}
