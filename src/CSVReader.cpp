#include "CSVReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace std;

vector<vector<string>> CSVReader::readColumns(const string& filename) {
    cout << "Reading CSV file: " << filename << endl;

    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filename);
    }

    vector<vector<string>> columns;
    string line;
    size_t rowCount = 0;
    bool isFirstLine = true;

    while (getline(file, line)) {
        if (line.empty()) {
            continue; // Skip empty lines
        }

        auto values = parseLine(line);

        if (isFirstLine) {
            // Header, init columns
            columns.resize(values.size());
            cout << "Detected " << values.size() << " columns" << endl;
            isFirstLine = false;
            continue;  // Skip header
        }

        // Validation: number of values must match number of columns
        if (values.size() != columns.size()) {
            cerr << "Warning: Row " << rowCount
                 << " has " << values.size() << " values, expected " << columns.size()
                 << ". Skipping." << endl;
            continue;
        }

        // Distribute values across columns
        for (size_t col = 0; col < values.size(); ++col) {
            columns[col].push_back(values[col]);
        }

        rowCount++;

        // Progress (every 1000 rows)
        if (rowCount % 1000 == 0) {
            cout << "Read " << rowCount << " rows..." << endl;
        }
    }

    file.close();

    cout << "CSV reading completed: " << rowCount << " rows, "
         << columns.size() << " columns" << endl;

    return columns;
}

vector<string> CSVReader::parseLine(const string& line) {
    vector<string> values;
    stringstream ss(line);
    string value;

    // Simple parsing: split by comma
    while (getline(ss, value, ',')) {
        values.push_back(std::move(value));
    }

    return values;
}