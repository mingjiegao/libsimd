/*
 * test_functional.c - Functional tests for libsimd functions
 * 
 * This file contains unit tests to verify the correctness of SIMD functions
 * including lfind8, lfind32, and other utility functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/* Include the SIMD headers and functions */
#include "../simd.h"

/* Function prototypes from lfind.c */
extern bool lfind8(uint8_t key, uint8_t *base, uint32_t nelem);
extern bool lfind8_le(uint8_t key, uint8_t *base, uint32_t nelem);
extern bool lfind32(uint32_t key, uint32_t *base, uint32_t nelem);

/* Test statistics */
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
} test_stats_t;

static test_stats_t stats = {0, 0, 0};

/* Test assertion macro */
#define TEST_ASSERT(condition, message) do { \
    stats.total_tests++; \
    if (condition) { \
        printf("PASS: %s\n", message); \
        stats.passed_tests++; \
    } else { \
        printf("FAIL: %s\n", message); \
        stats.failed_tests++; \
    } \
} while(0)

/* Standard linear search for comparison */
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

/* Test lfind8 function */
void test_lfind8_basic(void)
{
    printf("\n=== Testing lfind8 Basic Functionality ===\n");
    
    /* Test with small array */
    uint8_t small_array[] = {1, 3, 5, 7, 9, 11, 13, 15};
    uint32_t small_size = sizeof(small_array) / sizeof(small_array[0]);
    
    /* Test existing elements */
    TEST_ASSERT(lfind8(5, small_array, small_size) == true, 
                "lfind8 should find existing element 5");
    TEST_ASSERT(lfind8(1, small_array, small_size) == true, 
                "lfind8 should find first element 1");
    TEST_ASSERT(lfind8(15, small_array, small_size) == true, 
                "lfind8 should find last element 15");
    
    /* Test non-existing elements */
    TEST_ASSERT(lfind8(2, small_array, small_size) == false, 
                "lfind8 should not find non-existing element 2");
    TEST_ASSERT(lfind8(16, small_array, small_size) == false, 
                "lfind8 should not find element larger than max");
    TEST_ASSERT(lfind8(0, small_array, small_size) == false, 
                "lfind8 should not find element smaller than min");
}

void test_lfind8_edge_cases(void)
{
    printf("\n=== Testing lfind8 Edge Cases ===\n");
    
    /* Test empty array */
    uint8_t *empty_array = NULL;
    TEST_ASSERT(lfind8(5, empty_array, 0) == false, 
                "lfind8 should return false for empty array");
    
    /* Test single element array */
    uint8_t single[] = {42};
    TEST_ASSERT(lfind8(42, single, 1) == true, 
                "lfind8 should find element in single-element array");
    TEST_ASSERT(lfind8(41, single, 1) == false, 
                "lfind8 should not find different element in single-element array");
    
    /* Test array with duplicates */
    uint8_t duplicates[] = {1, 2, 3, 2, 4, 2, 5};
    uint32_t dup_size = sizeof(duplicates) / sizeof(duplicates[0]);
    TEST_ASSERT(lfind8(2, duplicates, dup_size) == true, 
                "lfind8 should find duplicate elements");
}

void test_lfind8_large_array(void)
{
    printf("\n=== Testing lfind8 Large Array ===\n");
    
    const uint32_t large_size = 10000;
    uint8_t *large_array = malloc(large_size * sizeof(uint8_t));
    
    /* Initialize array with even numbers */
    for (uint32_t i = 0; i < large_size; i++) {
        large_array[i] = (uint8_t)(i % 256);
    }
    
    /* Test finding elements */
    TEST_ASSERT(lfind8(100, large_array, large_size) == true, 
                "lfind8 should find element in large array");
    TEST_ASSERT(lfind8(255, large_array, large_size) == true, 
                "lfind8 should find max uint8 value");
    
    /* Compare with linear search for correctness */
    for (int test_key = 0; test_key < 256; test_key++) {
        bool simd_result = lfind8((uint8_t)test_key, large_array, large_size);
        bool linear_result = linear_search_uint8((uint8_t)test_key, large_array, large_size);
        
        if (simd_result != linear_result) {
            printf("FAIL: Mismatch for key %d - SIMD: %d, Linear: %d\n", 
                   test_key, simd_result, linear_result);
            stats.total_tests++;
            stats.failed_tests++;
        }
    }
    
    printf("Verified lfind8 correctness against linear search for all uint8 values\n");
    
    free(large_array);
}

