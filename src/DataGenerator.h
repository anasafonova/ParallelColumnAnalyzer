#ifndef COLUMNANALYZER_DATAGENERATOR_H
#define COLUMNANALYZER_DATAGENERATOR_H

#include <string>
#include <vector>
#include <random>

class DataGenerator {
public:
    /**
    * Generates CSV file with specified dimensions
    * @param filename Output filename
    * @param rows Number of rows
    * @param cols Number of columns
    */
    void generateCSV(const std::string& filename,
                     size_t rows,
                     size_t cols);

private:
    /**
    * Generates value for a cell
    * @param colIndex Column index
    * @return String representation of value
    */
    std::string generateValue(size_t colIndex);

    // Random number generator
    std::mt19937 rng{std::random_device{}()};
};

#endif //COLUMNANALYZER_DATAGENERATOR_H
