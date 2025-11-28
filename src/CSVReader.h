#ifndef COLUMNANALYZER_CSVREADER_H
#define COLUMNANALYZER_CSVREADER_H

#include <string>
#include <vector>
#include <functional>

class CSVReader {
public:
    /**
     * Reads CSV file and returns data by columns
     * @param filename Path to CSV file
     * @return Vector of columns, where each column is a vector of strings
     */
    static std::vector<std::vector<std::string>> readColumns(const std::string& filename);

private:
    /**
     * Parses a single CSV line
     * @param line Line from file
     * @return Vector of values (cells)
     */
    static std::vector<std::string> parseLine(const std::string& line);
};

#endif //COLUMNANALYZER_CSVREADER_H
