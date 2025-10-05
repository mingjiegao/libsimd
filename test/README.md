# libsimd Test Suite

This directory contains comprehensive test suites for the libsimd library functions.

## Test Files

### test_functional.c
Comprehensive functional tests that verify the correctness of all SIMD functions:

- **lfind8 tests**: Basic functionality, edge cases, large arrays
- **lfind8_le tests**: Less-than-or-equal functionality 
- **lfind32 tests**: 32-bit integer search functionality
- **Vector alignment tests**: Boundary condition handling
- **Correctness verification**: Comparison against linear search implementations

### test_performance.c
Performance benchmarks comparing SIMD implementations against standard algorithms:

- **Multiple array sizes**: 1K, 10K, 100K, 1M elements
- **Realistic test scenarios**: 75% existing keys, 25% non-existing keys
- **Warmup runs**: Eliminates cold cache effects
- **Correctness verification**: Ensures performance optimizations don't break functionality
- **Worst-case analysis**: Tests scenarios where linear search performs poorly

## Running Tests

### Individual Tests
```bash
# Run functional tests only
make test-functional

# Run performance tests only  
make test-performance
```

### All Tests (PostgreSQL Style)
```bash
# Run complete test suite
make check
```

## Test Results Summary

### Functional Tests
- **Total test cases**: 27+ individual assertions
- **Coverage**: All public functions in libsimd
- **Edge cases**: Empty arrays, single elements, boundary conditions
- **Data types**: uint8_t, uint32_t with various value ranges
- **Correctness**: 100% pass rate expected

### Performance Tests
- **SIMD vs Linear comparison**: Shows 2-4x speedup typically
- **Multiple data sizes**: Validates performance across different workloads
- **System information**: Reports CPU features (AVX2, SSE4.2)
- **Reproducible results**: Fixed random seeds for consistent benchmarks

## Expected Performance Results

Typical performance improvements on x86_64 with AVX2:

| Function | Array Size | Expected Speedup |
|----------|------------|------------------|
| lfind8   | 1K-1M      | 3-5x            |
| lfind32  | 1K-1M      | 2-4x            |

## Test Configuration

### Functional Tests
- Comprehensive edge case coverage
- Cross-validation with linear implementations
- Memory safety verification
- Boundary condition testing

### Performance Tests
- **Iterations per test**: 1000
- **Warmup runs**: 10
- **Key distribution**: 75% hits, 25% misses
- **Array sizes**: 1K, 10K, 100K, 1M elements
- **Timing precision**: Microsecond resolution

## Adding New Tests

To add tests for new functions:

1. Add function prototype to both test files
2. Implement standard linear version for comparison
3. Add functional tests following existing patterns
4. Add performance benchmarks with appropriate test sizes
5. Update this README with new test information

## Dependencies

- GCC with AVX2 and SSE4.2 support
- libsimd.so (built automatically by Makefile)
- Standard C library functions (malloc, printf, etc.)
- POSIX timing functions (gettimeofday)

## Notes

- Tests are designed to be deterministic and reproducible
- Performance results may vary based on CPU architecture and system load
- All tests include correctness verification to ensure SIMD optimizations are bug-free
- Tests follow PostgreSQL coding and testing conventions