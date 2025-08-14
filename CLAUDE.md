# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and Development Commands

### Building the project
```bash
# Initial build setup with Ninja generator
cmake -B build -S . -G Ninja

# Build all targets
cmake --build build

# Alternative: Use the provided test script (includes build + test + coverage)
./test.sh
```

### Running tests
```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run specific test
ctest --test-dir build -R <test_name>
```

### Coverage and Analysis
```bash
# Generate coverage report (requires build with coverage enabled)
gcovr --fail-under-line 10 --exclude build/_deps build

# Run static analysis (clang-tidy)
clang-tidy -p build `find middleware -name *.cpp`

# Run Python tests (via tox)
tox -e py39
```

## Project Architecture

This is a C++ examples repository with the following key structure:

### Main Components
- **Root level**: Contains basic RAII examples and main CMakeLists.txt
- **cjunk/**: Collection of experimental C++ code and examples
  - **trivialJson/**: Custom JSON library implementation with SmallMemoryModel and standard versions
  - **practice/**: Various C++ learning examples (async, networking, algorithms, etc.)
  - **swradio/**: Software-defined radio examples
  - **glFrontEnd/**: OpenGL frontend examples
  - **pocoJunk/**: Poco library experiments
- **middleware/**: FFT and signal processing utilities
- **example_streambuffer/**: Stream buffer examples
- **serialization/**: Serialization examples including FlatBuffers

### Key Libraries and Dependencies
- **Testing**: Catch2 (v3.5.4) for unit testing
- **Formatting**: fmt library for string formatting
- **Boost**: Header-only Boost libraries (v1.81.0)
- **Build**: CMake with Ninja generator preferred
- **Coverage**: gcovr for coverage reporting
- **Static Analysis**: clang-tidy integration

### Build Configuration
- C++17 standard
- Strict compiler warnings (-Werror on all warnings)
- Stack protection and security features enabled
- Coverage builds enabled by default
- Both shared and static analysis builds supported

### Testing Strategy
- Catch2-based unit tests in relevant subdirectories
- CTest integration for test discovery and execution
- Python tests via tox for any Python components
- Coverage reporting with minimum 10% line coverage requirement

## Development Notes

### CMake Structure
The project uses a modular CMake setup:
- Dependencies fetched via FetchContent from `cmake/dependencies.cmake`
- Each major component has its own CMakeLists.txt
- Libraries are built as separate targets (trivialJson, smallTrivialJson, etc.)
- commit after changes when all test pass.
- all commits should have exactly one author.

### CI/CD
- GitHub Actions workflow builds with Ubuntu + Ninja
- SonarCloud integration for code quality analysis
- Automatic dependency management via CMake FetchContent