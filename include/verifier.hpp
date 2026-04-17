#pragma once

#include "topk_common.hpp"

namespace topk {

bool same_result(const Result& a, const Result& b, float eps = 1e-6f);
void assert_correct(const Result& got, const Result& expected);

}  // namespace topk
