#include "../../common.hpp"
int main(int argc, char **argv)
{
  int threads = getThreads(argc, argv);
  auto g = sampleGraph();
  int n = g.size();
  auto solve = [&](bool par)
  {
    vector<int> d(n, -1), frontier = {0};
    d[0] = 0;
    int level = 0;
    while (!frontier.empty())
    {
      vector<int> next;
      if (par)
      {
#pragma omp parallel num_threads(threads)
        {
          vector<int> local;
#pragma omp for nowait
          for (int x = 0; x < (int)frontier.size(); x++)
            for (auto [v, w] : g[frontier[x]])
            {
              // Only the thread that first marks v will add it to its frontier.
              bool first = false;
#pragma omp critical(bfs_visit)
              {
                if (d[v] == -1)
                {
                  d[v] = level + 1;
                  first = true;
                }
              }
              if (first)
                local.push_back(v);
            }
#pragma omp critical
          next.insert(next.end(), local.begin(), local.end());
        }
      }
      else
        for (int u : frontier)
          for (auto [v, w] : g[u])
            if (d[v] == -1)
            {
              d[v] = level + 1;
              next.push_back(v);
            }
      frontier.swap(next);
      level++;
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
