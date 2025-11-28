# Download Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Include source directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Unit tests
add_executable(unit_tests
    unit/test_column_analyzer.cpp
    unit/test_parallel_processor.cpp
    ${CMAKE_SOURCE_DIR}/src/ColumnAnalyzer.cpp
    ${CMAKE_SOURCE_DIR}/src/ParallelProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/CSVReader.cpp
)

target_link_libraries(unit_tests
    gtest_main
    Threads::Threads
)

# Link TBB if available
find_package(TBB QUIET)
if(TBB_FOUND)
    target_link_libraries(unit_tests TBB::tbb)
endif()

# E2E tests
add_executable(e2e_tests
    e2e/test_end_to_end.cpp
    ${CMAKE_SOURCE_DIR}/src/DataGenerator.cpp
    ${CMAKE_SOURCE_DIR}/src/CSVReader.cpp
    ${CMAKE_SOURCE_DIR}/src/ColumnAnalyzer.cpp
    ${CMAKE_SOURCE_DIR}/src/ParallelProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/ResultAggregator.cpp
)

target_link_libraries(e2e_tests
    gtest_main
    Threads::Threads
)

# Link TBB if available
if(TBB_FOUND)
    target_link_libraries(e2e_tests TBB::tbb)
endif()

# Register tests
include(GoogleTest)
gtest_discover_tests(unit_tests)
gtest_discover_tests(e2e_tests)
