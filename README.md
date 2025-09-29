# C++ Examples Repository

A comprehensive collection of modern C++17 examples, algorithms, and learning projects demonstrating best practices, design patterns, and advanced programming techniques.

[![C/C++ CI](https://github.com/jvishnefske/cpp-examples/actions/workflows/ccpp.yml/badge.svg)](https://github.com/jvishnefske/cpp-examples/actions/workflows/ccpp.yml)

## 🚀 Quick Start

```bash
# Build with Ninja (recommended)
cmake -B build -S . -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
cmake --build build

# Or use the Makefile wrapper
make all

# Run tests
ctest --test-dir build --output-on-failure

# Generate coverage report  
make coverage

# Check coverage completeness
make coverage-check
```

## 📁 Project Structure

### Core Components

- **`cjunk/`** - Experimental C++ code and design patterns
  - **`trivialJson/`** - Custom JSON parsing library with memory optimization
  - **`practice/`** - Algorithm implementations, networking examples, and learning exercises
  - **`glFrontEnd/`** - OpenGL frontend demonstrations
  - **`swradio/`** - Software-defined radio signal processing examples
  - **`pocoDemo/`** - POCO C++ library integration examples

- **`middleware/`** - Signal processing and FFT utilities
  - Digital signal processing classes
  - FFT implementations using FFTW3
  - Demodulator components for radio applications

- **`example_streambuffer/`** - Custom stream buffer implementations
- **`nonHosted/http3Client/`** - HTTP/3 client implementation with fuzzing support
- **`serialization/`** - Various serialization examples including FlatBuffers

## 🔧 Build System

### Requirements
- **CMake** 3.22+
- **C++17** compatible compiler (Clang 19+ or GCC 11+)
- **Ninja** build system (recommended)

### Dependencies (Auto-fetched)
- **Catch2** v3.5.4 - Unit testing framework
- **fmt** - String formatting library  
- **Boost** 1.81.0 - Header-only libraries
- **GoogleTest** - Additional testing framework

### Optional Dependencies
- **OpenCV** - Computer vision examples
- **FFTW3** - Fast Fourier Transform library
- **libv4l2** - Video4Linux support for radio examples
- **POCO** - Network and utility libraries

### Build Targets

```bash
make help              # Show all available targets
make configure         # Configure CMake build
make build            # Build all targets
make test             # Run test suite
make coverage         # Generate code coverage report
make coverage-check   # Check coverage completeness against git files
make lint             # Run static analysis (clang-tidy)
make ci               # Full CI pipeline
make clean            # Clean build artifacts
```

## 🧪 Testing & Quality

### Test Coverage
- **Current Coverage**: 75%+ line coverage across core components
- **File Coverage**: 31.1% (14/45) of tracked C++ files
- **Testing Frameworks**: Catch2, GoogleTest, custom test harnesses

### Code Quality Tools
- **Static Analysis**: clang-tidy with C++ Core Guidelines
- **Sanitizers**: AddressSanitizer, UndefinedBehaviorSanitizer
- **Coverage**: llvm-cov integration with gcovr
- **CI/CD**: GitHub Actions with SonarCloud integration

### Running Tests
```bash
# Run all tests
ctest --test-dir build

# Run specific test categories  
ctest --test-dir build -R practice_tests
ctest --test-dir build -R http3_test

# Run with verbose output
ctest --test-dir build --output-on-failure --verbose
```

## 📚 Key Examples

### Algorithms & Data Structures
- **Financial Analysis** - Stock profit calculations
- **Functional Programming** - Immutable arrays with map/reduce
- **Small String Optimization** - Compile-time string class
- **Circular Queue** - Lock-free data structures
- **Kalman Filters** - Extended Kalman filter implementations

### Networking & Protocols  
- **HTTP/3 Client** - Modern HTTP/3 implementation with QPACK
- **WebSocket Servers** - Beast and ASIO-based implementations
- **UDP Servers** - High-performance UDP socket examples
- **Network Frame Streaming** - Real-time data streaming

### Signal Processing
- **FFT Implementations** - FFTW3 integration and custom algorithms  
- **Demodulators** - Digital signal processing components
- **Software Radio** - V4L2-based radio signal processing

### System Programming
- **RAII Examples** - Resource management best practices  
- **EGL Integration** - OpenGL context management
- **Memory Management** - Custom allocators and smart pointers
- **JSON Processing** - High-performance parsing with different approaches

## 🔍 Advanced Features

### Fuzzing Support
```bash
# Build with fuzzing (requires Clang)
CC=clang CXX=clang++ cmake -B build -S .
cmake --build build --target http3_fuzzer

# Run fuzzer
./build/nonHosted/http3Client/http3_fuzzer
```

### Benchmarking
```bash
# Enable benchmark builds
cmake -B build -S . -DENABLE_BENCHMARK=ON
cmake --build build

# Run benchmarks  
./build/cjunk/trivialJson/experimental/tests/experimental_benchmark
```

### Coverage Analysis
```bash
# Generate detailed coverage
make coverage

# Check which files are missing from coverage
./check_coverage_completeness.py

# Coverage with specific exclusions
gcovr --gcov-executable "llvm-cov gcov" --exclude ".*test.*" build/
```

## 🏗️ Architecture Highlights

- **Modern C++17**: Extensive use of constexpr, auto, lambdas, and RAII
- **Template Metaprogramming**: Compile-time string processing and type deduction  
- **Memory Safety**: Smart pointers, bounds checking, and sanitizer integration
- **Performance**: Zero-cost abstractions and optimization-friendly code
- **Modularity**: Well-separated concerns with clear interfaces

## 🤝 Contributing

This repository serves as both a learning resource and a testbed for C++ techniques. Feel free to:

- Explore the examples for learning purposes
- Suggest improvements via issues  
- Add new algorithms or design patterns
- Improve documentation and comments

## 📄 License

This project is intended for educational and demonstration purposes. See individual files for specific licensing information.

## 🔗 Related Resources

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Catch2 Documentation](https://github.com/catchorg/Catch2)
- [CMake Best Practices](https://cmake.org/cmake/help/latest/)
- [Modern C++ Features](https://github.com/AnthonyCalandra/modern-cpp-features)