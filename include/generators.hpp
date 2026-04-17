#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace topk {

std::vector<float> generate_logits_uniform(std::size_t n, unsigned seed);
std::vector<float> generate_logits_normal(std::size_t n, unsigned seed);
std::vector<float> generate_logits_zipf_like(std::size_t n, unsigned seed);
std::vector<float> generate_logits_near_ties(std::size_t n, unsigned seed);
std::vector<float> generate_logits_adversarial_sorted(std::size_t n, unsigned seed);
std::vector<float> generate_logits(const std::string& distribution, std::size_t n, unsigned seed);

}  // namespace topk
