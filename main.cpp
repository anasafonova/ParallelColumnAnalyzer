#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include "DataGenerator.h"
#include "CSVReader.h"
#include "ParallelProcessor.h"
#include "ResultAggregator.h"

using namespace std;
using namespace chrono;

void printHelp() {
    cout << "Column Analyzer - CSV data generator and analyzer\n\n";
    cout << "Usage:\n";
    cout << "  Generate CSV:\n";
    cout << "    ./ColumnAnalyzer --generate --output <file> --rows <N> --cols <M>\n\n";
    cout << "  Analyze CSV:\n";
    cout << "    ./ColumnAnalyzer --analyze --input <file> [--strategy <mode>] [--threads <N>]\n\n";
    cout << "  Options:\n";
    cout << "    --help              Show this help message\n";
    cout << "    --generate          Generate CSV file\n";
    cout << "    --analyze           Analyze CSV file\n";
    cout << "    --output <file>     Output file path (default: data.csv)\n";
    cout << "    --input <file>      Input file path\n";
    cout << "    --rows <N>          Number of rows (default: 10000)\n";
    cout << "    --cols <M>          Number of columns (default: 50)\n";
    cout << "    --strategy <mode>   Parallel strategy mode (default: 2)\n";
    cout << "                        1 = execution-policy (C++17 std::execution::par)\n";
    cout << "                        2 = threads (manual std::thread management)\n";
    cout << "                        3 = async (std::async tasks)\n";
    cout << "    --threads <N>       Number of threads for mode 2 (default: 8)\n\n";
    cout << "Examples:\n";
    cout << "  ./ColumnAnalyzer --generate --output data.csv --rows 10000 --cols 50\n";
    cout << "  ./ColumnAnalyzer --analyze --input data.csv --strategy 1\n";
    cout << "  ./ColumnAnalyzer --analyze --input data.csv --strategy 2 --threads 4\n";
    cout << "  ./ColumnAnalyzer --analyze --input data.csv --strategy 3\n";
}

struct Config {
    string mode;           // "generate" or "analyze"
    string outputFile = "data.csv";
    string inputFile;
    size_t rows = 10000;
    size_t cols = 50;
    int strategyMode = 2;
    size_t numThreads = 8;
};

Config parseArgs(int argc, char* argv[]) {
    Config config;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg.size() < 3 || arg[0] != '-' || arg[1] != '-') {
            cerr << "Invalid argument: " << arg << endl;
            cerr << "Arguments must start with '--'" << endl;
            exit(1);
        }

        string option = arg.substr(2);

        char firstChar = option.empty() ? '\0' : option[0];

        switch (firstChar) {
            case 'h':  // --help
                if (option == "help") {
                    printHelp();
                    exit(0);
                }
                break;

            case 'g':  // --generate
                if (option == "generate") {
                    config.mode = "generate";
                }
                break;

            case 'a':  // --analyze
                if (option == "analyze") {
                    config.mode = "analyze";
                }
                break;

            case 'o':  // --output
                if (option == "output" && i + 1 < argc) {
                    config.outputFile = argv[++i];
                }
                break;

            case 'i':  // --input
                if (option == "input" && i + 1 < argc) {
                    config.inputFile = argv[++i];
                }
                break;

            case 'r':  // --rows
                if (option == "rows" && i + 1 < argc) {
                    config.rows = stoul(argv[++i]);
                }
                break;

            case 'c':  // --cols
                if (option == "cols" && i + 1 < argc) {
                    config.cols = stoul(argv[++i]);
                }
                break;

            case 's':  // --strategy
                if (option == "strategy" && i + 1 < argc) {
                    config.strategyMode = stoi(argv[++i]);
                }
                break;

            case 't':  // --threads
                if (option == "threads" && i + 1 < argc) {
                    config.numThreads = stoul(argv[++i]);
                }
                break;

            default:
                cerr << "Unknown argument: " << arg << endl;
                cerr << "Use --help for usage information" << endl;
                exit(1);
        }
    }

    return config;
}

