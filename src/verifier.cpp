#include "../include/verifier.hpp"

#include <cmath>
#include <stdexcept>

namespace topk {

bool same_result(const Result& a, const Result& b, float eps) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i].index != b[i].index) {
            return false;
        }
        if (std::fabs(a[i].value - b[i].value) > eps) {
            return false;
        }
    }
    return true;
}

void assert_correct(const Result& got, const Result& expected) {
    if (!same_result(got, expected)) {
        throw std::runtime_error("Top-k result mismatch");
    }
}

}  // namespace topk
