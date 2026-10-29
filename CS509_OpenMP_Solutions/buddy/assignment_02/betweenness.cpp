#include "../../common.hpp"
int main(int argc, char **argv)
{
  int threads = getThreads(argc, argv);
  auto g = sampleGraph();
  int n = g.size();
  auto solve = [&](bool par)
  {
    vector<vector<double>> contrib(n, vector<double>(n, 0));
    // Brandes: different sources are independent, so parallelize over sources.
    auto source = [&](int s)
    {
      vector<vector<int>> pred(n);
      vector<int> dist(n, -1), order;
      vector<double> sigma(n), delta(n);
      queue<int> q;
      dist[s] = 0;
      sigma[s] = 1;
      q.push(s);
      while (!q.empty())
      {
        int u = q.front();
        q.pop();
        order.push_back(u);
        for (auto [v, w] : g[u])
        {
          if (dist[v] < 0)
          {
            dist[v] = dist[u] + 1;
            q.push(v);
          }
          if (dist[v] == dist[u] + 1)
          {
            sigma[v] += sigma[u];
            pred[v].push_back(u);
          }
        }
      }
      reverse(order.begin(), order.end());
      for (int w : order)
      {
        for (int v : pred[w])
          if (sigma[w] > 0)
            delta[v] += (sigma[v] / sigma[w]) * (1 + delta[w]);
        if (w != s)
          contrib[s][w] = delta[w] / 2;
      }
    };
    if (par)
    {
#pragma omp parallel for num_threads(threads)
      for (int s = 0; s < n; s++)
        source(s);
    }
    else
      for (int s = 0; s < n; s++)
        source(s);
    vector<double> sum(n);
    for (int s = 0; s < n; s++)
      for (int v = 0; v < n; v++)
        sum[v] += contrib[s][v];
    return sum;
  };
  double t = omp_get_wtime();
  auto a = solve(false);
  double st = omp_get_wtime() - t;
  t = omp_get_wtime();
  auto b = solve(true);
  double pt = omp_get_wtime() - t;
  bool ok = true;
  for (int i = 0; i < n; i++)
    if (abs(a[i] - b[i]) > 1e-9)
      ok = false;
  for (double x : b)
    cout << x << ' ';
  cout << '\n';
  status(ok, st, pt, threads);
  return !ok;
}
