/*
Blocked GEMM (General Matrix Multiply) Assignment
Implements a naïve blocked matrix multiplication with optional OpenMP
parallelism. The algorithm partitions matrices into blocks of size `block`
and computes each output tile. Parallelism is applied to the outermost
loops when `isParallel` is true.
*/
/*
Blocked GEMM (General Matrix Multiply) Assignment
Implements a naïve blocked matrix multiplication with optional OpenMP parallelism.
The algorithm partitions matrices A and B into blocks of size `block` and computes each output tile.
Parallelism is applied to the outermost loop nest when `isParallel` is true, ensuring each thread works on distinct output tiles.
*/
#include "../../common.hpp"
int main(int argc, char **argv)
{
  int threads = getThreads(argc, argv), n = argc > 2 ? max(1, atoi(argv[2])) : 256, block = argc > 3 ? max(1, atoi(argv[3])) : 32;
  vector<double> A(n * n), B(n * n), one(n * n), two(n * n);
  for (int x = 0; x < n * n; x++)
  {
    A[x] = x % 7;
    B[x] = x % 3;
  }
  /*
  solve(C, isParallel):
    Computes blocked matrix multiplication.
    - Serial: loops over block indices ii, jj, kk and inner i,k,j.
    - Parallel: outer ii, jj loops are parallelized with OpenMP, each thread computes distinct tiles.
*/
auto solve = [&](vector<double> &C, bool isParallel)
  {
    // Each output tile belongs to exactly one worker. The k-block loop stays serial.
    if (isParallel)
    {
#pragma omp parallel for collapse(2) num_threads(threads) schedule(static)
      for (int ii = 0; ii < n; ii += block)
        for (int jj = 0; jj < n; jj += block)
          for (int kk = 0; kk < n; kk += block)
            for (int i = ii; i < min(n, ii + block); i++)
              for (int k = kk; k < min(n, kk + block); k++)
                for (int j = jj; j < min(n, jj + block); j++)
                  C[i * n + j] += A[i * n + k] * B[k * n + j];
    }
    else
      for (int ii = 0; ii < n; ii += block)
        for (int jj = 0; jj < n; jj += block)
          for (int kk = 0; kk < n; kk += block)
            for (int i = ii; i < min(n, ii + block); i++)
              for (int k = kk; k < min(n, kk + block); k++)
                for (int j = jj; j < min(n, jj + block); j++)
                  C[i * n + j] += A[i * n + k] * B[k * n + j];
  };
  double t = omp_get_wtime();
  solve(one, false);
  double st = omp_get_wtime() - t;
  t = omp_get_wtime();
  solve(two, true);
  double pt = omp_get_wtime() - t;
  status(one == two, st, pt, threads);
  return one != two;
}
