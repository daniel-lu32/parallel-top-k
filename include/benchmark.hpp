#pragma once

#include "topk_algorithm.hpp"

#include <cstddef>
#include <string>

namespace topk {

struct BenchmarkConfig {
    std::size_t n;
    std::size_t k;
    int trials;
    int warmup_trials;
    int num_threads;
    std::string distribution;
    unsigned seed;
};

struct BenchmarkResult {
    std::string algorithm;
    std::size_t n;
    std::size_t k;
    int num_threads;
    std::string distribution;
    double avg_ms;
    double min_ms;
    double max_ms;
};

BenchmarkResult run_benchmark(const TopKAlgorithm& algo, const BenchmarkConfig& config);

}  // namespace topk
