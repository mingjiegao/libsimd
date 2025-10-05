# libsimd - High-Performance SIMD Library

A PostgreSQL-style SIMD (Single Instruction, Multiple Data) library that provides optimized implementations of common search and comparison operations using vector instructions.

## Overview

libsimd is designed to accelerate data-intensive operations by leveraging modern CPU SIMD instruction sets (SSE4.2, AVX2) to process multiple data elements simultaneously. This library provides significant performance improvements over traditional scalar implementations for array search operations.

## Library Functions

### lfind8
```c
bool lfind8(uint8 key, uint8 *base, uint32 nelem);
```
**Purpose**: Search for a specific 8-bit value in an array using SIMD optimization.

**Parameters**:
- `key`: The 8-bit value to search for
- `base`: Pointer to the array to search
- `nelem`: Number of elements in the array

**Returns**: `true` if the key is found, `false` otherwise

**Performance**: Processes 16 elements per vector operation (SSE) or 32 elements (AVX2)

**Use Cases**:
- Character searching in text processing
- Byte pattern matching
- Small integer lookups
- Hash table probing

### lfind8_le
```c
bool lfind8_le(uint8 key, uint8 *base, uint32 nelem);
```
**Purpose**: Search for any 8-bit value less than or equal to the specified key.

**Parameters**:
- `key`: The maximum 8-bit value to search for
- `base`: Pointer to the array to search
- `nelem`: Number of elements in the array

**Returns**: `true` if any element ≤ key is found, `false` otherwise

**Performance**: Uses saturating subtraction for efficient comparison

**Use Cases**:
- Range queries
- Threshold detection
- Data validation
- Statistical analysis

### lfind32
```c
bool lfind32(uint32 key, uint32 *base, uint32 nelem);
```
**Purpose**: Search for a specific 32-bit value in an array using advanced SIMD optimization.

**Parameters**:
- `key`: The 32-bit value to search for
- `base`: Pointer to the array to search
- `nelem`: Number of elements in the array

**Returns**: `true` if the key is found, `false` otherwise

**Performance**: Uses 4-register parallel processing for maximum throughput

**Use Cases**:
- Integer array searches
- ID lookups
- Hash value matching
- Index scanning

## Architecture Support

### x86_64 Platform
- **SSE4.2**: 128-bit vector operations
- **AVX2**: 256-bit vector operations for doubled throughput
- **Instruction Sets**: Uses `_mm_*` intrinsics for optimal performance
- **Compiler Support**: GCC 4.9+, Clang 3.4+

### ARM64 Platform (Future)
- **NEON**: 128-bit vector operations
- **SVE**: Scalable Vector Extension support planned
- **Cross-compilation**: Full ARM64 support framework in place

## Performance Characteristics

### Benchmark Results
Based on comprehensive testing with 1000 iterations per test:

| Function | Array Size | SIMD Time | Linear Time | Speedup |
|----------|------------|-----------|-------------|---------|
| lfind8   | 1K         | 0.02ms    | 0.09ms      | 3.7x    |
| lfind8   | 100K       | 0.03ms    | 0.10ms      | 4.2x    |
| lfind8   | 1M         | 0.03ms    | 0.10ms      | 4.0x    |
| lfind32  | 1K         | 0.09ms    | 0.22ms      | 2.5x    |
| lfind32  | 100K       | 6.75ms    | 21.0ms      | 3.1x    |
| lfind32  | 1M         | 104ms     | 214ms       | 2.1x    |


## Building and Installation

### Prerequisites
```bash
# Required compiler features
gcc -mavx2 -msse4.2  # x86_64 with SIMD support
```

### Build Process
```bash
# Build shared library
make all

# Build with tests
make tests

# Run complete test suite
make check

# Install system-wide
make install
```

### Integration
```c
#include "simd.h"

// Example usage
uint8 data[] = {1, 2, 3, 4, 5};
bool found = lfind8(3, data, 5);  // Returns true
```

### Test Execution
```bash
make test-functional   # Run correctness tests
make test-performance  # Run benchmark tests
make check            # Run all tests (PostgreSQL style)
```


