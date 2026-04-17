#include "../include/topk_heap.hpp"
#include "../include/topk_parallel_heapmerge.hpp"
#include "../include/topk_quickselect.hpp"
#include "../include/topk_sort.hpp"

#include <iostream>
#include <vector>

namespace {

void print_result(const char* name, const topk::Result& r) {
    std::cout << name << ": ";
    for (std::size_t i = 0; i < r.size(); ++i) {
        std::cout << "(" << r[i].index << ", " << r[i].value << ")";
        if (i + 1 < r.size()) {
            std::cout << " ";
        }
    }
    std::cout << "\n";
}

}  // namespace

int main() {
    std::vector<float> logits;
    logits.push_back(0.7f);
    logits.push_back(-1.0f);
    logits.push_back(3.2f);
    logits.push_back(1.1f);
    logits.push_back(3.2f);
    logits.push_back(0.0f);
    logits.push_back(2.7f);
    const std::size_t k = 3;

    topk::FullSortTopK full_sort;
    topk::HeapTopK heap;
    topk::QuickselectTopK quickselect;
    topk::ParallelHeapMergeTopK parallel_merge(4);

    print_result("full_sort", full_sort.run(logits, k));
    print_result("heap", heap.run(logits, k));
    print_result("quickselect", quickselect.run(logits, k));
    print_result("parallel_heapmerge", parallel_merge.run(logits, k));
    return 0;
}
