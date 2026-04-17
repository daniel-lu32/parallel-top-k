#pragma once

#include "topk_common.hpp"

#include <string>
#include <vector>

namespace topk {

class TopKAlgorithm {
public:
    virtual ~TopKAlgorithm() = default;
    virtual std::string name() const = 0;
    virtual Result run(const std::vector<float>& logits, std::size_t k) const = 0;
};

}  // namespace topk
