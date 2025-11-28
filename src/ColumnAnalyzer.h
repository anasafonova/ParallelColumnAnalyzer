#ifndef COLUMNANALYZER_COLUMNANALYZER_H
#define COLUMNANALYZER_COLUMNANALYZER_H

#include <string>
#include <vector>
#include <unordered_set>

/**
 * Result of analyzing a single column
 */
struct ColumnResult {
    size_t columnIndex;
    std::unordered_set<std::string> uniqueValues;
    size_t uniqueCount;

    explicit ColumnResult(size_t index = 0)
            : columnIndex(index), uniqueCount(0) {}
};

/**
 * Column analyzer - finds unique values in a column
 */
class ColumnAnalyzer {
public:
    /**
     * Analyzes a single column and finds unique values
     * @param columnIndex Column index
     * @param columnData Column data (vector of strings)
     * @return Analysis result
     */
    static ColumnResult analyze(size_t columnIndex,
                         const std::vector<std::string>& columnData) ;
};

#endif //COLUMNANALYZER_COLUMNANALYZER_H
