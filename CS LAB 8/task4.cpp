
#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

int main(int argc, char *argv[])
{
    int n = (argc > 1) ? atoi(argv[1]) : 8;
    int threads = (argc > 2) ? atoi(argv[2]) : 4;

    if (n <= 0 || threads <= 0)
    {
        cerr << "Size and thread count must be positive.\n";
        return 1;
    }

    omp_set_num_threads(threads);

    // Create matrices
    vector<int> matrix(n * n);
    vector<int> serial(n * n);
    vector<int> parallel(n * n);

    // Initialize input matrix
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matrix[i * n + j] = i * n + j + 1;
        }
    }

    // Serial transpose
    double start = omp_get_wtime();

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            serial[j * n + i] = matrix[i * n + j];
        }
    }

    double serialTime = omp_get_wtime() - start;

    // Parallel transpose
    start = omp_get_wtime();

#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            parallel[j * n + i] = matrix[i * n + j];
        }
    }

    double parallelTime = omp_get_wtime() - start;

    // Verify result
    bool correct = (serial == parallel);

    cout << "Matrix size: " << n << " x " << n << endl;
    cout << "Threads: " << threads << endl;

    // Print only small matrices
    if (n <= 8)
    {
        cout << "Transposed matrix:\n";

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                cout << parallel[i * n + j] << "\t";
            }
            cout << endl;
        }
    }

    cout << "Serial time: " << serialTime << " sec\n";
    cout << "Parallel time: " << parallelTime << " sec\n";

    cout << "Verification: "
         << (correct ? "PASS" : "FAIL") << endl;

    if (parallelTime > 0)
    {
        cout << "Speedup: "
             << serialTime / parallelTime << endl;
    }

    return correct ? 0 : 1;
}
