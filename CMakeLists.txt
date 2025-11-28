cmake_minimum_required(VERSION 3.20)
project(ParallelColumnAnalyzer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimizations
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

# Executable
add_executable(ParallelColumnAnalyzer
        main.cpp
        src/DataGenerator.cpp
        src/CSVReader.cpp
        src/ColumnAnalyzer.cpp
        src/ParallelProcessor.cpp
        src/ResultAggregator.cpp
)

target_include_directories(ParallelColumnAnalyzer PRIVATE src)

# Threads
find_package(Threads REQUIRED)
target_link_libraries(ParallelColumnAnalyzer Threads::Threads)

# TBB for execution policy (optional)
find_package(TBB QUIET)
if(TBB_FOUND)
    message(STATUS "TBB found, execution policy will be available")
    target_link_libraries(ParallelColumnAnalyzer TBB::tbb)
else()
    message(WARNING "TBB not found, execution policy may not work")
endif()

option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Compilation info
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
