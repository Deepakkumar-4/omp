/*
Floyd–Warshall Assignment
Computes all-pairs shortest paths for a weighted graph using the classic DP algorithm.
The algorithm iteratively improves path lengths via an intermediate vertex `k`.
Parallel version parallelizes the inner i,j loops for each `k` iteration, ensuring each thread updates distinct matrix cells.
*/
#include "../../common.hpp"
int main(int argc, char **argv)
{
  int threads = getThreads(argc, argv), n = 6, INF = 100000000;
  auto g = sampleGraph();
  vector<vector<int>> initial(n, vector<int>(n, INF));
  for (int i = 0; i < n; i++)
  {
    initial[i][i] = 0;
    for (auto [v, w] : g[i])
      initial[i][v] = min(initial[i][v], w);
  }
  auto solve = [&](bool parallel)
  {
    auto d = initial;
    for (int k = 0; k < n; k++)
    {
      // Freeze pivot row and column; the next k-phase must wait for all updates.
      vector<int> row = d[k], col(n);
      for (int i = 0; i < n; i++)
        col[i] = d[i][k];
      if (parallel)
      {
#pragma omp parallel for collapse(2) num_threads(threads)
        for (int i = 0; i < n; i++)
          for (int j = 0; j < n; j++)
            if (col[i] < INF && row[j] < INF)
              d[i][j] = min(d[i][j], col[i] + row[j]);
      }
      else
        for (int i = 0; i < n; i++)
          for (int j = 0; j < n; j++)
            if (col[i] < INF && row[j] < INF)
              d[i][j] = min(d[i][j], col[i] + row[j]);
    }
    return d;
  };
  double t = omp_get_wtime();
  auto a = solve(false);
  double st = omp_get_wtime() - t;
  t = omp_get_wtime();
  auto b = solve(true);
  double pt = omp_get_wtime() - t;
  for (auto &r : b)
  {
    for (int v : r)
      cout << v << ' ';
    cout << '\n';
  }
  status(a == b, st, pt, threads);
  return a != b;
}
