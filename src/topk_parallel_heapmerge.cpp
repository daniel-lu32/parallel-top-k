#include "../include/topk_parallel_heapmerge.hpp"

#include <algorithm>
#include <queue>
#include <utility>
#include <vector>

#ifdef TOPK_USE_OPENMP
#include <omp.h>
#endif

namespace topk {

namespace {

struct WorseFirst {
    bool operator()(const Entry& a, const Entry& b) const {
        return better(a, b);
    }
};

Result mergeTwoTopK(const Result& left, const Result& right, std::size_t k) {
    if (k == 0) {
        return Result();
    }

    std::priority_queue<Entry, std::vector<Entry>, WorseFirst> minHeap;
    const std::size_t limit = k;

    for (std::size_t i = 0; i < left.size(); ++i) {
        const Entry& e = left[i];
        if (minHeap.size() < limit) {
            minHeap.push(e);
        } else if (better(e, minHeap.top())) {
            minHeap.pop();
            minHeap.push(e);
        }
    }

    for (std::size_t i = 0; i < right.size(); ++i) {
        const Entry& e = right[i];
        if (minHeap.size() < limit) {
            minHeap.push(e);
        } else if (better(e, minHeap.top())) {
            minHeap.pop();
            minHeap.push(e);
        }
    }

    Result merged;
    merged.reserve(minHeap.size());
    while (!minHeap.empty()) {
        merged.push_back(minHeap.top());
        minHeap.pop();
    }
    std::sort(merged.begin(), merged.end(), better);
    return merged;
}

Result parallelMergeTopK(std::vector<Result> localResults, std::size_t k, int p) {
    if (localResults.empty() || k == 0) {
        return Result();
    }

    while (localResults.size() > 1) {
        const std::size_t pair_count = localResults.size() / 2;
        std::vector<Result> nextLevel(pair_count + (localResults.size() % 2));

#ifdef TOPK_USE_OPENMP
#pragma omp parallel for num_threads(p)
#endif
        for (std::size_t i = 0; i < pair_count; ++i) {
            const Result& l1 = localResults[2 * i];
            const Result& l2 = localResults[2 * i + 1];
            nextLevel[i] = mergeTwoTopK(l1, l2, k);
        }

        // If there is an odd tail list, carry it to the next level.
        if (localResults.size() % 2 == 1) {
            nextLevel[pair_count] = std::move(localResults.back());
        }

        localResults = std::move(nextLevel);
    }

    return localResults[0];
}

int resolveThreadCount(int configured_threads, std::size_t n) {
    if (n == 0) {
        return 1;
    }

    int threads = configured_threads;
    if (threads <= 0) {
#ifdef TOPK_USE_OPENMP
        threads = omp_get_max_threads();
#else
        threads = 1;
#endif
    }
    if (threads < 1) {
        threads = 1;
    }

    const int max_by_data = static_cast<int>(n);
    if (threads > max_by_data) {
        threads = max_by_data;
    }
    return threads;
}

}  // namespace

ParallelHeapMergeTopK::ParallelHeapMergeTopK(int num_threads) : num_threads_(num_threads) {}

std::string ParallelHeapMergeTopK::name() const {
    return "parallel_heapmerge";
}

Result ParallelHeapMergeTopK::run(const std::vector<float>& logits, std::size_t k) const {
    if (k == 0 || logits.empty()) {
        return Result();
    }

    const std::size_t n = logits.size();
    const std::size_t take = std::min(k, n);
    const int p = resolveThreadCount(num_threads_, n);

    // Split input into p contiguous chunks and compute each chunk's local top-k.
    std::vector<Result> localResults(static_cast<std::size_t>(p));

#ifdef TOPK_USE_OPENMP
#pragma omp parallel for num_threads(p)
#endif
    for (int chunk_id = 0; chunk_id < p; ++chunk_id) {
        const std::size_t start = (n * static_cast<std::size_t>(chunk_id)) / static_cast<std::size_t>(p);
        const std::size_t end = (n * static_cast<std::size_t>(chunk_id + 1)) / static_cast<std::size_t>(p);

        std::priority_queue<Entry, std::vector<Entry>, WorseFirst> minHeap;
        for (std::size_t i = start; i < end; ++i) {
            Entry e;
            e.index = static_cast<int>(i);
            e.value = logits[i];

            if (minHeap.size() < take) {
                minHeap.push(e);
            } else if (better(e, minHeap.top())) {
                minHeap.pop();
                minHeap.push(e);
            }
        }

        Result localTopK;
        localTopK.reserve(minHeap.size());
        while (!minHeap.empty()) {
            localTopK.push_back(minHeap.top());
            minHeap.pop();
        }
        localResults[static_cast<std::size_t>(chunk_id)] = std::move(localTopK);
    }

    Result finalResult = parallelMergeTopK(std::move(localResults), take, p);
    std::sort(finalResult.begin(), finalResult.end(), better);
    if (finalResult.size() > take) {
        finalResult.resize(take);
    }
    return finalResult;
}

}  // namespace topk
