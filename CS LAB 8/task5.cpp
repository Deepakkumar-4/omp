
#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

int main(int argc, char *argv[])
{
    int n = (argc > 1) ? atoi(argv[1]) : 5;
    int threads = (argc > 2) ? atoi(argv[2]) : 4;

    if (n < 3 || threads <= 0)
    {
        cerr << "Matrix size must be at least 3 "
             << "and threads must be positive.\n";
        return 1;
    }

    omp_set_num_threads(threads);

    // Output size without padding
    int outSize = n - 2;

    // Create matrices
    vector<vector<int>> image(n, vector<int>(n));

    vector<vector<double>> serial(
        outSize, vector<double>(outSize));

    vector<vector<double>> parallel(
        outSize, vector<double>(outSize));

    // Initialize image
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            image[i][j] = (i * n + j + 1) * 10;
        }
    }

    // Serial convolution
    double start = omp_get_wtime();

    for (int i = 0; i < outSize; i++)
    {
        for (int j = 0; j < outSize; j++)
        {

            int sum = 0;

            // Apply 3x3 averaging filter
            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    sum += image[i + x][j + y];
                }
            }

            serial[i][j] = sum / 9.0;
        }
    }

    double serialTime = omp_get_wtime() - start;

    // Parallel convolution
    start = omp_get_wtime();

#pragma omp parallel for
    for (int i = 0; i < outSize; i++)
    {
        for (int j = 0; j < outSize; j++)
        {

            int sum = 0;

            // Apply 3x3 averaging filter
            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    sum += image[i + x][j + y];
                }
            }

            parallel[i][j] = sum / 9.0;
        }
    }

    double parallelTime = omp_get_wtime() - start;

    // Verify correctness
    bool correct = (serial == parallel);

    cout << "Input size: " << n << " x " << n << endl;
    cout << "Output size: " << outSize
         << " x " << outSize << endl;
    cout << "Threads: " << threads << endl;

    // Print output for small matrices
    if (n <= 8)
    {
        cout << "Filtered output:\n";

        for (int i = 0; i < outSize; i++)
        {
            for (int j = 0; j < outSize; j++)
            {
                cout << parallel[i][j] << "\t";
            }
            cout << endl;
        }
    }

    cout << "Serial time: "
         << serialTime << " sec\n";

    cout << "Parallel time: "
         << parallelTime << " sec\n";

    cout << "Verification: "
         << (correct ? "PASS" : "FAIL") << endl;

    if (parallelTime > 0)
    {
        cout << "Speedup: "
             << serialTime / parallelTime << endl;
    }

    return correct ? 0 : 1;
}
