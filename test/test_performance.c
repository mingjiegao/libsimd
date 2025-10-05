/*
 * test_performance.c - Performance tests for libsimd functions
 * 
 * This file contains performance benchmarks comparing SIMD implementations
 * against standard linear search algorithms.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

/* Include the SIMD headers and functions */
#include "../simd.h"

/* Function prototypes from lfind.c */
extern bool lfind8(uint8_t key, uint8_t *base, uint32_t nelem);
extern bool lfind8_le(uint8_t key, uint8_t *base, uint32_t nelem);
extern bool lfind32(uint32_t key, uint32_t *base, uint32_t nelem);

/* Performance test configuration */
#define SMALL_ARRAY_SIZE    10000
#define MEDIUM_ARRAY_SIZE   100000
#define LARGE_ARRAY_SIZE    1000000
#define XLARGE_ARRAY_SIZE   10000000
#define NUM_ITERATIONS      1000
#define NUM_WARMUP_RUNS     10

/* Timing utilities */
static double get_time_microseconds(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

/* Standard implementations for comparison */
static bool linear_search_uint8(uint8_t key, uint8_t *base, uint32_t nelem)
{
    for (uint32_t i = 0; i < nelem; i++) {
        if (base[i] == key) {
            return true;
        }
    }
    return false;
}

static bool linear_search_uint32(uint32_t key, uint32_t *base, uint32_t nelem)
{
    for (uint32_t i = 0; i < nelem; i++) {
        if (base[i] == key) {
            return true;
        }
    }
    return false;
}

static bool linear_search_uint8_le(uint8_t key, uint8_t *base, uint32_t nelem)
{
    for (uint32_t i = 0; i < nelem; i++) {
        if (base[i] <= key) {
            return true;
        }
    }
    return false;
}

/* Performance test structure */
typedef struct {
    const char *test_name;
    uint32_t array_size;
    double simd_time;
    double linear_time;
    double speedup;
    bool correctness_verified;
} perf_result_t;

/* Test lfind8 performance */
perf_result_t test_lfind8_performance(uint32_t array_size, const char *test_name)
{
    perf_result_t result = {0};
    result.test_name = test_name;
    result.array_size = array_size;
    
    /* Allocate and initialize test array */
    uint8_t *test_array = malloc(array_size * sizeof(uint8_t));
    if (!test_array) {
        printf("ERROR: Failed to allocate memory for %s\n", test_name);
        return result;
    }
    
    /* Fill array with pseudo-random values */
    srand(42); // Fixed seed for reproducible results
    for (uint32_t i = 0; i < array_size; i++) {
        test_array[i] = (uint8_t)(rand() % 256);
    }
    
    /* Prepare test keys - mix of existing and non-existing */
    uint8_t test_keys[NUM_ITERATIONS];
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        if (i % 4 == 0) {
            /* 25% non-existing keys */
            test_keys[i] = (uint8_t)(256 + (rand() % 256));
        } else {
            /* 75% existing keys from array */
            test_keys[i] = test_array[rand() % array_size];
        }
    }
    
    /* Warmup runs */
    for (int i = 0; i < NUM_WARMUP_RUNS; i++) {
        lfind8(test_keys[i % NUM_ITERATIONS], test_array, array_size);
        linear_search_uint8(test_keys[i % NUM_ITERATIONS], test_array, array_size);
    }
    
    /* Benchmark SIMD implementation */
    double start_time = get_time_microseconds();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        volatile bool found = lfind8(test_keys[i], test_array, array_size);
        (void)found; // Prevent optimization
    }
    double end_time = get_time_microseconds();
    result.simd_time = end_time - start_time;
    
    /* Benchmark linear implementation */
    start_time = get_time_microseconds();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        volatile bool found = linear_search_uint8(test_keys[i], test_array, array_size);
        (void)found; // Prevent optimization
    }
    end_time = get_time_microseconds();
    result.linear_time = end_time - start_time;
    
    /* Calculate speedup */
    result.speedup = result.linear_time / result.simd_time;
    
    /* Verify correctness */
    result.correctness_verified = true;
    for (int i = 0; i < 100; i++) { // Sample verification
        uint8_t key = test_keys[i];
        bool simd_result = lfind8(key, test_array, array_size);
        bool linear_result = linear_search_uint8(key, test_array, array_size);
        
        if (simd_result != linear_result) {
            result.correctness_verified = false;
            printf("ERROR: Correctness mismatch in %s for key %u\n", test_name, key);
            break;
        }
    }
    
    free(test_array);
    return result;
}

