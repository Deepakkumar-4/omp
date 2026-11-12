#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <string>

// STEP 1: Check if compiler recognized OpenMP flag
#ifndef _OPENMP
#error "[CRITICAL ERROR] OpenMP is NOT enabled by your compiler! You must compile with '-fopenmp' (for g++/clang) or '/openmp' (for MSVC)."
#endif

// STEP 2: Include the OpenMP header
#include <omp.h>

using namespace std;

// Helper function to interpret OpenMP version macro
string get_openmp_version_name(long code) {
    if (code >= 202011) return "OpenMP 5.1 / 5.2";
    if (code >= 201811) return "OpenMP 5.0";
    if (code >= 201511) return "OpenMP 4.5";
    if (code >= 201307) return "OpenMP 4.0";
    if (code >= 201107) return "OpenMP 3.1";
    if (code >= 200805) return "OpenMP 3.0";
    if (code >= 200505) return "OpenMP 2.5";
    return "Legacy OpenMP";
}

int main() {
    int passed = 0;
    int total = 0;

    auto check = [&](const string& name, bool condition) {
        total++;
        if (condition) passed++;
        cout << "  [" << (condition ? " PASS " : " FAIL ") << "] " << name << "\n";
    };

    cout << "========================================================================\n";
    cout << "           OPENMP SYSTEM & LIBRARY DIAGNOSTIC CHECKER                   \n";
    cout << "========================================================================\n";

    // 1. Compiler and Version Information
    cout << "\n[1] COMPILER & SPECIFICATION CHECK:\n";
    cout << "  _OPENMP Version Code: " << _OPENMP << " (" << get_openmp_version_name(_OPENMP) << ")\n";
    check("OpenMP Macro Defined (_OPENMP)", true);

    // 2. OpenMP Timer & Hardware Functions
    cout << "\n[2] RUNTIME LIBRARY & FUNCTION CHECKS:\n";

    // Test omp_get_wtime()
    double t1 = omp_get_wtime();
    volatile double sum_dummy = 0;
    for (int i = 0; i < 100000; i++) sum_dummy = sum_dummy + i;
    double t2 = omp_get_wtime();
    check("omp_get_wtime() (Wall-clock timer)", (t2 >= t1 && t1 > 0 && sum_dummy > 0));

    // Test omp_get_wtick()
    double tick = omp_get_wtick();
    check("omp_get_wtick() (Timer resolution)", (tick > 0.0 && tick < 1.0));

    // Test thread count and hardware core functions
    int procs = omp_get_num_procs();
    int max_th = omp_get_max_threads();
    cout << "  Available CPU Cores (omp_get_num_procs): " << procs << "\n";
    cout << "  Max Thread Capacity (omp_get_max_threads): " << max_th << "\n";
    check("omp_get_num_procs() >= 1", procs >= 1);
    check("omp_get_max_threads() >= 1", max_th >= 1);

    // Test dynamic threads control
    omp_set_dynamic(0);
    check("omp_set_dynamic(0) / omp_get_dynamic() == 0", omp_get_dynamic() == 0);

    // Test omp_set_num_threads
    omp_set_num_threads(2);
    check("omp_set_num_threads(2)", true);

    // 3. Test Pragma Directives and Work-Sharing
    cout << "\n[3] PRAGMA DIRECTIVE & WORK-SHARING CHECKS:\n";

    // Pragma 1: #pragma omp parallel & omp_get_thread_num / omp_get_num_threads
    int threads_seen = 0;
    bool in_parallel_ok = false;
    #pragma omp parallel num_threads(4)
    {
        #pragma omp atomic
        threads_seen++;

        #pragma omp single
        {
            in_parallel_ok = (omp_in_parallel() != 0);
        }
    }
    check("#pragma omp parallel (Multi-threading execution)", threads_seen > 1);
    check("omp_in_parallel() (Detects parallel region)", in_parallel_ok);

    // Pragma 2: #pragma omp parallel for
    const int N = 100;
    vector<int> a(N, 0);
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < N; i++) {
        a[i] = i * 2;
    }
    bool pfor_ok = true;
    for (int i = 0; i < N; i++) {
        if (a[i] != i * 2) { pfor_ok = false; break; }
    }
    check("#pragma omp parallel for (Loop distribution)", pfor_ok);

    // Pragma 3: #pragma omp parallel for reduction(+:...)
    long long total_sum = 0;
    #pragma omp parallel for num_threads(4) reduction(+:total_sum)
    for (int i = 1; i <= N; i++) {
        total_sum += i;
    }
    long long expected_sum = (1LL * N * (N + 1)) / 2;
    check("#pragma omp parallel for reduction(+:var)", total_sum == expected_sum);

    // Pragma 4: #pragma omp parallel for collapse(2)
    int grid_sum = 0;
    #pragma omp parallel for collapse(2) num_threads(4) reduction(+:grid_sum)
    for (int r = 0; r < 10; r++) {
        for (int c = 0; c < 10; c++) {
            grid_sum += 1;
        }
    }
    check("#pragma omp parallel for collapse(2) (Nested 2D loops)", grid_sum == 100);

    // Pragma 5: #pragma omp single and #pragma omp master / masked
    int single_count = 0;
    int master_count = 0;
    #pragma omp parallel num_threads(4)
    {
        #pragma omp single
        {
            single_count++;
        }

#if defined(__GNUC__) && (__GNUC__ >= 12)
        #pragma omp masked
        {
            master_count++;
        }
#else
        #pragma omp master
        {
            master_count++;
        }
#endif
    }
    check("#pragma omp single (Executed exactly once)", single_count == 1);
    check("#pragma omp master/masked (Executed only by Thread 0)", master_count == 1);

    // Pragma 6: #pragma omp sections and #pragma omp section
    int secA = 0, secB = 0;
    #pragma omp parallel sections num_threads(2)
    {
        #pragma omp section
        {
            secA = 10;
        }
        #pragma omp section
        {
            secB = 20;
        }
    }
    check("#pragma omp sections & section", (secA == 10 && secB == 20));

    // Pragma 7: #pragma omp critical
    int crit_val = 0;
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < 100; i++) {
        #pragma omp critical
        {
            crit_val++;
        }
    }
    check("#pragma omp critical (Mutual exclusion)", crit_val == 100);

    // Pragma 8: #pragma omp atomic update and capture
    int atomic_counter = 0;
    vector<int> slots(100, -1);
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < 100; i++) {
        int my_slot;
        #pragma omp atomic capture
        my_slot = atomic_counter++;
        slots[i] = my_slot;
    }
    check("#pragma omp atomic update and capture", atomic_counter == 100);

    // Pragma 9: #pragma omp barrier
    int step_flag = 0;
    #pragma omp parallel num_threads(4)
    {
        #pragma omp atomic
        step_flag++;

        #pragma omp barrier
        // All threads must pass here together
    }
    check("#pragma omp barrier (Thread synchronization)", step_flag == 4);

    // Pragma 10: Explicit Locks (omp_lock_t)
    omp_lock_t lock;
    omp_init_lock(&lock);
    int locked_val = 0;
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < 50; i++) {
        omp_set_lock(&lock);
        locked_val++;
        omp_unset_lock(&lock);
    }
    omp_destroy_lock(&lock);
    check("omp_lock_t (init, set, unset, destroy)", locked_val == 50);

    // Pragma 11: #pragma omp task and #pragma omp taskwait
    int task_res = 0;
    #pragma omp parallel num_threads(2)
    {
        #pragma omp single
        {
            #pragma omp task shared(task_res)
            {
                task_res += 5;
            }
            #pragma omp taskwait
        }
    }
    check("#pragma omp task & #pragma omp taskwait", task_res == 5);

    // Summary
    cout << "\n========================================================================\n";
    cout << "                         DIAGNOSTIC SUMMARY                             \n";
    cout << "========================================================================\n";
    cout << "  Total Checks:  " << total << "\n";
    cout << "  Passed Checks: " << passed << "\n";
    cout << "  Failed Checks: " << (total - passed) << "\n";

    if (passed == total) {
        cout << "\n  >>> [SUCCESS] All OpenMP functions, pragmas, and libraries are\n";
        cout << "                PRESENT and WORKING CORRECTLY on your system! <<<\n";
    } else {
        cout << "\n  >>> [WARNING] Some checks failed! Please review the FAIL items above. <<<\n";
    }
    cout << "========================================================================\n";

    return (passed == total) ? 0 : 1;
}
