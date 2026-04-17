#include "../include/topk_sort.hpp"

#include <algorithm>

namespace topk {

std::string FullSortTopK::name() const {
    return "full_sort";
}

Result FullSortTopK::run(const std::vector<float>& logits, std::size_t k) const {
    if (k == 0 || logits.empty()) {
        return Result();
    }

    const std::size_t take = std::min(k, logits.size());

    Result all;
    all.reserve(logits.size());
    for (std::size_t i = 0; i < logits.size(); ++i) {
        Entry e;
        e.index = static_cast<int>(i);
        e.value = logits[i];
        all.push_back(e);
    }

    std::sort(all.begin(), all.end(), better);
    all.resize(take);
    return all;
}

}  // namespace topk
