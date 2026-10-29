
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <omp.h>
using namespace std;

int main(int argc, char *argv[])
{
    int n = (argc > 1) ? atoi(argv[1]) : 1000;
    int threads = (argc > 2) ? atoi(argv[2]) : 4;

    if (n <= 0 || threads <= 0)
    {
        cerr << "Size and thread count must be positive.\n";
        return 1;
    }

    omp_set_num_threads(threads);

    // Create array in reverse order
    vector<int> arr(n);
    for (int i = 0; i < n; i++)
    {
        arr[i] = n - i;
    }

    // Serial reference sorting
    vector<int> serial = arr;

    double start = omp_get_wtime();
    sort(serial.begin(), serial.end());
    double serialTime = omp_get_wtime() - start;

    // Parallel odd-even bubble sort
    start = omp_get_wtime();

    for (int phase = 0; phase < n; phase++)
    {

        // Even phase starts at 0, odd phase at 1
        int first = phase % 2;

#pragma omp parallel for
        for (int i = first; i < n - 1; i += 2)
        {
            if (arr[i] > arr[i + 1])
            {
                swap(arr[i], arr[i + 1]);
            }
        }
    }

    double parallelTime = omp_get_wtime() - start;

    // Verify correctness
    bool correct = (arr == serial);

    cout << "Array size: " << n << endl;
    cout << "Threads: " << threads << endl;

    cout << "First values after sorting: ";
    for (int i = 0; i < min(n, 10); i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;

    cout << "Serial std::sort time: "
         << serialTime << " sec\n";

    cout << "Parallel odd-even sort time: "
         << parallelTime << " sec\n";

    cout << "Verification: "
         << (correct ? "PASS" : "FAIL") << endl;

    return correct ? 0 : 1;
}
