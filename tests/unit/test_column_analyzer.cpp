#include <gtest/gtest.h>
#include "ColumnAnalyzer.h"

class ColumnAnalyzerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup if needed
    }
};

TEST_F(ColumnAnalyzerTest, EmptyColumn) {
    std::vector<std::string> data;
    
    auto result = ColumnAnalyzer::analyze(0, data);
    
    EXPECT_EQ(result.columnIndex, 0);
    EXPECT_EQ(result.uniqueCount, 0);
    EXPECT_TRUE(result.uniqueValues.empty());
}

TEST_F(ColumnAnalyzerTest, SingleValue) {
    std::vector<std::string> data = {"value1"};
    
    auto result = ColumnAnalyzer::analyze(5, data);
    
    EXPECT_EQ(result.columnIndex, 5);
    EXPECT_EQ(result.uniqueCount, 1);
    EXPECT_EQ(result.uniqueValues.size(), 1);
    EXPECT_TRUE(result.uniqueValues.count("value1") > 0);
}

TEST_F(ColumnAnalyzerTest, AllUnique) {
    std::vector<std::string> data = {"a", "b", "c", "d"};
    
    auto result = ColumnAnalyzer::analyze(0, data);
    
    EXPECT_EQ(result.uniqueCount, 4);
    EXPECT_EQ(result.uniqueValues.size(), 4);
}

TEST_F(ColumnAnalyzerTest, WithDuplicates) {
    std::vector<std::string> data = {"a", "b", "a", "c", "b", "a"};
    
    auto result = ColumnAnalyzer::analyze(0, data);
    
    EXPECT_EQ(result.uniqueCount, 3);
    EXPECT_EQ(result.uniqueValues.size(), 3);
    EXPECT_TRUE(result.uniqueValues.count("a") > 0);
    EXPECT_TRUE(result.uniqueValues.count("b") > 0);
    EXPECT_TRUE(result.uniqueValues.count("c") > 0);
}

TEST_F(ColumnAnalyzerTest, AllSame) {
    std::vector<std::string> data(100, "same");
    
    auto result = ColumnAnalyzer::analyze(0, data);
    
    EXPECT_EQ(result.uniqueCount, 1);
    EXPECT_EQ(result.uniqueValues.size(), 1);
}

TEST_F(ColumnAnalyzerTest, LargeDataset) {
    std::vector<std::string> data;
    data.reserve(10000);
    for (int i = 0; i < 10000; ++i) {
        data.push_back("value_" + std::to_string(i % 100));
    }
    
    auto result = ColumnAnalyzer::analyze(0, data);
    
    EXPECT_EQ(result.uniqueCount, 100);
}

