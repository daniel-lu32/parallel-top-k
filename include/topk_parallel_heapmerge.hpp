#pragma once

#include "topk_algorithm.hpp"

namespace topk {

class ParallelHeapMergeTopK : public TopKAlgorithm {
public:
    explicit ParallelHeapMergeTopK(int num_threads = 0);
    std::string name() const override;
    Result run(const std::vector<float>& logits, std::size_t k) const override;

private:
    int num_threads_;
};

}  // namespace topk
