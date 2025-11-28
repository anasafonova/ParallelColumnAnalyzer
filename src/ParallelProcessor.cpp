#include "ParallelProcessor.h"
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>
#include <iostream>

#if !defined(__APPLE__) && defined(__cpp_lib_execution)
#  include <execution>
#  define HAS_EXECUTION_POLICY
#endif

using namespace std;

ParallelStrategy strategyFromInt(int value) {
    switch (value) {
        case 1:
            return ParallelStrategy::EXECUTION_POLICY;
        case 2:
            return ParallelStrategy::THREADS;
        case 3:
            return ParallelStrategy::ASYNC;
        default:
            throw invalid_argument("Unknown strategy: " + to_string(value) +
                                   ". Valid values: 1 (policy), 2 (threads), 3 (async)");
    }
}

string strategyToString(ParallelStrategy strategy) {
    switch (strategy) {
        case ParallelStrategy::EXECUTION_POLICY:
            return "execution-policy";
        case ParallelStrategy::THREADS:
            return "threads";
        case ParallelStrategy::ASYNC:
            return "async";
        default:
            return "unknown";
    }
}

ParallelProcessor::ParallelProcessor(size_t numThreads)
    : numThreads_(numThreads) {
    // If not specified, use the number of hardware threads
    if (numThreads_ == 0) {
        numThreads_ = thread::hardware_concurrency();
        if (numThreads_ == 0) {
            numThreads_ = 8;  // Fallback
        }
    }
}

vector<ColumnResult> ParallelProcessor::process(
        const vector<vector<string>>& columns,
        ParallelStrategy strategy) {

    cout << "Processing " << columns.size() << " columns using strategy: "
         << strategyToString(strategy) << endl;

    if (columns.empty()) {
        cout << "Warning: No columns to process" << endl;
        return {};
    }

    switch (strategy) {
        case ParallelStrategy::EXECUTION_POLICY:
            return processWithExecutionPolicy(columns);
        case ParallelStrategy::THREADS:
            return processWithThreads(columns);
        case ParallelStrategy::ASYNC:
            return processWithAsync(columns);
        default:
            throw invalid_argument("Unknown strategy");
    }
}

vector<ColumnResult> ParallelProcessor::processWithExecutionPolicy(
        const vector<vector<string>>& columns) {

#ifdef HAS_EXECUTION_POLICY
    try {
        cout << "Using C++17 execution policy (parallel)" << endl;

        vector<size_t> indices(columns.size());
        iota(indices.begin(), indices.end(), 0);

        vector<ColumnResult> results(columns.size());

        transform(execution::par,
                  indices.begin(), indices.end(),
                  results.begin(),
                  [&](size_t i) {
                      return ColumnAnalyzer::analyze(i, columns[i]);
                  });

        return results;
    } catch (const exception& e) {
        cerr << "Execution policy failed: " << e.what() << endl;
        cerr << "Falling back to threads..." << endl;
        return processWithThreads(columns);
    }
#else
    cerr << "Warning: Execution policy not available, falling back to threads" << endl;
    return processWithThreads(columns);
#endif
}

vector<ColumnResult> ParallelProcessor::processWithThreads(
        const vector<vector<string>>& columns) const {

    cout << "Using std::thread (" << numThreads_ << " threads)" << endl;

    vector<ColumnResult> results(columns.size());
    vector<thread> threads;

    // Split between threads
    size_t colsPerThread = (columns.size() + numThreads_ - 1) / numThreads_;

    for (size_t t = 0; t < numThreads_; ++t) {
        size_t start = t * colsPerThread;
        size_t end = min(start + colsPerThread, columns.size());

        if (start >= columns.size()) {
            break;
        }

        threads.emplace_back([&, start, end]() {
            for (size_t i = start; i < end; ++i) {
                results[i] = ColumnAnalyzer::analyze(i, columns[i]);
            }
        });
    }

    // Wait for completion
    for (auto& t : threads) {
        t.join();
    }

    return results;
}

vector<ColumnResult> ParallelProcessor::processWithAsync(
        const vector<vector<string>>& columns) {

    cout << "Using std::async (asynchronous tasks)" << endl;

    // Limit concurrent tasks to hardware threads
    size_t maxConcurrent = thread::hardware_concurrency();
    if (maxConcurrent == 0) maxConcurrent = 8;

    vector<ColumnResult> results(columns.size());

    for (size_t start = 0; start < columns.size(); start += maxConcurrent) {
        size_t end = min(start + maxConcurrent, columns.size());
        vector<future<ColumnResult>> batch;

        // Launch batch
        for (size_t i = start; i < end; ++i) {
            batch.push_back(
                    async(launch::async, [i, &columns]() {
                        return ColumnAnalyzer::analyze(i, columns[i]);
                    })
            );
        }

        // Collect batch results
        for (size_t i = 0; i < batch.size(); ++i) {
            results[start + i] = batch[i].get();
        }
    }

    return results;
}
