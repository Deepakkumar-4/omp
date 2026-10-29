
#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
using namespace std;

int main(int argc, char *argv[])
{
    int n = (argc > 1) ? atoi(argv[1]) : 1000000;
    int threads = (argc > 2) ? atoi(argv[2]) : 4;

    omp_set_num_threads(threads);

    // Create array
    vector<int> arr(n);

    for (int i = 0; i < n; i++)
    {
        arr[i] = i;
    }

    // Search for the last element
    int target = n - 1;

    // Serial search
    int serialIndex = -1;
    double start = omp_get_wtime();

    for (int i = 0; i < n; i++)
    {
        if (arr[i] == target)
        {
            serialIndex = i;
            break;
        }
    }

    double serialTime = omp_get_wtime() - start;

    // Parallel search
    int parallelIndex = -1;
    start = omp_get_wtime();

#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        if (arr[i] == target)
        {
#pragma omp critical
            {
                if (parallelIndex == -1 || i < parallelIndex)
                {
                    parallelIndex = i;
                }
            }
        }
    }

    double parallelTime = omp_get_wtime() - start;

    // Print result
    cout << "Array size: " << n << endl;
    cout << "Threads: " << threads << endl;
    cout << "Target: " << target << endl;
    cout << "Found at index: " << parallelIndex << endl;

    cout << "Serial time: " << serialTime << " sec\n";
    cout << "Parallel time: " << parallelTime << " sec\n";

    cout << "Verification: "
         << (serialIndex == parallelIndex ? "PASS" : "FAIL")
         << endl;

    if (parallelTime > 0)
    {
        cout << "Speedup: "
             << serialTime / parallelTime << endl;
    }

    return 0;
}
