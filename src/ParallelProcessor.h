#ifndef COLUMNANALYZER_PARALLELPROCESSOR_H
#define COLUMNANALYZER_PARALLELPROCESSOR_H

#include <vector>
#include <string>
#include "ColumnAnalyzer.h"

/**
 * Parallel processing strategy
 */
enum class ParallelStrategy {
    EXECUTION_POLICY = 1,  // C++17 execution policy
    THREADS = 2,           // std::thread
    ASYNC = 3              // std::async
};

/**
 * Convert integer to strategy
 * @param value Strategy number (1, 2, 3)
 * @return Strategy
 */
ParallelStrategy strategyFromInt(int value);

/**
 * Convert strategy to string for output
 */
std::string strategyToString(ParallelStrategy strategy);

/**
 * Processor for parallel column processing
 */
class ParallelProcessor {
public:
    /**
     * Constructor
     * @param numThreads Number of threads (for THREADS strategy)
     */
    explicit ParallelProcessor(size_t numThreads = 8);

    /**
     * Process columns in parallel
     * @param columns Column data
     * @param strategy Parallelism strategy
     * @return Analysis results for each column
     */
    std::vector<ColumnResult> process(
            const std::vector<std::vector<std::string>>& columns,
            ParallelStrategy strategy
    );

private:
    size_t numThreads_;

    /**
     * Processing with execution policy (C++17)
     * Uses std::transform with std::execution::par
     * @param columns Column data
     * @return Analysis results
     */
    std::vector<ColumnResult> processWithExecutionPolicy(
            const std::vector<std::vector<std::string>>& columns
    );

    /**
     * Processing with std::thread (manual thread management)
     * Creates a thread pool and distributes work among them
     * @param columns Column data
     * @return Analysis results
     */
    [[nodiscard]] std::vector<ColumnResult> processWithThreads(
            const std::vector<std::vector<std::string>>& columns
    ) const;

    /**
     * Processing with std::async (asynchronous tasks)
     * Launches async tasks for each column
     * @param columns Column data
     * @return Analysis results
     */
    static std::vector<ColumnResult> processWithAsync(
            const std::vector<std::vector<std::string>>& columns
    );
};

#endif //COLUMNANALYZER_PARALLELPROCESSOR_H