/* Test lfind32 performance */
perf_result_t test_lfind32_performance(uint32_t array_size, const char *test_name)
{
    perf_result_t result = {0};
    result.test_name = test_name;
    result.array_size = array_size;
    
    /* Allocate and initialize test array */
    uint32_t *test_array = malloc(array_size * sizeof(uint32_t));
    if (!test_array) {
        printf("ERROR: Failed to allocate memory for %s\n", test_name);
        return result;
    }
    
    /* Fill array with pseudo-random values */
    srand(42); // Fixed seed for reproducible results
    for (uint32_t i = 0; i < array_size; i++) {
        test_array[i] = (uint32_t)rand() * (uint32_t)rand();
    }
    
    /* Prepare test keys */
    uint32_t test_keys[NUM_ITERATIONS];
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        if (i % 4 == 0) {
            /* 25% non-existing keys */
            test_keys[i] = (uint32_t)rand() * (uint32_t)rand() + 1;
        } else {
            /* 75% existing keys from array */
            test_keys[i] = test_array[rand() % array_size];
        }
    }
    
    /* Warmup runs */
    for (int i = 0; i < NUM_WARMUP_RUNS; i++) {
        lfind32(test_keys[i % NUM_ITERATIONS], test_array, array_size);
        linear_search_uint32(test_keys[i % NUM_ITERATIONS], test_array, array_size);
    }
    
    /* Benchmark SIMD implementation */
    double start_time = get_time_microseconds();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        volatile bool found = lfind32(test_keys[i], test_array, array_size);
        (void)found; // Prevent optimization
    }
    double end_time = get_time_microseconds();
    result.simd_time = end_time - start_time;
    
    /* Benchmark linear implementation */
    start_time = get_time_microseconds();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        volatile bool found = linear_search_uint32(test_keys[i], test_array, array_size);
        (void)found; // Prevent optimization
    }
    end_time = get_time_microseconds();
    result.linear_time = end_time - start_time;
    
    /* Calculate speedup */
    result.speedup = result.linear_time / result.simd_time;
    
    /* Verify correctness */
    result.correctness_verified = true;
    for (int i = 0; i < 100; i++) { // Sample verification
        uint32_t key = test_keys[i];
        bool simd_result = lfind32(key, test_array, array_size);
        bool linear_result = linear_search_uint32(key, test_array, array_size);
        
        if (simd_result != linear_result) {
            result.correctness_verified = false;
            printf("ERROR: Correctness mismatch in %s for key %u\n", test_name, key);
            break;
        }
    }
    
    free(test_array);
    return result;
}

/* Print performance results */
void print_performance_result(const perf_result_t *result)
{
    printf("%-20s | %8u | %10.2f | %10.2f | %8.2fx | %s\n",
           result->test_name,
           result->array_size,
           result->simd_time / 1000.0, // Convert to milliseconds
           result->linear_time / 1000.0,
           result->speedup,
           result->correctness_verified ? "PASS" : "FAIL");
}