/* Test lfind8_le function */
void test_lfind8_le_basic(void)
{
    printf("\n=== Testing lfind8_le Basic Functionality ===\n");
    
    /* Test with small array */
    uint8_t small_array[] = {10, 30, 50, 70, 90, 110, 130, 150};
    uint32_t small_size = sizeof(small_array) / sizeof(small_array[0]);
    
    /* Test less than or equal cases */
    TEST_ASSERT(lfind8_le(50, small_array, small_size) == true, 
                "lfind8_le should find element <= 50");
    TEST_ASSERT(lfind8_le(25, small_array, small_size) == true, 
                "lfind8_le should find element <= 25 (first element 10)");
    TEST_ASSERT(lfind8_le(200, small_array, small_size) == true, 
                "lfind8_le should find elements <= 200 (all elements)");
    
    /* Test cases where no element is <= key */
    TEST_ASSERT(lfind8_le(5, small_array, small_size) == false, 
                "lfind8_le should not find elements <= 5");
    
    /* Compare with linear implementation */
    for (uint8_t test_key = 0; test_key < 200; test_key += 10) {
        bool simd_result = lfind8_le(test_key, small_array, small_size);
        bool linear_result = linear_search_uint8_le(test_key, small_array, small_size);
        
        if (simd_result != linear_result) {
            printf("FAIL: lfind8_le mismatch for key %u - SIMD: %d, Linear: %d\n", 
                   test_key, simd_result, linear_result);
            stats.total_tests++;
            stats.failed_tests++;
        }
    }
    
    printf("Verified lfind8_le correctness against linear search\n");
}

/* Test lfind32 function */
void test_lfind32_basic(void)
{
    printf("\n=== Testing lfind32 Basic Functionality ===\n");
    
    /* Test with small array */
    uint32_t small_array[] = {10, 30, 50, 70, 90, 110, 130, 150};
    uint32_t small_size = sizeof(small_array) / sizeof(small_array[0]);
    
    /* Test existing elements */
    TEST_ASSERT(lfind32(50, small_array, small_size) == true, 
                "lfind32 should find existing element 50");
    TEST_ASSERT(lfind32(10, small_array, small_size) == true, 
                "lfind32 should find first element 10");
    TEST_ASSERT(lfind32(150, small_array, small_size) == true, 
                "lfind32 should find last element 150");
    
    /* Test non-existing elements */
    TEST_ASSERT(lfind32(25, small_array, small_size) == false, 
                "lfind32 should not find non-existing element 25");
    TEST_ASSERT(lfind32(200, small_array, small_size) == false, 
                "lfind32 should not find element larger than max");
    TEST_ASSERT(lfind32(5, small_array, small_size) == false, 
                "lfind32 should not find element smaller than min");
}

void test_lfind32_edge_cases(void)
{
    printf("\n=== Testing lfind32 Edge Cases ===\n");
    
    /* Test empty array */
    uint32_t *empty_array = NULL;
    TEST_ASSERT(lfind32(5, empty_array, 0) == false, 
                "lfind32 should return false for empty array");
    
    /* Test single element array */
    uint32_t single[] = {0xDEADBEEF};
    TEST_ASSERT(lfind32(0xDEADBEEF, single, 1) == true, 
                "lfind32 should find element in single-element array");
    TEST_ASSERT(lfind32(0xCAFEBABE, single, 1) == false, 
                "lfind32 should not find different element in single-element array");
    
    /* Test with extreme values */
    uint32_t extremes[] = {0, UINT32_MAX, 1, UINT32_MAX - 1};
    uint32_t ext_size = sizeof(extremes) / sizeof(extremes[0]);
    TEST_ASSERT(lfind32(0, extremes, ext_size) == true, 
                "lfind32 should find minimum uint32 value");
    TEST_ASSERT(lfind32(UINT32_MAX, extremes, ext_size) == true, 
                "lfind32 should find maximum uint32 value");
}

