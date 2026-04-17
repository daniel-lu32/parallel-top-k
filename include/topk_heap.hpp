#pragma once

#include "topk_algorithm.hpp"

namespace topk {

class HeapTopK : public TopKAlgorithm {
public:
    std::string name() const override;
    Result run(const std::vector<float>& logits, std::size_t k) const override;
};

}  // namespace topk
