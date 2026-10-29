#include "../../common.hpp"
int main(int argc, char **argv)
{
   int threads = getThreads(argc, argv), n = argc > 2 ? max(3, atoi(argv[2])) : 10000, k = 3;
   vector<vector<double>> p(n, vector<double>(2));
   for (int i = 0; i < n; i++)
   {
      p[i][0] = (i % 3) * 10 + (i % 7) * .01;
      p[i][1] = (i % 3) * 10 + (i % 11) * .01;
   }
   auto solve = [&](bool par)
   {
      vector<vector<double>> center(p.begin(), p.begin() + k);
      vector<int> label(n);
      for (int it = 0; it < 25; it++)
      {
         // Assignment: each point is independent.
         if (par)
         {
#pragma omp parallel for num_threads(threads)
            for (int i = 0; i < n; i++)
            {
               double best = 1e100;
               int who = 0;
               for (int c = 0; c < k; c++)
               {
                  double dx = p[i][0] - center[c][0], dy = p[i][1] - center[c][1], d = dx * dx + dy * dy;
                  if (d < best)
                  {
                     best = d;
                     who = c;
                  }
               }
               label[i] = who;
            }
         }
         else
            for (int i = 0; i < n; i++)
            {
               double best = 1e100;
               int who = 0;
               for (int c = 0; c < k; c++)
               {
                  double dx = p[i][0] - center[c][0], dy = p[i][1] - center[c][1], d = dx * dx + dy * dy;
                  if (d < best)
                  {
                     best = d;
                     who = c;
                  }
               }
               label[i] = who;
            }
         // Deterministic serial centroid update (avoids floating-point atomic races).
         vector<vector<double>> sum(k, vector<double>(2));
         vector<int> count(k);
         for (int i = 0; i < n; i++)
         {
            count[label[i]]++;
            for (int d = 0; d < 2; d++)
               sum[label[i]][d] += p[i][d];
         }
         for (int c = 0; c < k; c++)
            if (count[c])
               for (int d = 0; d < 2; d++)
                  center[c][d] = sum[c][d] / count[c];
      }
      return label;
   };
   double t = omp_get_wtime();
   auto a = solve(false);
   double st = omp_get_wtime() - t;
   t = omp_get_wtime();
   auto b = solve(true);
   double pt = omp_get_wtime() - t;
   cout << "Clusters: " << k << '\n';
   status(a == b, st, pt, threads);
   return a != b;
}