### make check result

```
Test executable test/test_performance built successfully
All test executables built successfully
Running all libsimd tests...

=== Functional Tests ===
libsimd Functional Tests
========================

=== Testing lfind8 Basic Functionality ===
PASS: lfind8 should find existing element 5
PASS: lfind8 should find first element 1
PASS: lfind8 should find last element 15
PASS: lfind8 should not find non-existing element 2
PASS: lfind8 should not find element larger than max
PASS: lfind8 should not find element smaller than min

=== Testing lfind8 Edge Cases ===
PASS: lfind8 should return false for empty array
PASS: lfind8 should find element in single-element array
PASS: lfind8 should not find different element in single-element array
PASS: lfind8 should find duplicate elements

=== Testing lfind8 Large Array ===
PASS: lfind8 should find element in large array
PASS: lfind8 should find max uint8 value
Verified lfind8 correctness against linear search for all uint8 values

=== Testing lfind8_le Basic Functionality ===
PASS: lfind8_le should find element <= 50
PASS: lfind8_le should find element <= 25 (first element 10)
PASS: lfind8_le should find elements <= 200 (all elements)
PASS: lfind8_le should not find elements <= 5
Verified lfind8_le correctness against linear search

=== Testing lfind32 Basic Functionality ===
PASS: lfind32 should find existing element 50
PASS: lfind32 should find first element 10
PASS: lfind32 should find last element 150
PASS: lfind32 should not find non-existing element 25
PASS: lfind32 should not find element larger than max
PASS: lfind32 should not find element smaller than min

=== Testing lfind32 Edge Cases ===
PASS: lfind32 should return false for empty array
PASS: lfind32 should find element in single-element array
PASS: lfind32 should not find different element in single-element array
PASS: lfind32 should find minimum uint32 value
PASS: lfind32 should find maximum uint32 value

=== Testing lfind32 Large Array ===
PASS: lfind32 should find existing even number
PASS: lfind32 should not find odd number in even array
Verified lfind32 correctness against linear search for sample values

=== Testing Vector Alignment ===
Vector alignment tests completed successfully

================================================
Functional Test Summary:
Total tests: 30
Passed: 30
Failed: 0
Success rate: 100.00%
All functional tests PASSED!
================================================

=== Performance Tests ===
libsimd Performance Tests
=========================
System Information:
- Compiler: 8.5.0 20210514 (Tencent 8.5.0-26)
- AVX2 support: Enabled
- SSE4.2 support: Enabled

Performance Test Results
========================
Test Configuration:
- Iterations per test: 1000
- Warmup runs: 10
- Key distribution: 75% existing, 25% non-existing

Test Name            |     Size |  SIMD (ms) | Linear (ms) |  Speedup | Status
--------------------------------------------------------------------------------
lfind8_small         |    10000 |       0.03 |       0.10 |     3.92x | PASS
lfind8_medium        |   100000 |       0.03 |       0.10 |     4.16x | PASS
lfind8_large         |  1000000 |       0.03 |       0.11 |     4.08x | PASS
lfind8_xlarge        | 10000000 |       0.03 |       0.10 |     3.96x | PASS
lfind32_small        |    10000 |       0.67 |       2.06 |     3.06x | PASS
lfind32_medium       |   100000 |       6.73 |      20.72 |     3.08x | PASS
lfind32_large        |  1000000 |     102.91 |     209.44 |     2.04x | PASS
lfind32_xlarge       | 10000000 |    1986.14 |    3120.74 |     1.57x | PASS
--------------------------------------------------------------------------------

Performance Summary:
- Total tests: 8
- Passed tests: 8
- Failed tests: 0
- Average speedup: 3.23x
- SIMD implementation is 3.23x faster on average

Notes:
- Speedup values > 1.0 indicate SIMD is faster
- Results may vary based on CPU architecture and compiler optimizations
- All tests include correctness verification


Worst Case Performance Analysis
===============================
Worst case (key not found):
- SIMD time: 0.45 ms
- Linear time: 7.01 ms
- Speedup: 15.68x

Performance testing completed.

All tests completed.
```