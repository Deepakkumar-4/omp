#include "../../common.hpp"
int main(int argc, char **argv)
{
    int threads = getThreads(argc, argv);
    vector<double> coef = {9, -6, 1}; // f(x)=(x-3)^2
    auto solve = [&](bool par)
    {
        double x = 0;
        for (int iter = 0; iter < 5000; iter++)
        {
            double grad = 0;
            if (par)
            {
#pragma omp parallel for reduction(+ : grad) num_threads(threads)
                for (int i = 1; i < (int)coef.size(); i++)
                    grad += i * coef[i] * pow(x, i - 1);
            }
            else
                for (int i = 1; i < (int)coef.size(); i++)
                    grad += i * coef[i] * pow(x, i - 1);
            double next = x - .1 * grad;
            if (abs(next - x) < 1e-8)
                break;
            x = next;
        }
        return x;
    };
    double t = omp_get_wtime();
    auto a = solve(false);
    double st = omp_get_wtime() - t;
    t = omp_get_wtime();
    auto b = solve(true);
    double pt = omp_get_wtime() - t;
    cout << "Minimum x: " << b << '\n';
    status(abs(a - b) < 1e-6, st, pt, threads);
    return abs(a - b) >= 1e-6;
}