void generateMode(const Config& config) {
    cout << "=== Generate Mode ===" << endl;
    cout << "Output file: " << config.outputFile << endl;
    cout << "Dimensions: " << config.rows << " rows × " << config.cols << " columns\n" << endl;

    std::filesystem::path outputPath(config.outputFile);
    if (outputPath.has_parent_path()) {
        std::filesystem::create_directories(outputPath.parent_path());
    }

    DataGenerator generator;

    auto start = chrono::high_resolution_clock::now();
    generator.generateCSV(config.outputFile, config.rows, config.cols);
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "\nGeneration completed in " << duration.count() << " ms" << endl;
}

void analyzeMode(const Config& config) {
    cout << "=== Analyze Mode ===" << endl;
    cout << "Input file: " << config.inputFile << endl;

    ParallelStrategy strategy = strategyFromInt(config.strategyMode);
    cout << "Strategy: " << strategyToString(strategy) << endl;
    if (strategy == ParallelStrategy::THREADS) {
        cout << "Threads: " << config.numThreads << endl;
    }
    cout << endl;

    try {
        cout << "Reading CSV..." << endl;

        auto startRead = high_resolution_clock::now();
        auto columns = CSVReader::readColumns(config.inputFile);
        auto endRead = high_resolution_clock::now();
        auto readDuration = duration_cast<milliseconds>(endRead - startRead);

        cout << "Reading completed in " << readDuration.count() << " ms" << endl;
        cout << "Loaded: " << columns.size() << " columns × "
             << (columns.empty() ? 0 : columns[0].size()) << " rows\n" << endl;

        cout << "Analyzing columns..." << endl;

        ParallelProcessor processor(config.numThreads);

        auto startAnalysis = high_resolution_clock::now();
        auto results = processor.process(columns, strategy);
        auto endAnalysis = high_resolution_clock::now();
        auto analysisDuration = duration_cast<milliseconds>(endAnalysis - startAnalysis);

        cout << "\nAnalysis completed in " << analysisDuration.count() << " ms" << endl;

        ResultAggregator aggregator;
        aggregator.printResults(results);
        aggregator.printSummary(results);

        string outputBaseName = config.inputFile;

        size_t dotPos = outputBaseName.find_last_of('.');
        if (dotPos != string::npos) {
            outputBaseName = outputBaseName.substr(0, dotPos);
        }

        aggregator.saveCountsToFile(results, outputBaseName + "_counts.csv");

        aggregator.saveFullResultsToFile(results, outputBaseName + "_full.csv");

        // For small CSVs
        if (!results.empty() && results.size() <= 10) {
            aggregator.printDetailedResults(results, 5);
        }

        cout << "\n=== Performance ===" << endl;
        cout << "Reading time:  " << readDuration.count() << " ms" << endl;
        cout << "Analysis time: " << analysisDuration.count() << " ms" << endl;
        cout << "Total time:    " << (readDuration + analysisDuration).count() << " ms" << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        throw;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== Testing C++17 Execution Policy Support ===" << std::endl;
    std::cout << "C++ Standard: " << __cplusplus << std::endl;
#ifdef __cpp_lib_execution
    std::cout << "std::execution IS available" << std::endl;
    std::cout << "__cpp_lib_execution = " << __cpp_lib_execution << std::endl;

    std::vector<int> v(100);
    std::for_each(std::execution::par, v.begin(), v.end(),
                  [](int& x) { x = 42; });

    std::cout << "Parallel execution works!" << std::endl;
#else
    std::cout << "std::execution NOT available" << std::endl;
#endif

    std::cout << "=== Test completed ===" << std::endl;

    if (argc < 2) {
        printHelp();
        return 1;
    }

    try {
        Config config = parseArgs(argc, argv);

        if (config.mode == "generate") {
            generateMode(config);
        }
        else if (config.mode == "analyze") {
            if (config.inputFile.empty()) {
                cerr << "Error: --input <file> is required for --analyze mode" << endl;
                return 1;
            }
            analyzeMode(config);
        }
        else {
            cerr << "Error: specify --generate or --analyze" << endl;
            printHelp();
            return 1;
        }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
