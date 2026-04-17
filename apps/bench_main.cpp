#include "../include/benchmark.hpp"
#include "../include/topk_heap.hpp"
#include "../include/topk_parallel_heapmerge.hpp"
#include "../include/topk_quickselect.hpp"
#include "../include/topk_sort.hpp"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::size_t parse_size_t(const char* s, const char* name) {
    try {
        return static_cast<std::size_t>(std::stoull(s));
    } catch (...) {
        throw std::invalid_argument(std::string("invalid value for ") + name);
    }
}

int parse_int(const char* s, const char* name) {
    try {
        return std::stoi(s);
    } catch (...) {
        throw std::invalid_argument(std::string("invalid value for ") + name);
    }
}

}  // namespace

int main(int argc, char** argv) {
    topk::BenchmarkConfig cfg;
    cfg.n = 1000000;
    cfg.k = 100;
    cfg.trials = 5;
    cfg.warmup_trials = 1;
    cfg.num_threads = 0;
    cfg.distribution = "uniform";
    cfg.seed = 12345u;

    if (argc > 1) cfg.n = parse_size_t(argv[1], "n");
    if (argc > 2) cfg.k = parse_size_t(argv[2], "k");
    if (argc > 3) cfg.trials = parse_int(argv[3], "trials");
    if (argc > 4) cfg.warmup_trials = parse_int(argv[4], "warmup_trials");
    if (argc > 5) cfg.num_threads = parse_int(argv[5], "num_threads");
    if (argc > 6) cfg.distribution = argv[6];
    if (argc > 7) cfg.seed = static_cast<unsigned>(parse_size_t(argv[7], "seed"));

    topk::FullSortTopK full_sort;
    topk::HeapTopK heap;
    topk::QuickselectTopK quickselect;
    topk::ParallelHeapMergeTopK parallel_merge(cfg.num_threads);

    std::vector<const topk::TopKAlgorithm*> algos;
    algos.push_back(&full_sort);
    algos.push_back(&heap);
    algos.push_back(&quickselect);
    algos.push_back(&parallel_merge);

    std::cout << "algorithm,n,k,threads,distribution,avg_ms,min_ms,max_ms\n";
    for (std::size_t i = 0; i < algos.size(); ++i) {
        const topk::BenchmarkResult r = topk::run_benchmark(*algos[i], cfg);
        std::cout << r.algorithm << ","
                  << r.n << ","
                  << r.k << ","
                  << r.num_threads << ","
                  << r.distribution << ","
                  << std::fixed << std::setprecision(4)
                  << r.avg_ms << ","
                  << r.min_ms << ","
                  << r.max_ms << "\n";
    }

    return 0;
}
