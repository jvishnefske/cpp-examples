.PHONY: all build test coverage coverage-check clean help configure lint ci tox

BUILD_DIR = build

# Default target
all: build test coverage

# Configure cmake
configure:
	@echo "=== Configuring project ==="
	@cmake -B $(BUILD_DIR) -S . -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang

# Build the project
build:
	@echo "=== Building project ==="
	@test -f $(BUILD_DIR)/build.ninja || $(MAKE) configure
	@cmake --build $(BUILD_DIR)

# Run tests
test: build
	@echo "=== Running tests ==="
	@ctest --test-dir $(BUILD_DIR) --output-on-failure

# Generate coverage report
coverage: test
	@echo "=== Generating coverage report ==="
	@gcovr --gcov-executable "llvm-cov gcov" --gcov-ignore-errors=no_working_dir_found --fail-under-line 10 --exclude ".*_deps.*" --exclude ".*catch2.*" --root . $(BUILD_DIR)/nonHosted $(BUILD_DIR)/cjunk $(BUILD_DIR)/example_streambuffer $(BUILD_DIR)/middleware $(BUILD_DIR)/CMakeFiles/raii.dir

# Check coverage completeness (files in git vs coverage report)
coverage-check: 
	@echo "=== Checking coverage completeness ==="
	@./check_coverage_completeness.py

# Run static analysis
lint: configure
	@echo "=== Running static analysis ==="
	@files=$$(find middleware cjunk -name "*.cpp" -o -name "*.cc" 2>/dev/null); \
	if [ -n "$$files" ]; then \
		clang-tidy -p $(BUILD_DIR) $$files; \
	else \
		echo "No C++ files found for static analysis"; \
	fi

# Run Python tests via tox
tox:
	@echo "=== Running Python tests via tox ==="
	@tox -e py39

# Full CI pipeline
ci: all lint tox
	@echo "=== All CI checks passed! ==="

# Clean build directory
clean:
	@echo "=== Cleaning build directory ==="
	@rm -rf $(BUILD_DIR)

# Clean everything including Python cache
clean-all: clean
	@echo "=== Cleaning all build artifacts ==="
	@rm -rf .tox __pycache__ **/__pycache__ *.egg-info .pytest_cache .coverage

# Show help
help:
	@echo "Available targets:"
	@echo "  all        - Build, test, and generate coverage (default)"
	@echo "  configure  - Configure cmake build"
	@echo "  build      - Build the project"
	@echo "  test       - Run tests"
	@echo "  coverage   - Generate coverage report"
	@echo "  coverage-check - Check which git files are missing from coverage"
	@echo "  lint       - Run static analysis"
	@echo "  tox        - Run Python tests via tox"
	@echo "  ci         - Full CI pipeline (build + test + coverage + lint + tox)"
	@echo "  clean      - Clean build directory"
	@echo "  clean-all  - Clean all build artifacts"
	@echo "  help       - Show this help message"
