#pragma once
#include <omp.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

// Usage: ./taskN [size] [threads]. All programs print CSV rows.
inline int argument(int argc, char** argv, int position, int fallback) {
    if (argc <= position) return fallback;
    try {
        int value = std::stoi(argv[position]);
        if (value < 1) throw std::invalid_argument("nonpositive");
        return value;
    } catch (...) {
        std::cerr << "Arguments must be positive integers: [size] [threads]\n";
        std::exit(EXIT_FAILURE);
    }
}
inline void setup_threads(int threads) {
    omp_set_dynamic(0);
    omp_set_num_threads(threads);
}
template <class Function> double seconds(Function operation) {
    const double start = omp_get_wtime();
    operation();
    return omp_get_wtime() - start;
}
inline void print_result(const std::string& task, int size, int threads,
                         double serial, double parallel, bool valid) {
    std::cout << std::fixed << std::setprecision(8)
              << task << ',' << size << ',' << threads << ','
              << serial << ',' << parallel << ','
              << (parallel > 0 ? serial / parallel : 0) << ','
              << (parallel > 0 ? serial / parallel / threads : 0) << ','
              << (valid ? "PASS" : "FAIL") << '\n';
}
