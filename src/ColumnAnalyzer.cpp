#include "ColumnAnalyzer.h"

using namespace std;

ColumnResult ColumnAnalyzer::analyze(size_t columnIndex,
                                     const vector<string>& columnData) {
    ColumnResult result(columnIndex);

    // Add to unordered_set — O(1) avg
    // Auto duplicates filtering
    for (const auto& value : columnData) {
        result.uniqueValues.insert(value);
    }

    result.uniqueCount = result.uniqueValues.size();

    return result;
}