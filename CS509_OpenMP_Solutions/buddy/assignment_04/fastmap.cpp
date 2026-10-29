#include "../../common.hpp"
int main(int argc, char **argv)
{
  int threads = getThreads(argc, argv), n = argc > 2 ? max(3, atoi(argv[2])) : 100, k = 2;
  vector<vector<double>> d(n, vector<double>(n));
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      d[i][j] = abs(i - j);
  auto solve = [&](bool par)
  {
    auto residual = d;
    vector<vector<double>> coords(n, vector<double>(k));
    for (int dim = 0; dim < k; dim++)
    {
      // Pick approximate farthest pivots (two sweeps); selection is serial.
      int a = 0, b = 0;
      for (int pass = 0; pass < 2; pass++)
      {
        double best = -1;
        for (int j = 0; j < n; j++)
          if (residual[a][j] > best)
          {
            best = residual[a][j];
            b = j;
          }
        a = b;
      }
      double best = -1;
      for (int j = 0; j < n; j++)
        if (residual[a][j] > best)
        {
          best = residual[a][j];
          b = j;
        }
      double ab = residual[a][b];
      if (ab < 1e-12)
        break;
      // Each coordinate depends only on old residual distances.
      if (par)
      {
#pragma omp parallel for num_threads(threads)
        for (int i = 0; i < n; i++)
          coords[i][dim] = (residual[a][i] * residual[a][i] + ab * ab - residual[b][i] * residual[b][i]) / (2 * ab);
      }
      else
        for (int i = 0; i < n; i++)
          coords[i][dim] = (residual[a][i] * residual[a][i] + ab * ab - residual[b][i] * residual[b][i]) / (2 * ab);
      // Each distance-pair cell is updated independently after projection.
      if (par)
      {
#pragma omp parallel for collapse(2) num_threads(threads)
        for (int i = 0; i < n; i++)
          for (int j = 0; j < n; j++)
          {
            double diff = coords[i][dim] - coords[j][dim];
            residual[i][j] = sqrt(max(0.0, residual[i][j] * residual[i][j] - diff * diff));
          }
      }
      else
        for (int i = 0; i < n; i++)
          for (int j = 0; j < n; j++)
          {
            double diff = coords[i][dim] - coords[j][dim];
            residual[i][j] = sqrt(max(0.0, residual[i][j] * residual[i][j] - diff * diff));
          }
    }
    return coords;
  };
  double t = omp_get_wtime();
  auto a = solve(false);
  double st = omp_get_wtime() - t;
  t = omp_get_wtime();
  auto b = solve(true);
  double pt = omp_get_wtime() - t;
  bool ok = true;
  for (int i = 0; i < n; i++)
    for (int j = 0; j < k; j++)
      if (abs(a[i][j] - b[i][j]) > 1e-8)
        ok = false;
  cout << "First point coordinates: ";
  for (double x : b[0])
    cout << x << ' ';
  cout << '\n';
  status(ok, st, pt, threads);
  return !ok;
}
