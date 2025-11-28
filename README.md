# ParallelColumnAnalyzer ✨

CSV column's uniqueness analyzer with multiple parallelization strategies implemented in C++17.

## 🚀 Overview

This project implements three parallel processing strategies for analyzing unique values in CSV columns:

1. **Execution Policy** - C++17 `std::execution::par` with parallel algorithms
2. **Manual Threads** - `std::thread` with work distribution across thread pool
3. **Async Tasks** - `std::async` with automatic task scheduling

Each strategy demonstrates different approaches to parallelization in modern C++.

---

## 📋 Requirements

- **C++17** compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- **CMake** 3.20 or higher
- **TBB** (Intel Threading Building Blocks) - required for execution policy strategy
- **Ninja** build system (optional, recommended)

---

## 🛠️ Building

### Local Build (Linux/macOS)

```bash
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

### Docker Build

```bash
docker build -t parallel-analyzer .
```

---

## 💡 Usage

### Generate Test Data

```bash
./ParallelColumnAnalyzer --generate --output data.csv --rows 10000 --cols 50
```

**Options:**
- `--output <file>` - Output file path (default: `data.csv`)
- `--rows <N>` - Number of rows (default: `10000`)
- `--cols <M>` - Number of columns (default: `50`)

### Analyze CSV

```bash
./ParallelColumnAnalyzer --analyze --input data.csv --strategy 2 --threads 8
```

**Options:**
- `--input <file>` - Input CSV file path (required)
- `--strategy <mode>` - Parallel strategy (default: `2`)
  - `1` = Execution Policy
  - `2` = Manual Threads
  - `3` = Async Tasks
- `--threads <N>` - Number of threads for strategy 2 (default: `8`)

**Output Files:**
- `<input>_counts.csv` - Unique value counts per column
- `<input>_full.csv` - Complete lists of unique values

---

## ⚡ Examples

```bash
# Generate test data
./ParallelColumnAnalyzer --generate --output data.csv --rows 10000 --cols 50

# Analyze with manual threads
./ParallelColumnAnalyzer --analyze --input data.csv --strategy 2 --threads 8

# Analyze with async tasks
./ParallelColumnAnalyzer --analyze --input data.csv --strategy 3
```

### Docker Examples

```bash
# Create volume for data persistence
docker volume create column-data

# Generate data
docker run --rm -v column-data:/app/build parallel-analyzer \
  --generate --output data.csv --rows 1000 --cols 10

# Analyze data (using threads strategy - guaranteed to work)
docker run --rm -v column-data:/app/build parallel-analyzer \
  --analyze --input data.csv --strategy 2 --threads 4

# View results
docker run --rm -v column-data:/app/build alpine ls -la /app/build
```

---

## ⚙️ Platform Notes

### Execution Policy Support

| Platform | Status | Notes |
|----------|--------|-------|
| **Linux** | ⚠️ Limited | Requires GCC with specially built libstdc++ + TBB |
| **macOS** | ⚠️ Not tested | Requires GCC with TBB (Clang implementation incomplete) |
| **Windows** | ✅ Supported | MSVC 2019+ with proper STL implementation |
| **Docker** | ⚠️ Fallback | Ubuntu image lacks execution policy support, uses threads strategy |

**Note:** Execution policy (strategy 1) is experimental in GCC and requires libstdc++ built with `_GLIBCXX_USE_TBB_PAR_BACKEND`. Standard Ubuntu/Debian packages do not include this. The program automatically falls back to manual threads strategy when execution policy is unavailable.

**Recommendation:** Use strategy 2 (threads) or 3 (async) for guaranteed cross-platform compatibility.

---

## 🧪 Testing

Unit and end-to-end tests are provided to demonstrate testing practices.

### Build and Run Tests

```bash
# Enable tests in CMake
cmake -DBUILD_TESTS=ON ..
ninja

# Run all tests
ctest --verbose

# Or run individually
./tests/unit_tests
./tests/e2e_tests
```

**Test Coverage:**
- Unit tests for `ColumnAnalyzer` and `ParallelProcessor`
- End-to-end tests for full workflow
- Strategy comparison and validation tests

---

## 📝 License

This project is created as a technical exercise.

---

## 📚 References

- [C++17 Parallel Algorithms](https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag_t)
- [Intel TBB Documentation](https://www.intel.com/content/www/us/en/docs/onetbb/developer-guide-api-reference/2021-11/overview.html)
- [Google Test Framework](https://google.github.io/googletest/)

---

**Built with ❤️ and C++17**
