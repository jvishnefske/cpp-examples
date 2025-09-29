.PHONY: all build test coverage coverage-check clean help configure lint ci tox perf perf-record perf-report perf-stat perf-mem

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

# Performance profiling targets
# Run performance analysis on tests
perf: build
	@echo "=== Running performance analysis ==="
	@echo "Recording performance data..."
	@perf record --call-graph=dwarf -o perf.data -- ctest --test-dir $(BUILD_DIR) --output-on-failure
	@echo "Generating performance report..."
	@perf report -i perf.data --stdio > perf-report.txt
	@echo "Performance report saved to perf-report.txt"
	@echo "Raw data available in perf.data"

# Record performance data only
perf-record: build
	@echo "=== Recording performance data ==="
	@if [ -n "$(TARGET)" ]; then \
		echo "Recording performance for target: $(TARGET)"; \
		perf record --call-graph=dwarf -o perf.data -- $(BUILD_DIR)/$(TARGET); \
	else \
		echo "Recording performance for all tests"; \
		perf record --call-graph=dwarf -o perf.data -- ctest --test-dir $(BUILD_DIR) --output-on-failure; \
	fi
	@echo "Performance data recorded to perf.data"

# Generate performance report from existing perf.data
perf-report:
	@echo "=== Generating performance report ==="
	@if [ -f perf.data ]; then \
		perf report -i perf.data --stdio > perf-report.txt; \
		echo "Performance report saved to perf-report.txt"; \
		echo "For interactive report, run: perf report -i perf.data"; \
	else \
		echo "No perf.data file found. Run 'make perf-record' first."; \
	fi

# Get performance statistics
perf-stat: build
	@echo "=== Running performance statistics ==="
	@if [ -n "$(TARGET)" ]; then \
		echo "Performance stats for target: $(TARGET)"; \
		perf stat -d -d -d $(BUILD_DIR)/$(TARGET); \
	else \
		echo "Performance stats for all tests"; \
		perf stat -d -d -d -- ctest --test-dir $(BUILD_DIR) --output-on-failure; \
	fi

# Memory profiling with perf
perf-mem: build
	@echo "=== Running memory profiling ==="
	@if [ -n "$(TARGET)" ]; then \
		echo "Memory profiling for target: $(TARGET)"; \
		perf mem record -o perf-mem.data -- $(BUILD_DIR)/$(TARGET); \
		perf mem report -i perf-mem.data --stdio > perf-mem-report.txt; \
	else \
		echo "Memory profiling for all tests"; \
		perf mem record -o perf-mem.data -- ctest --test-dir $(BUILD_DIR) --output-on-failure; \
		perf mem report -i perf-mem.data --stdio > perf-mem-report.txt; \
	fi
	@echo "Memory profiling report saved to perf-mem-report.txt"

# Full CI pipeline
ci: all lint tox
	@echo "=== All CI checks passed! ==="

# Clean build directory
clean:
	@echo "=== Cleaning build directory ==="
	@rm -rf $(BUILD_DIR)

# Clean everything including Python cache and perf data
clean-all: clean
	@echo "=== Cleaning all build artifacts ==="
	@rm -rf .tox __pycache__ **/__pycache__ *.egg-info .pytest_cache .coverage
	@rm -f perf.data perf-mem.data perf-report.txt perf-mem-report.txt

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
	@echo "  perf       - Run complete performance analysis (record + report)"
	@echo "  perf-record - Record performance data (use TARGET=executable for specific binary)"
	@echo "  perf-report - Generate report from existing perf.data"
	@echo "  perf-stat  - Get performance statistics (use TARGET=executable for specific binary)"
	@echo "  perf-mem   - Run memory profiling (use TARGET=executable for specific binary)"
	@echo "  clean      - Clean build directory"
	@echo "  clean-all  - Clean all build artifacts"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Performance profiling examples:"
	@echo "  make perf                    # Profile all tests"
	@echo "  make perf-record TARGET=middleware/test_demodulator  # Profile specific test"
	@echo "  make perf-stat TARGET=cjunk/cjunk_tests             # Get stats for cjunk tests"
