#pragma once

#include <cstddef>
#include <vector>

namespace topk {

struct Entry {
    int index;
    float value;
};

inline bool better(const Entry& a, const Entry& b) {
    if (a.value != b.value) {
        return a.value > b.value;
    }
    return a.index < b.index;
}

using Result = std::vector<Entry>;

}  // namespace topk