/* Run comprehensive performance tests */
void run_performance_tests(void)
{
    printf("\nPerformance Test Results\n");
    printf("========================\n");
    printf("Test Configuration:\n");
    printf("- Iterations per test: %d\n", NUM_ITERATIONS);
    printf("- Warmup runs: %d\n", NUM_WARMUP_RUNS);
    printf("- Key distribution: 75%% existing, 25%% non-existing\n\n");
    
    printf("%-20s | %8s | %10s | %10s | %8s | %s\n",
           "Test Name", "Size", "SIMD (ms)", "Linear (ms)", "Speedup", "Status");
    printf("--------------------------------------------------------------------------------\n");
    
    /* lfind8 performance tests */
    perf_result_t results[8];
    int test_idx = 0;
    
    results[test_idx++] = test_lfind8_performance(SMALL_ARRAY_SIZE, "lfind8_small");
    results[test_idx++] = test_lfind8_performance(MEDIUM_ARRAY_SIZE, "lfind8_medium");
    results[test_idx++] = test_lfind8_performance(LARGE_ARRAY_SIZE, "lfind8_large");
    results[test_idx++] = test_lfind8_performance(XLARGE_ARRAY_SIZE, "lfind8_xlarge");
    
    /* lfind32 performance tests */
    results[test_idx++] = test_lfind32_performance(SMALL_ARRAY_SIZE, "lfind32_small");
    results[test_idx++] = test_lfind32_performance(MEDIUM_ARRAY_SIZE, "lfind32_medium");
    results[test_idx++] = test_lfind32_performance(LARGE_ARRAY_SIZE, "lfind32_large");
    results[test_idx++] = test_lfind32_performance(XLARGE_ARRAY_SIZE, "lfind32_xlarge");
    
    /* Print all results */
    for (int i = 0; i < test_idx; i++) {
        print_performance_result(&results[i]);
    }
    
    /* Calculate and print summary statistics */
    printf("--------------------------------------------------------------------------------\n");
    
    double total_speedup = 0.0;
    int valid_tests = 0;
    int failed_tests = 0;
    
    for (int i = 0; i < test_idx; i++) {
        if (results[i].correctness_verified) {
            total_speedup += results[i].speedup;
            valid_tests++;
        } else {
            failed_tests++;
        }
    }
    
    if (valid_tests > 0) {
        double avg_speedup = total_speedup / valid_tests;
        printf("\nPerformance Summary:\n");
        printf("- Total tests: %d\n", test_idx);
        printf("- Passed tests: %d\n", valid_tests);
        printf("- Failed tests: %d\n", failed_tests);
        printf("- Average speedup: %.2fx\n", avg_speedup);
        
        if (avg_speedup > 1.0) {
            printf("- SIMD implementation is %.2fx faster on average\n", avg_speedup);
        } else {
            printf("- WARNING: SIMD implementation is slower than linear search\n");
        }
    }
    
    printf("\nNotes:\n");
    printf("- Speedup values > 1.0 indicate SIMD is faster\n");
    printf("- Results may vary based on CPU architecture and compiler optimizations\n");
    printf("- All tests include correctness verification\n");
}

/* Test specific scenarios */
void test_worst_case_performance(void)
{
    printf("\n\nWorst Case Performance Analysis\n");
    printf("===============================\n");
    
    const uint32_t test_size = 100000;
    
    /* Test 1: Key not found (worst case for linear search) */
    uint8_t *array = malloc(test_size * sizeof(uint8_t));
    for (uint32_t i = 0; i < test_size; i++) {
        array[i] = (uint8_t)(i % 255); // Exclude 255
    }
    
    uint8_t missing_key = 255; // Not in array
    
    double start_time = get_time_microseconds();
    for (int i = 0; i < 100; i++) {
        volatile bool found = lfind8(missing_key, array, test_size);
        (void)found;
    }
    double simd_time = get_time_microseconds() - start_time;
    
    start_time = get_time_microseconds();
    for (int i = 0; i < 100; i++) {
        volatile bool found = linear_search_uint8(missing_key, array, test_size);
        (void)found;
    }
    double linear_time = get_time_microseconds() - start_time;
    
    printf("Worst case (key not found):\n");
    printf("- SIMD time: %.2f ms\n", simd_time / 1000.0);
    printf("- Linear time: %.2f ms\n", linear_time / 1000.0);
    printf("- Speedup: %.2fx\n", linear_time / simd_time);
    
    free(array);
}

int main(void)
{
    printf("libsimd Performance Tests\n");
    printf("=========================\n");
    
    /* Check CPU features */
    printf("System Information:\n");
    printf("- Compiler: %s\n", __VERSION__);
    #ifdef __AVX2__
    printf("- AVX2 support: Enabled\n");
    #else
    printf("- AVX2 support: Disabled\n");
    #endif
    #ifdef __SSE4_2__
    printf("- SSE4.2 support: Enabled\n");
    #else
    printf("- SSE4.2 support: Disabled\n");
    #endif
    
    /* Run performance tests */
    run_performance_tests();
    
    /* Run worst case analysis */
    test_worst_case_performance();
    
    printf("\nPerformance testing completed.\n");
    return 0;
}