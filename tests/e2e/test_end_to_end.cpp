#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "DataGenerator.h"
#include "CSVReader.h"
#include "ParallelProcessor.h"
#include "ResultAggregator.h"

namespace fs = std::filesystem;

class EndToEndTest : public ::testing::Test {
protected:
    std::string testDir = "test_data";
    std::string testFile;
    
    void SetUp() override {
        // Create test directory
        fs::create_directories(testDir);
        testFile = testDir + "/test.csv";
    }
    
    void TearDown() override {
        // Cleanup
        fs::remove_all(testDir);
    }
};

TEST_F(EndToEndTest, GenerateAndAnalyze) {
    // Generate test CSV
    DataGenerator generator;
    generator.generateCSV(testFile, 100, 5);
    
    // Verify file exists
    ASSERT_TRUE(fs::exists(testFile));
    
    // Read and analyze
    auto columns = CSVReader::readColumns(testFile);
    
    EXPECT_EQ(columns.size(), 5);
    EXPECT_EQ(columns[0].size(), 100);
    
    // Process with all strategies
    ParallelProcessor processor(2);
    
    auto resultsThreads = processor.process(columns, ParallelStrategy::THREADS);
    auto resultsAsync = processor.process(columns, ParallelStrategy::ASYNC);
    
    EXPECT_EQ(resultsThreads.size(), 5);
    EXPECT_EQ(resultsAsync.size(), 5);
    
    // Verify all strategies produce same counts
    for (size_t i = 0; i < resultsThreads.size(); ++i) {
        EXPECT_EQ(resultsThreads[i].uniqueCount, resultsAsync[i].uniqueCount);
    }
}

TEST_F(EndToEndTest, SaveAndLoadResults) {
    // Generate small dataset
    DataGenerator generator;
    generator.generateCSV(testFile, 50, 3);
    
    // Analyze
    auto columns = CSVReader::readColumns(testFile);
    ParallelProcessor processor(2);
    auto results = processor.process(columns, ParallelStrategy::THREADS);
    
    // Save results
    std::string countsFile = testDir + "/counts.csv";
    std::string fullFile = testDir + "/full.csv";
    
    ResultAggregator aggregator;
    aggregator.saveCountsToFile(results, countsFile);
    aggregator.saveFullResultsToFile(results, fullFile);
    
    // Verify files exist
    ASSERT_TRUE(fs::exists(countsFile));
    ASSERT_TRUE(fs::exists(fullFile));
    
    // Verify counts file format
    std::ifstream file(countsFile);
    std::string header;
    std::getline(file, header);
    EXPECT_EQ(header, "Column,UniqueCount");
    
    // Count lines (should be 3 columns + 1 header)
    int lineCount = 1;  // header
    std::string line;
    while (std::getline(file, line)) {
        lineCount++;
    }
    EXPECT_EQ(lineCount, 4);  // 1 header + 3 data lines
}

TEST_F(EndToEndTest, LargeDataset) {
    // Test with larger dataset
    DataGenerator generator;
    generator.generateCSV(testFile, 1000, 10);
    
    auto columns = CSVReader::readColumns(testFile);
    
    EXPECT_EQ(columns.size(), 10);
    EXPECT_EQ(columns[0].size(), 1000);
    
    ParallelProcessor processor(4);
    auto results = processor.process(columns, ParallelStrategy::THREADS);
    
    EXPECT_EQ(results.size(), 10);
    
    // Each result should have reasonable unique count
    for (const auto& result : results) {
        EXPECT_GT(result.uniqueCount, 0);
        EXPECT_LE(result.uniqueCount, 1000);
    }
}

TEST_F(EndToEndTest, InvalidFile) {
    // Try to read non-existent file
    EXPECT_THROW(CSVReader::readColumns("nonexistent.csv"), std::runtime_error);
}

TEST_F(EndToEndTest, EmptyCSV) {
    // Create empty CSV
    std::ofstream empty(testFile);
    empty.close();
    
    auto columns = CSVReader::readColumns(testFile);
    
    EXPECT_TRUE(columns.empty());
}

