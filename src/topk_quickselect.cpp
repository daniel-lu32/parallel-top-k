#include "../include/topk_quickselect.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

#ifdef TOPK_USE_OPENMP
#include <omp.h>
#endif

namespace topk {

namespace {

struct PartitionBuckets {
    Result greater;
    Result equal;
    Result less;
};

int resolveThreadCount() {
#ifdef TOPK_USE_OPENMP
    const int p = omp_get_max_threads();
    return (p > 0) ? p : 1;
#else
    return 1;
#endif
}

PartitionBuckets parallelPartition(const Result& s, float pivot, int p) {
    std::vector<Result> greater_local(static_cast<std::size_t>(p));
    std::vector<Result> equal_local(static_cast<std::size_t>(p));
    std::vector<Result> less_local(static_cast<std::size_t>(p));

#ifdef TOPK_USE_OPENMP
#pragma omp parallel num_threads(p)
    {
        const int tid = omp_get_thread_num();
#pragma omp for
        for (std::size_t i = 0; i < s.size(); ++i) {
            const Entry& x = s[i];
            if (x.value > pivot) {
                greater_local[static_cast<std::size_t>(tid)].push_back(x);
            } else if (x.value < pivot) {
                less_local[static_cast<std::size_t>(tid)].push_back(x);
            } else {
                equal_local[static_cast<std::size_t>(tid)].push_back(x);
            }
        }
    }
#else
    for (std::size_t i = 0; i < s.size(); ++i) {
        const Entry& x = s[i];
        if (x.value > pivot) {
            greater_local[0].push_back(x);
        } else if (x.value < pivot) {
            less_local[0].push_back(x);
        } else {
            equal_local[0].push_back(x);
        }
    }
#endif

    PartitionBuckets out;
    for (int t = 0; t < p; ++t) {
        Result& g = greater_local[static_cast<std::size_t>(t)];
        Result& e = equal_local[static_cast<std::size_t>(t)];
        Result& l = less_local[static_cast<std::size_t>(t)];

        out.greater.insert(out.greater.end(), g.begin(), g.end());
        out.equal.insert(out.equal.end(), e.begin(), e.end());
        out.less.insert(out.less.end(), l.begin(), l.end());
    }
    return out;
}

}  // namespace

std::string QuickselectTopK::name() const {
    return "quickselect";
}

Result QuickselectTopK::run(const std::vector<float>& logits, std::size_t k) const {
    if (k == 0 || logits.empty()) {
        return Result();
    }

    Result s;
    s.reserve(logits.size());
    for (std::size_t i = 0; i < logits.size(); ++i) {
        Entry e;
        e.index = static_cast<int>(i);
        e.value = logits[i];
        s.push_back(e);
    }

    std::size_t remaining_k = std::min(k, s.size());
    Result committed;
    committed.reserve(remaining_k);
    const int p = resolveThreadCount();

    while (true) {
        // Choose a simple deterministic pivot from current working set.
        const float pivot = s[s.size() / 2].value;
        PartitionBuckets buckets = parallelPartition(s, pivot, p);

        const std::size_t g_size = buckets.greater.size();
        const std::size_t e_size = buckets.equal.size();

        if (g_size >= remaining_k) {
            if (g_size == remaining_k) {
                Result result = std::move(committed);
                std::sort(buckets.greater.begin(), buckets.greater.end(), better);
                result.insert(result.end(), buckets.greater.begin(), buckets.greater.end());
                std::sort(result.begin(), result.end(), better);
                return result;
            }
            s = std::move(buckets.greater);
            continue;
        }

        if (g_size + e_size >= remaining_k) {
            Result result = std::move(committed);
            result.insert(result.end(), buckets.greater.begin(), buckets.greater.end());
            const std::size_t need = remaining_k - g_size;

            std::sort(buckets.equal.begin(), buckets.equal.end(), better);
            if (need < buckets.equal.size()) {
                buckets.equal.resize(need);
            }
            result.insert(result.end(), buckets.equal.begin(), buckets.equal.end());

            std::sort(result.begin(), result.end(), better);
            return result;
        }

        committed.insert(committed.end(), buckets.greater.begin(), buckets.greater.end());
        committed.insert(committed.end(), buckets.equal.begin(), buckets.equal.end());
        s = std::move(buckets.less);
        remaining_k -= (g_size + e_size);
    }
}

}  // namespace topk
