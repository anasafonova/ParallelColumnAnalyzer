#include "ResultAggregator.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;

void ResultAggregator::printResults(const vector<ColumnResult>& results) const {
    cout << "\n=== Results ===" << endl;
    cout << "Total columns: " << results.size() << endl << endl;

    size_t totalUniqueValues = 0;

    for (const auto& result : results) {
        totalUniqueValues += result.uniqueCount;
        cout << "Column " << setw(3) << result.columnIndex
             << ": " << setw(6) << result.uniqueCount << " unique values" << endl;
    }

    cout << "\n" << string(50, '-') << endl;
    cout << "Total unique values across all columns: " << totalUniqueValues << endl;
}

void ResultAggregator::printDetailedResults(const vector<ColumnResult>& results,
                                            size_t samplesPerColumn) const {
    cout << "\n=== Detailed Results ===" << endl;

    for (const auto& result : results) {
        cout << "\nColumn " << result.columnIndex << ":" << endl;
        cout << "  Unique count: " << result.uniqueCount << endl;

        if (!result.uniqueValues.empty()) {
            cout << "  Sample values (first " << min(samplesPerColumn, result.uniqueCount) << "):" << endl;

            size_t count = 0;
            for (const auto& value : result.uniqueValues) {
                cout << "    - " << value << endl;
                if (++count >= samplesPerColumn) break;
            }
        }
    }
}

void ResultAggregator::saveCountsToFile(const vector<ColumnResult>& results,
                                        const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open output file: " + filename);
    }

    file << "Column,UniqueCount" << endl;

    for (const auto& result : results) {
        file << result.columnIndex << "," << result.uniqueCount << endl;
    }

    // RAII: destructor closes file automatically on scope exit
    cout << "Counts saved to: " << filename << endl;
}

void ResultAggregator::saveFullResultsToFile(const vector<ColumnResult>& results,
                                             const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open output file: " + filename);
    }

    // Format: Column,UniqueCount,UniqueValues (separated by semicolon)
    file << "Column,UniqueCount,UniqueValues" << endl;

    for (const auto& result : results) {
        file << result.columnIndex << "," << result.uniqueCount << ",";

        // Write all unique values separated by semicolon
        bool first = true;
        for (const auto& value : result.uniqueValues) {
            if (!first) {
                file << ";";
            }
            file << value;
            first = false;
        }

        file << endl;
    }

    // RAII: destructor closes file automatically on scope exit
    cout << "Full results saved to: " << filename << endl;
}

void ResultAggregator::printSummary(const vector<ColumnResult>& results) const {
    if (results.empty()) {
        cout << "No results to summarize" << endl;
        return;
    }

    // Statistics
    size_t totalUnique = 0;
    size_t minUnique = results[0].uniqueCount;
    size_t maxUnique = results[0].uniqueCount;

    for (const auto& result : results) {
        totalUnique += result.uniqueCount;
        minUnique = min(minUnique, result.uniqueCount);
        maxUnique = max(maxUnique, result.uniqueCount);
    }

    double avgUnique = static_cast<double>(totalUnique) / static_cast<double>(results.size());

    cout << "\n=== Summary Statistics ===" << endl;
    cout << "Columns analyzed:        " << results.size() << endl;
    cout << "Total unique values:     " << totalUnique << endl;
    cout << "Average per column:      " << fixed << setprecision(1) << avgUnique << endl;
    cout << "Min unique in column:    " << minUnique << endl;
    cout << "Max unique in column:    " << maxUnique << endl;
}