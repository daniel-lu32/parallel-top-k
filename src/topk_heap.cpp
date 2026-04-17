#include "../include/topk_heap.hpp"

#include <algorithm>
#include <queue>

namespace topk {

namespace {

struct WorseFirst {
    bool operator()(const Entry& a, const Entry& b) const {
        return better(a, b);
    }
};

}

std::string HeapTopK::name() const {
    return "heap";
}

Result HeapTopK::run(const std::vector<float>& logits, std::size_t k) const {
    if (k == 0 || logits.empty()) {
        return Result();
    }

    const std::size_t take = std::min(k, logits.size());
    std::priority_queue<Entry, std::vector<Entry>, WorseFirst> heap;

    for (std::size_t i = 0; i < logits.size(); ++i) {
        Entry current;
        current.index = static_cast<int>(i);
        current.value = logits[i];
        if (heap.size() < take) {
            heap.push(current);
            continue;
        }
        if (better(current, heap.top())) {
            heap.pop();
            heap.push(current);
        }
    }

    Result out;
    out.reserve(heap.size());
    while (!heap.empty()) {
        out.push_back(heap.top());
        heap.pop();
    }

    std::sort(out.begin(), out.end(), better);
    return out;
}

}  // namespace topk
