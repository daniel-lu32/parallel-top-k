#include "../include/benchmark.hpp"

#include "../include/generators.hpp"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <vector>

namespace topk {

BenchmarkResult run_benchmark(const TopKAlgorithm& algo, const BenchmarkConfig& config) {
    if (config.trials <= 0) {
        throw std::invalid_argument("trials must be > 0");
    }
    if (config.warmup_trials < 0) {
        throw std::invalid_argument("warmup_trials must be >= 0");
    }

    std::vector<float> logits = generate_logits(config.distribution, config.n, config.seed);

    for (int i = 0; i < config.warmup_trials; ++i) {
        (void)algo.run(logits, config.k);
    }

    std::vector<double> timings_ms;
    timings_ms.reserve(static_cast<std::size_t>(config.trials));
    for (int i = 0; i < config.trials; ++i) {
        const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
        (void)algo.run(logits, config.k);
        const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        const double ms =
            std::chrono::duration<double, std::milli>(t1 - t0).count();
        timings_ms.push_back(ms);
    }

    std::vector<double>::const_iterator min_it =
        std::min_element(timings_ms.begin(), timings_ms.end());
    std::vector<double>::const_iterator max_it =
        std::max_element(timings_ms.begin(), timings_ms.end());
    double sum = 0.0;
    for (std::size_t i = 0; i < timings_ms.size(); ++i) {
        sum += timings_ms[i];
    }

    BenchmarkResult out;
    out.algorithm = algo.name();
    out.n = config.n;
    out.k = config.k;
    out.num_threads = config.num_threads;
    out.distribution = config.distribution;
    out.avg_ms = sum / static_cast<double>(timings_ms.size());
    out.min_ms = *min_it;
    out.max_ms = *max_it;
    return out;
}

}  // namespace topk
