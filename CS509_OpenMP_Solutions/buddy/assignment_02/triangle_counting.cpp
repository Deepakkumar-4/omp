#include "../../common.hpp"
int main(int argc, char **argv)
{
  int threads = getThreads(argc, argv);
  auto g = sampleGraph();
  int n = g.size();
  vector<vector<bool>> has(n, vector<bool>(n));
  for (int u = 0; u < n; u++)
    for (auto [v, w] : g[u])
      has[u][v] = true;
  auto solve = [&](bool par)
  {
    ll count = 0;
    if (par)
    {
#pragma omp parallel for reduction(+ : count) num_threads(threads)
      for (int u = 0; u < n; u++)
        for (int v = u + 1; v < n; v++)
          if (has[u][v])
            for (int w = v + 1; w < n; w++)
              if (has[u][w] && has[v][w])
                count++;
    }
    else
      for (int u = 0; u < n; u++)
        for (int v = u + 1; v < n; v++)
          if (has[u][v])
            for (int w = v + 1; w < n; w++)
              if (has[u][w] && has[v][w])
                count++;
    return count;
  };
  double t = omp_get_wtime();
  auto a = solve(false);
  double st = omp_get_wtime() - t;
  t = omp_get_wtime();
  auto b = solve(true);
  double pt = omp_get_wtime() - t;
  cout << "Triangles: " << b << '\n';
  status(a == b, st, pt, threads);
  return a != b;
}
