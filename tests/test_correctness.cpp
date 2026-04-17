#include "../include/generators.hpp"
#include "../include/topk_heap.hpp"
#include "../include/topk_parallel_heapmerge.hpp"
#include "../include/topk_quickselect.hpp"
#include "../include/topk_sort.hpp"
#include "../include/verifier.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main() {
    topk::FullSortTopK reference_algo;
    topk::HeapTopK heap_algo;
    topk::QuickselectTopK quickselect_algo;
    topk::ParallelHeapMergeTopK parallel_algo(4);

    std::vector<std::string> dists;
    dists.push_back("uniform");
    dists.push_back("normal");
    dists.push_back("zipf_like");
    dists.push_back("near_ties");
    dists.push_back("adversarial_sorted");

    std::vector<std::size_t> ns;
    ns.push_back(1);
    ns.push_back(8);
    ns.push_back(32);
    ns.push_back(1024);
    ns.push_back(10000);

    std::vector<std::size_t> ks;
    ks.push_back(1);
    ks.push_back(3);
    ks.push_back(10);
    ks.push_back(100);

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "runtime_ms,case,full_sort,heap,quickselect,parallel_heapmerge\n";

    int tested = 0;
    double ms_ref = 0.0;
    double ms_heap = 0.0;
    double ms_qs = 0.0;
    double ms_parallel = 0.0;
    for (std::size_t di = 0; di < dists.size(); ++di) {
        for (std::size_t ni = 0; ni < ns.size(); ++ni) {
            const std::size_t n = ns[ni];
            for (std::size_t ki = 0; ki < ks.size(); ++ki) {
                const std::size_t k = ks[ki];
                const std::vector<float> logits = topk::generate_logits(dists[di], n, 100u + static_cast<unsigned>(tested));

                const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
                const topk::Result expected = reference_algo.run(logits, k);
                const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
                const topk::Result got_heap = heap_algo.run(logits, k);
                const std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
                const topk::Result got_qs = quickselect_algo.run(logits, k);
                const std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
                const topk::Result got_parallel = parallel_algo.run(logits, k);
                const std::chrono::steady_clock::time_point t4 = std::chrono::steady_clock::now();

                ms_ref += std::chrono::duration<double, std::milli>(t1 - t0).count();
                ms_heap += std::chrono::duration<double, std::milli>(t2 - t1).count();
                ms_qs += std::chrono::duration<double, std::milli>(t3 - t2).count();
                ms_parallel += std::chrono::duration<double, std::milli>(t4 - t3).count();
                const double case_ref = std::chrono::duration<double, std::milli>(t1 - t0).count();
                const double case_heap = std::chrono::duration<double, std::milli>(t2 - t1).count();
                const double case_qs = std::chrono::duration<double, std::milli>(t3 - t2).count();
                const double case_parallel = std::chrono::duration<double, std::milli>(t4 - t3).count();

                topk::assert_correct(got_heap, expected);
                topk::assert_correct(got_qs, expected);
                topk::assert_correct(got_parallel, expected);
                std::cout << "runtime_ms,"
                          << dists[di] << "_n" << n << "_k" << k << ","
                          << case_ref << ","
                          << case_heap << ","
                          << case_qs << ","
                          << case_parallel << "\n";
                ++tested;
            }
        }
    }

    std::cout << "test_correctness: passed " << tested << " cases\n";
    std::cout << "runtime_ms,total,"
              << ms_ref << ","
              << ms_heap << ","
              << ms_qs << ","
              << ms_parallel << "\n";
    return 0;
}
