/*
Matrix Multiplication (GEMM) Assignment
Computes C = A * B for dense square matrices. Serial version loops over i, j, k.
Parallel version uses OpenMP `collapse(2)` to parallelize the outer i and j loops, each iteration computes a cell independently.
*/
#include "../../common.hpp"
int main(int argc, char **argv)
{
  int threads = getThreads(argc, argv), n = argc > 2 ? max(1, atoi(argv[2])) : 256;
  vector<double> a(n * n), b(n * n), serial(n * n), parallel(n * n);
  for (int i = 0; i < n * n; i++)
  {
    a[i] = (i % 7) + 1;
    b[i] = (i % 5) + 1;
  }
  auto solve = [&](vector<double> &c, bool ompMode)
  {
    // Every (i,j) computes one output element independently.
    if (ompMode)
    {
#pragma omp parallel for collapse(2) schedule(static) num_threads(threads)
      for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
          double sum = 0;
          for (int k = 0; k < n; k++)
            sum += a[i * n + k] * b[k * n + j];
          c[i * n + j] = sum;
        }
    }
    else
      for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
          double sum = 0;
          for (int k = 0; k < n; k++)
            sum += a[i * n + k] * b[k * n + j];
          c[i * n + j] = sum;
        }
  };
  double t = omp_get_wtime();
  solve(serial, false);
  double st = omp_get_wtime() - t;
  t = omp_get_wtime();
  solve(parallel, true);
  double pt = omp_get_wtime() - t;
  status(serial == parallel, st, pt, threads);
  return serial != parallel;
}
