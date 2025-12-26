# Design Document - cpp-examples

## Overview

This repository provides educational C++ examples demonstrating modern idioms, patterns, and library usage.

## MVP Functional Requirements

### Build System
- [x] FR-1: Project builds with CMake 3.22+
- [x] FR-2: C++17 standard compliance
- [x] FR-3: Strict compiler warnings (-Werror, -Wall, -Wextra, -Wpedantic)
- [x] FR-4: Coverage instrumentation support via gcov

### Testing
- [x] FR-5: Unit tests using Catch2 framework
- [x] FR-6: CTest integration for test discovery
- [x] FR-7: Tests executable via `make test`

### CI/CD
- [x] FR-8: GitHub Actions workflow for continuous integration
- [x] FR-9: Build verification on push and pull request
- [x] FR-10: Coverage artifact generation and upload

### Code Examples
- [x] FR-11: JSON parser implementation (trivialJson)
- [x] FR-12: RAII pattern demonstration (raii.cc)
- [x] FR-13: Async programming examples (asyncStdin, chainablePromise)
- [x] FR-14: Stream buffer customization examples

## Traceability Matrix

| Requirement | Implementation | Test |
|-------------|----------------|------|
| FR-1 | CMakeLists.txt | CI build step |
| FR-2 | CMakeLists.txt (CMAKE_CXX_STANDARD 17) | Compilation |
| FR-3 | CMakeLists.txt compiler flags | Compilation |
| FR-4 | CMakeLists.txt ENABLE_COVERAGE | make coverage |
| FR-5 | cjunk/trivialJson/tests/main.cpp | make test |
| FR-6 | enable_testing(), add_test() | ctest |
| FR-7 | Makefile test target | CI |
| FR-8 | .github/workflows/ci.yml | GitHub Actions |
| FR-9 | ci.yml on: push/pull_request | GitHub Actions |
| FR-10 | ci.yml coverage artifact | GitHub Actions |
| FR-11 | cjunk/trivialJson/ | jsonTests |
| FR-12 | raii.cc | raii test |
| FR-13 | cjunk/practice/*.cc | Compilation |
| FR-14 | example_streambuffer/ | testHello |

## Architecture Notes

- Uses FetchContent for dependencies (Boost, Catch2, fmt)
- Supports both MSVC and GCC/Clang toolchains
- Optional benchmarking support via ENABLE_BENCHMARK flag
