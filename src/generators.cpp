#include "../include/generators.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

namespace topk {

std::vector<float> generate_logits_uniform(std::size_t n, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);
    std::vector<float> out(n);
    for (std::size_t i = 0; i < n; ++i) {
        out[i] = dist(rng);
    }
    return out;
}

std::vector<float> generate_logits_normal(std::size_t n, unsigned seed) {
    std::mt19937 rng(seed);
    std::normal_distribution<float> dist(0.0f, 2.5f);
    std::vector<float> out(n);
    for (std::size_t i = 0; i < n; ++i) {
        out[i] = dist(rng);
    }
    return out;
}

std::vector<float> generate_logits_zipf_like(std::size_t n, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> noise(-0.01f, 0.01f);
    std::vector<float> out(n);
    for (std::size_t i = 0; i < n; ++i) {
        const float rank = static_cast<float>(i + 1);
        out[i] = 1.0f / rank + noise(rng);
    }
    std::shuffle(out.begin(), out.end(), rng);
    return out;
}

std::vector<float> generate_logits_near_ties(std::size_t n, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> bucket(0, 9);
    std::uniform_real_distribution<float> tiny(-1e-5f, 1e-5f);
    std::vector<float> out(n);
    for (std::size_t i = 0; i < n; ++i) {
        out[i] = static_cast<float>(bucket(rng)) + tiny(rng);
    }
    return out;
}

std::vector<float> generate_logits_adversarial_sorted(std::size_t n, unsigned seed) {
    std::vector<float> out = generate_logits_uniform(n, seed);
    std::sort(out.begin(), out.end(), std::greater<float>());
    return out;
}

std::vector<float> generate_logits(const std::string& distribution, std::size_t n, unsigned seed) {
    if (distribution == "uniform") {
        return generate_logits_uniform(n, seed);
    }
    if (distribution == "normal") {
        return generate_logits_normal(n, seed);
    }
    if (distribution == "zipf" || distribution == "zipf_like") {
        return generate_logits_zipf_like(n, seed);
    }
    if (distribution == "near_ties") {
        return generate_logits_near_ties(n, seed);
    }
    if (distribution == "adversarial" || distribution == "adversarial_sorted") {
        return generate_logits_adversarial_sorted(n, seed);
    }
    return generate_logits_uniform(n, seed);
}

}  // namespace topk
