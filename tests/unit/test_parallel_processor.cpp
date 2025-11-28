#include <gtest/gtest.h>
#include "ParallelProcessor.h"
#include <algorithm>

class ParallelProcessorTest : public ::testing::Test {
protected:
    std::vector<std::vector<std::string>> testData;
    
    void SetUp() override {
        // Create test data: 5 columns × 100 rows
        testData.resize(5);
        for (size_t col = 0; col < 5; ++col) {
            for (size_t row = 0; row < 100; ++row) {
                testData[col].push_back("col" + std::to_string(col) + 
                                       "_val" + std::to_string(row % 10));
            }
        }
    }
};

TEST_F(ParallelProcessorTest, ThreadsStrategy) {
    ParallelProcessor processor(4);
    
    auto results = processor.process(testData, ParallelStrategy::THREADS);
    
    ASSERT_EQ(results.size(), 5);
    for (size_t i = 0; i < results.size(); ++i) {
        EXPECT_EQ(results[i].columnIndex, i);
        EXPECT_EQ(results[i].uniqueCount, 10);  // Each column has 10 unique values
    }
}

TEST_F(ParallelProcessorTest, AsyncStrategy) {
    ParallelProcessor processor(4);
    
    auto results = processor.process(testData, ParallelStrategy::ASYNC);
    
    ASSERT_EQ(results.size(), 5);
    for (size_t i = 0; i < results.size(); ++i) {
        EXPECT_EQ(results[i].columnIndex, i);
        EXPECT_EQ(results[i].uniqueCount, 10);
    }
}

// Only test execution policy if available
#if !defined(__APPLE__) && defined(__cpp_lib_execution)
TEST_F(ParallelProcessorTest, ExecutionPolicyStrategy) {
    ParallelProcessor processor(4);
    
    auto results = processor.process(testData, ParallelStrategy::EXECUTION_POLICY);
    
    ASSERT_EQ(results.size(), 5);
    for (size_t i = 0; i < results.size(); ++i) {
        EXPECT_EQ(results[i].columnIndex, i);
        EXPECT_EQ(results[i].uniqueCount, 10);
    }
}
#endif

TEST_F(ParallelProcessorTest, EmptyData) {
    ParallelProcessor processor(4);
    std::vector<std::vector<std::string>> empty;
    
    auto results = processor.process(empty, ParallelStrategy::THREADS);
    
    EXPECT_TRUE(results.empty());
}

TEST_F(ParallelProcessorTest, SingleColumn) {
    ParallelProcessor processor(4);
    std::vector<std::vector<std::string>> singleCol = {{"a", "b", "a"}};
    
    auto results = processor.process(singleCol, ParallelStrategy::THREADS);
    
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].uniqueCount, 2);
}

TEST_F(ParallelProcessorTest, AllStrategiesProduceSameResults) {
    ParallelProcessor processor(4);
    
    auto resultsThreads = processor.process(testData, ParallelStrategy::THREADS);
    auto resultsAsync = processor.process(testData, ParallelStrategy::ASYNC);
    
    ASSERT_EQ(resultsThreads.size(), resultsAsync.size());
    
    for (size_t i = 0; i < resultsThreads.size(); ++i) {
        EXPECT_EQ(resultsThreads[i].uniqueCount, resultsAsync[i].uniqueCount);
    }
}

TEST(StrategyConversionTest, ValidStrategies) {
    EXPECT_EQ(strategyFromInt(1), ParallelStrategy::EXECUTION_POLICY);
    EXPECT_EQ(strategyFromInt(2), ParallelStrategy::THREADS);
    EXPECT_EQ(strategyFromInt(3), ParallelStrategy::ASYNC);
}

TEST(StrategyConversionTest, InvalidStrategy) {
    EXPECT_THROW(strategyFromInt(0), std::invalid_argument);
    EXPECT_THROW(strategyFromInt(4), std::invalid_argument);
    EXPECT_THROW(strategyFromInt(-1), std::invalid_argument);
}

TEST(StrategyToStringTest, AllStrategies) {
    EXPECT_EQ(strategyToString(ParallelStrategy::EXECUTION_POLICY), "execution-policy");
    EXPECT_EQ(strategyToString(ParallelStrategy::THREADS), "threads");
    EXPECT_EQ(strategyToString(ParallelStrategy::ASYNC), "async");
}

