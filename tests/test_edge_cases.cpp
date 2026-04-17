#include "../include/topk_heap.hpp"
#include "../include/topk_parallel_heapmerge.hpp"
#include "../include/topk_quickselect.hpp"
#include "../include/topk_sort.hpp"
#include "../include/verifier.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
    topk::FullSortTopK reference_algo;
    topk::HeapTopK heap_algo;
    topk::QuickselectTopK quickselect_algo;
    topk::ParallelHeapMergeTopK parallel_algo(4);
    double ms_ref = 0.0;
    double ms_heap = 0.0;
    double ms_qs = 0.0;
    double ms_parallel = 0.0;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "runtime_ms,case,full_sort,heap,quickselect,parallel_heapmerge\n";

    // Empty input
    {
        const std::vector<float> logits;
        const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
        const topk::Result expected = reference_algo.run(logits, 5);
        const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        const topk::Result got_heap = heap_algo.run(logits, 5);
        const std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
        const topk::Result got_qs = quickselect_algo.run(logits, 5);
        const std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
        const topk::Result got_parallel = parallel_algo.run(logits, 5);
        const std::chrono::steady_clock::time_point t4 = std::chrono::steady_clock::now();
        ms_ref += std::chrono::duration<double, std::milli>(t1 - t0).count();
        ms_heap += std::chrono::duration<double, std::milli>(t2 - t1).count();
        ms_qs += std::chrono::duration<double, std::milli>(t3 - t2).count();
        ms_parallel += std::chrono::duration<double, std::milli>(t4 - t3).count();
        std::cout << "runtime_ms,empty_k5,"
                  << std::chrono::duration<double, std::milli>(t1 - t0).count() << ","
                  << std::chrono::duration<double, std::milli>(t2 - t1).count() << ","
                  << std::chrono::duration<double, std::milli>(t3 - t2).count() << ","
                  << std::chrono::duration<double, std::milli>(t4 - t3).count() << "\n";
        topk::assert_correct(got_heap, expected);
        topk::assert_correct(got_qs, expected);
        topk::assert_correct(got_parallel, expected);
    }

    // k = 0
    {
        std::vector<float> logits;
        logits.push_back(1.0f);
        logits.push_back(2.0f);
        logits.push_back(3.0f);
        const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
        const topk::Result expected = reference_algo.run(logits, 0);
        const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        const topk::Result got_heap = heap_algo.run(logits, 0);
        const std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
        const topk::Result got_qs = quickselect_algo.run(logits, 0);
        const std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
        const topk::Result got_parallel = parallel_algo.run(logits, 0);
        const std::chrono::steady_clock::time_point t4 = std::chrono::steady_clock::now();
        ms_ref += std::chrono::duration<double, std::milli>(t1 - t0).count();
        ms_heap += std::chrono::duration<double, std::milli>(t2 - t1).count();
        ms_qs += std::chrono::duration<double, std::milli>(t3 - t2).count();
        ms_parallel += std::chrono::duration<double, std::milli>(t4 - t3).count();
        std::cout << "runtime_ms,k0_n3,"
                  << std::chrono::duration<double, std::milli>(t1 - t0).count() << ","
                  << std::chrono::duration<double, std::milli>(t2 - t1).count() << ","
                  << std::chrono::duration<double, std::milli>(t3 - t2).count() << ","
                  << std::chrono::duration<double, std::milli>(t4 - t3).count() << "\n";
        topk::assert_correct(got_heap, expected);
        topk::assert_correct(got_qs, expected);
        topk::assert_correct(got_parallel, expected);
    }

    // k > n
    {
        std::vector<float> logits;
        logits.push_back(4.0f);
        logits.push_back(-1.0f);
        logits.push_back(2.0f);
        const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
        const topk::Result expected = reference_algo.run(logits, 100);
        const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        const topk::Result got_heap = heap_algo.run(logits, 100);
        const std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
        const topk::Result got_qs = quickselect_algo.run(logits, 100);
        const std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
        const topk::Result got_parallel = parallel_algo.run(logits, 100);
        const std::chrono::steady_clock::time_point t4 = std::chrono::steady_clock::now();
        ms_ref += std::chrono::duration<double, std::milli>(t1 - t0).count();
        ms_heap += std::chrono::duration<double, std::milli>(t2 - t1).count();
        ms_qs += std::chrono::duration<double, std::milli>(t3 - t2).count();
        ms_parallel += std::chrono::duration<double, std::milli>(t4 - t3).count();
        std::cout << "runtime_ms,kgt_n,"
                  << std::chrono::duration<double, std::milli>(t1 - t0).count() << ","
                  << std::chrono::duration<double, std::milli>(t2 - t1).count() << ","
                  << std::chrono::duration<double, std::milli>(t3 - t2).count() << ","
                  << std::chrono::duration<double, std::milli>(t4 - t3).count() << "\n";
        topk::assert_correct(got_heap, expected);
        topk::assert_correct(got_qs, expected);
        topk::assert_correct(got_parallel, expected);
    }

    // Many ties; stable tie-break should be by ascending index.
    {
        std::vector<float> logits;
        logits.push_back(5.0f);
        logits.push_back(5.0f);
        logits.push_back(5.0f);
        logits.push_back(5.0f);
        logits.push_back(4.0f);
        logits.push_back(4.0f);
        const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
        const topk::Result expected = reference_algo.run(logits, 4);
        const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        const topk::Result got_heap = heap_algo.run(logits, 4);
        const std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
        const topk::Result got_qs = quickselect_algo.run(logits, 4);
        const std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
        const topk::Result got_parallel = parallel_algo.run(logits, 4);
        const std::chrono::steady_clock::time_point t4 = std::chrono::steady_clock::now();
        ms_ref += std::chrono::duration<double, std::milli>(t1 - t0).count();
        ms_heap += std::chrono::duration<double, std::milli>(t2 - t1).count();
        ms_qs += std::chrono::duration<double, std::milli>(t3 - t2).count();
        ms_parallel += std::chrono::duration<double, std::milli>(t4 - t3).count();
        std::cout << "runtime_ms,many_ties_k4,"
                  << std::chrono::duration<double, std::milli>(t1 - t0).count() << ","
                  << std::chrono::duration<double, std::milli>(t2 - t1).count() << ","
                  << std::chrono::duration<double, std::milli>(t3 - t2).count() << ","
                  << std::chrono::duration<double, std::milli>(t4 - t3).count() << "\n";
        topk::assert_correct(got_heap, expected);
        topk::assert_correct(got_qs, expected);
        topk::assert_correct(got_parallel, expected);
    }

    std::cout << "test_edge_cases: passed\n";
    std::cout << "runtime_ms,total,"
              << ms_ref << ","
              << ms_heap << ","
              << ms_qs << ","
              << ms_parallel << "\n";
    return 0;
}
