# cpp-examples

A collection of modern C++ examples demonstrating idioms, patterns, and library usage including JSON parsing, RAII patterns, async programming, and more.

[![C/C++ CI](https://github.com/jvishnefske/cpp-examples/actions/workflows/ci.yml/badge.svg)](https://github.com/jvishnefske/cpp-examples/actions/workflows/ci.yml)

## Quick Start

```bash
# Build
make build

# Test
make test

# Coverage
make coverage
```

## Project Structure

- `cjunk/trivialJson/` - Lightweight JSON parser implementation
- `cjunk/practice/` - Various C++ practice examples (async, promises, etc.)
- `example_streambuffer/` - Custom streambuffer implementations
- `middleware/` - Middleware examples (FFT, serialization)
- `raii.cc` - RAII pattern demonstration

## Requirements

- CMake 3.22+
- C++17 compatible compiler
- Ninja (optional, for faster builds)

## License

See repository for license information.