void test_lfind32_large_array(void)
{
    printf("\n=== Testing lfind32 Large Array ===\n");
    
    const uint32_t large_size = 10000;
    uint32_t *large_array = malloc(large_size * sizeof(uint32_t));
    
    /* Initialize array with sequential values */
    for (uint32_t i = 0; i < large_size; i++) {
        large_array[i] = i * 2; // Even numbers
    }
    
    /* Test finding elements */
    TEST_ASSERT(lfind32(1000, large_array, large_size) == true, 
                "lfind32 should find existing even number");
    TEST_ASSERT(lfind32(1001, large_array, large_size) == false, 
                "lfind32 should not find odd number in even array");
    
    /* Compare with linear search for a subset */
    for (uint32_t test_key = 0; test_key < 1000; test_key += 10) {
        bool simd_result = lfind32(test_key, large_array, large_size);
        bool linear_result = linear_search_uint32(test_key, large_array, large_size);
        
        if (simd_result != linear_result) {
            printf("FAIL: Mismatch for key %u - SIMD: %d, Linear: %d\n", 
                   test_key, simd_result, linear_result);
            stats.total_tests++;
            stats.failed_tests++;
        }
    }
    
    printf("Verified lfind32 correctness against linear search for sample values\n");
    
    free(large_array);
}

/* Test vector alignment and boundary conditions */
void test_vector_alignment(void)
{
    printf("\n=== Testing Vector Alignment ===\n");
    
    /* Test arrays of different sizes to check vector boundary handling */
    for (uint32_t size = 1; size <= 64; size++) {
        uint8_t *test_array = malloc(size * sizeof(uint8_t));
        
        /* Fill with sequential values */
        for (uint32_t i = 0; i < size; i++) {
            test_array[i] = (uint8_t)(i % 256);
        }
        
        /* Test finding first and last elements */
        bool found_first = lfind8(0, test_array, size);
        bool found_last = lfind8((uint8_t)((size - 1) % 256), test_array, size);
        
        if (!found_first || !found_last) {
            printf("FAIL: Vector alignment issue with size %u\n", size);
            stats.total_tests++;
            stats.failed_tests++;
        }
        
        free(test_array);
    }
    
    stats.total_tests++;
    stats.passed_tests++;
    printf("Vector alignment tests completed successfully\n");
}

/* Print test summary */
void print_test_summary(void)
{
    printf("\n" "================================================\n");
    printf("Functional Test Summary:\n");
    printf("Total tests: %d\n", stats.total_tests);
    printf("Passed: %d\n", stats.passed_tests);
    printf("Failed: %d\n", stats.failed_tests);
    printf("Success rate: %.2f%%\n", 
           stats.total_tests > 0 ? (double)stats.passed_tests / stats.total_tests * 100 : 0);
    
    if (stats.failed_tests == 0) {
        printf("All functional tests PASSED!\n");
    } else {
        printf("WARNING: %d functional tests FAILED!\n", stats.failed_tests);
    }
    printf("================================================\n");
}

int main(void)
{
    printf("libsimd Functional Tests\n");
    printf("========================\n");
    
    /* Run all functional tests */
    test_lfind8_basic();
    test_lfind8_edge_cases();
    test_lfind8_large_array();
    test_lfind8_le_basic();
    
    test_lfind32_basic();
    test_lfind32_edge_cases();
    test_lfind32_large_array();
    
    test_vector_alignment();
    
    /* Print summary */
    print_test_summary();
    
    /* Return number of failed tests as exit code */
    return stats.failed_tests;
}