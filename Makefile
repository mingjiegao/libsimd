# Makefile for libsimd - SIMD instruction wrapper library
# PostgreSQL style shared library build

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O3 -fPIC -mavx2 -msse4.2 -std=c99
LDFLAGS = -shared
TEST_CFLAGS = $(CFLAGS) -I. -DTEST_BUILD
TEST_LDFLAGS = -L. -l$(LIBNAME:lib%=%) -Wl,-rpath,.

# Library name
LIBNAME = libsimd
SHARED_LIB = $(LIBNAME).so

# Source files (automatically discover all .c files, excluding test files)
SOURCES = $(filter-out test/%.c, $(wildcard *.c))
OBJECTS = $(SOURCES:.c=.o)

# Test files
TEST_DIR = test
TEST_FUNCTIONAL = $(TEST_DIR)/test_functional
TEST_PERFORMANCE = $(TEST_DIR)/test_performance
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_EXECUTABLES = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_DIR)/%)

# Default target
all: $(SHARED_LIB)

# Build shared library
$(SHARED_LIB): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "Shared library $(SHARED_LIB) built successfully"

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build test executables
$(TEST_DIR)/%: $(TEST_DIR)/%.c $(SHARED_LIB)
	$(CC) $(TEST_CFLAGS) -o $@ $< $(TEST_LDFLAGS)
	@echo "Test executable $@ built successfully"

# Build all tests
tests: $(TEST_EXECUTABLES)
	@echo "All test executables built successfully"

# Run functional tests
test-functional: $(TEST_FUNCTIONAL)
	@echo "Running functional tests..."
	@./$(TEST_FUNCTIONAL)

# Run performance tests
test-performance: $(TEST_PERFORMANCE)
	@echo "Running performance tests..."
	@./$(TEST_PERFORMANCE)

# Run all tests (PostgreSQL style check target)
check: tests
	@echo "Running all libsimd tests..."
	@echo ""
	@echo "=== Functional Tests ==="
	@./$(TEST_FUNCTIONAL)
	@echo ""
	@echo "=== Performance Tests ==="
	@./$(TEST_PERFORMANCE)
	@echo ""
	@echo "All tests completed."

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(SHARED_LIB) $(TEST_EXECUTABLES)
	@echo "Cleaned build artifacts and test executables"

# Install library (optional)
install: $(SHARED_LIB)
	sudo cp $(SHARED_LIB) /usr/local/lib/
	sudo ldconfig
	@echo "Library installed to /usr/local/lib/"

# Show build information
info:
	@echo "Compiler: $(CC)"
	@echo "Library flags: $(CFLAGS)"
	@echo "Test flags: $(TEST_CFLAGS)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Target: $(SHARED_LIB)"
	@echo "Test sources: $(TEST_SOURCES)"
	@echo "Test executables: $(TEST_EXECUTABLES)"

# Help target
help:
	@echo "Available targets:"
	@echo "  all              - Build shared library (default)"
	@echo "  tests            - Build test executables"
	@echo "  test-functional  - Run functional tests"
	@echo "  test-performance - Run performance tests"
	@echo "  check            - Run all tests (functional + performance)"
	@echo "  clean            - Remove build artifacts and tests"
	@echo "  install          - Install library to system"
	@echo "  info             - Show build information"
	@echo "  help             - Show this help message"

# Phony targets
.PHONY: all tests test-functional test-performance check clean install info help