# Parallel Top-K Selection for LLM Inference

This project implements and compares four Top-K selection strategies for logits used in LLM inference:

- Full sort baseline
- Heap-based Top-K
- Quickselect-based Top-K
- Parallel heap + parallel merge Top-K (OpenMP)

It also includes:

- Input generators (multiple distributions)
- Correctness and edge-case tests
- Runtime reporting in tests
- A benchmark app for end-to-end timing

## Algorithms

All implementations return a `Result` of `(index, value)` pairs ordered by:

1. Higher `value` first
2. Smaller `index` first when values tie

Implemented classes:

- `FullSortTopK` in `src/topk_sort.cpp`
- `HeapTopK` in `src/topk_heap.cpp`
- `QuickselectTopK` in `src/topk_quickselect.cpp`
- `ParallelHeapMergeTopK` in `src/topk_parallel_heapmerge.cpp`

## Project Structure

- `include/` - public headers and interfaces
- `src/` - algorithm and utility implementations
- `apps/`
  - `bench_main.cpp` - benchmark runner
  - `debug_main.cpp` - small sanity/debug run
- `tests/`
  - `test_correctness.cpp` - broad correctness checks + per-input runtime
  - `test_edge_cases.cpp` - edge-case correctness + per-case runtime

## Build

From project root (`parallel-top-k`):

```bash
cmake -S . -B build -DTOPK_ENABLE_OPENMP=ON
cmake --build build -j
```

## macOS OpenMP Notes (Apple Clang)

Install dependencies:

```bash
brew install libomp cmake
```

If CMake cannot detect OpenMP automatically, configure with explicit flags:

```bash
cmake -S . -B build \
  -DTOPK_ENABLE_OPENMP=ON \
  -DOpenMP_CXX_FLAGS="-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include" \
  -DOpenMP_CXX_LIB_NAMES="omp" \
  -DOpenMP_omp_LIBRARY="/opt/homebrew/opt/libomp/lib/libomp.dylib"
cmake --build build -j
```

Verify OpenMP is active:

```bash
grep -nE "OpenMP_CXX_FLAGS|OpenMP_CXX_LIB_NAMES|TOPK_USE_OPENMP" build/CMakeCache.txt
grep -R -n "TOPK_USE_OPENMP" build/CMakeFiles
```

## Run Tests

```bash
./build/test_correctness
./build/test_edge_cases
```

Both tests:

- Assert correctness against `FullSortTopK` reference
- Print runtime as CSV-like lines

Example output format:

```text
runtime_ms,case,full_sort,heap,quickselect,parallel_heapmerge
runtime_ms,uniform_n1024_k10,0.139,0.035,0.487,0.096
...
runtime_ms,total,23.797,3.342,34.954,5.226
```

## Run Benchmark App

```bash
./build/bench [n] [k] [trials] [warmup_trials] [num_threads] [distribution] [seed]
```

Default values:

- `n=1000000`
- `k=100`
- `trials=5`
- `warmup_trials=1`
- `num_threads=0` (auto)
- `distribution=uniform`
- `seed=12345`

Example:

```bash
./build/bench 100000 50 5 1 4 normal 42
```

Output format:

```text
algorithm,n,k,threads,distribution,avg_ms,min_ms,max_ms
full_sort,100000,50,4,normal,...
heap,100000,50,4,normal,...
quickselect,100000,50,4,normal,...
parallel_heapmerge,100000,50,4,normal,...
```

## Run Debug App

```bash
./build/debug_main
```

This prints Top-K results from all four algorithms on a small fixed input.

## Distributions

Supported generator distributions:

- `uniform`
- `normal`
- `zipf_like` (or `zipf`)
- `near_ties`
- `adversarial_sorted` (or `adversarial`)

## Notes

- Parallel implementations use OpenMP only when `TOPK_USE_OPENMP=1` is defined by CMake.
- Without OpenMP, code still runs using the sequential fallback paths.