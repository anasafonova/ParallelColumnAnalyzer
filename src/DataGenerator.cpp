#include "DataGenerator.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

void DataGenerator::generateCSV(const string& filename,
                                size_t rows,
                                size_t cols) {
    cout << "Generating CSV: " << filename << endl;
    cout << "Dimensions: " << rows << " rows × " << cols << " columns" << endl;

    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filename);
    }

    // Header
    for (size_t col = 0; col < cols; ++col) {
        file << "col" << col;
        if (col < cols - 1) file << ",";
    }
    file << "\n";

    // Data
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            file << generateValue(col);
            if (col < cols - 1) file << ",";
        }
        file << "\n";

        if ((row + 1) % 1000 == 0) {
            cout << "Generated " << (row + 1) << " / " << rows << " rows..." << endl;
        }
    }

    file.close();
    cout << "CSV generation completed: " << filename << endl;
}

string DataGenerator::generateValue(size_t colIndex) {
    size_t typeIndex = colIndex % 4;

    // Fully random generation (no uniqueness constraint)
    uniform_int_distribution<int> dist(0, 10000);
    int value = dist(rng);

    switch (typeIndex) {
        case 0: {
            // Integer
            return to_string(value);
        }
        case 1: {
            // Float
            ostringstream oss;
            oss << fixed << setprecision(2) << (value * 0.5);
            return oss.str();
        }
        case 2: {
            // String
            return "str_" + to_string(value);
        }
        case 3: {
            // Char
            char c = 'A' + (value % 26);
            return {1, c};
        }
        default:
            return to_string(value);
    }
}
