#ifndef COLUMNANALYZER_RESULTAGGREGATOR_H
#define COLUMNANALYZER_RESULTAGGREGATOR_H

#include <vector>
#include <string>
#include "ColumnAnalyzer.h"

/**
 * Analysis results aggregator
 * Collects and outputs results for all columns
 */
class ResultAggregator {
public:
    /**
     * Print results to console
     * @param results Analysis results for all columns
     */
    void printResults(const std::vector<ColumnResult>& results) const;

    /**
     * Print results with sample unique values
     * @param results Analysis results
     * @param samplesPerColumn Number of samples per column
     */
    void printDetailedResults(const std::vector<ColumnResult>& results,
                              size_t samplesPerColumn = 5) const;

    /**
     * Save only unique value counts
     * @param results Analysis results
     * @param filename Output file path
     */
    void saveCountsToFile(const std::vector<ColumnResult>& results,
                          const std::string& filename) const;

    /**
     * Save complete lists of unique values
     * @param results Analysis results
     * @param filename Output file path
     */
    void saveFullResultsToFile(const std::vector<ColumnResult>& results,
                               const std::string& filename) const;

    /**
     * Print summary statistics
     * @param results Analysis results
     */
    void printSummary(const std::vector<ColumnResult>& results) const;
};

#endif //COLUMNANALYZER_RESULTAGGREGATOR_H
