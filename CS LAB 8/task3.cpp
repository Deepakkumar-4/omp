
#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

int main(int argc, char *argv[])
{
    int n = (argc > 1) ? atoi(argv[1]) : 1000000;
    int threads = (argc > 2) ? atoi(argv[2]) : 4;

    if (n <= 0 || threads <= 0)
    {
        cerr << "Size and thread count must be positive.\n";
        return 1;
    }

    omp_set_num_threads(threads);

    // Create array with values from 0 to 9
    vector<int> arr(n);

    for (int i = 0; i < n; i++)
    {
        arr[i] = i % 10;
    }

    // Serial histogram
    int serial[10] = {0};

    double start = omp_get_wtime();

    for (int i = 0; i < n; i++)
    {
        serial[arr[i]]++;
    }

    double serialTime = omp_get_wtime() - start;

    // Parallel histogram
    int histogram[10] = {0};

    start = omp_get_wtime();

#pragma omp parallel
    {
        // Each thread has its own counters
        int local[10] = {0};

#pragma omp for
        for (int i = 0; i < n; i++)
        {
            local[arr[i]]++;
        }

// Safely combine local results
#pragma omp critical
        {
            for (int j = 0; j < 10; j++)
            {
                histogram[j] += local[j];
            }
        }
    }

    double parallelTime = omp_get_wtime() - start;

    // Verify correctness
    bool correct = true;

    cout << "Array size: " << n << endl;
    cout << "Threads: " << threads << endl;

    cout << "Value  Frequency\n";

    for (int j = 0; j < 10; j++)
    {
        cout << j << "      " << histogram[j] << endl;

        if (histogram[j] != serial[j])
        {
            correct = false;
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
