# Makefile for cpp-examples
# Wraps CMake build system with standard targets

BUILD_DIR := cmake-build
CMAKE_GENERATOR := Ninja
CMAKE_FLAGS := -DENABLE_COVERAGE=ON

.PHONY: all build test coverage clean help

all: build

# Build the project using CMake
build:
	@cmake -B $(BUILD_DIR) -S . -G$(CMAKE_GENERATOR) $(CMAKE_FLAGS)
	@cmake --build $(BUILD_DIR)

# Run tests via CTest
test: build
	@cd $(BUILD_DIR) && ctest --output-on-failure

# Generate coverage report using gcov/lcov
coverage: test
	@mkdir -p $(BUILD_DIR)/coverage
	@lcov --capture --directory $(BUILD_DIR) --output-file $(BUILD_DIR)/coverage/coverage.info --ignore-errors mismatch,source,gcov
	@lcov --remove $(BUILD_DIR)/coverage/coverage.info '/usr/*' '*/thirdparty/*' '*/_deps/*' --output-file $(BUILD_DIR)/coverage/coverage.info --ignore-errors unused,source
	@genhtml $(BUILD_DIR)/coverage/coverage.info --output-directory $(BUILD_DIR)/coverage/html --ignore-errors source || true
	@echo "Coverage report generated at $(BUILD_DIR)/coverage/html/index.html"

# Clean build artifacts
clean:
	@rm -rf $(BUILD_DIR)

help:
	@echo "Available targets:"
	@echo "  build    - Build the project (default)"
	@echo "  test     - Run tests via CTest"
	@echo "  coverage - Generate coverage report"
	@echo "  clean    - Remove build directory"
	@echo "  help     - Show this help message"
