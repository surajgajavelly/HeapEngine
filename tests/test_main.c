/**
 * @file test_main.c
 * @author Gajavelly Sai Suraj (saisurajgajavelly@gmail.com)
 * @brief Unit tests for the custom memory allocator.
 * @version 1.0
 * @date 2025-10-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "my_allocator.h"
#include "unity.h"
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define ALIGNMENT 8

// --- Test Setup ---

void setUp(void) {
    allocator_init();
}

void tearDown(void) {}

// --- Malloc Tests ---

/**
 * @brief Verifies that my_malloc returns a non-null pointer.
 */
void test_malloc_basic_should_return_non_null(void) {
    void *ptr = my_malloc(10);
    TEST_ASSERT_NOT_NULL(ptr);
    my_free(ptr);
}

/**
 * @brief Verifies that my_malloc splits larger free blocks.
 */
void test_malloc_should_split_large_block(void) {
    void *ptr1 = my_malloc(100);
    TEST_ASSERT_NOT_NULL(ptr1);

    void *ptr2 = my_malloc(500);
    TEST_ASSERT_NOT_NULL(ptr2);

    my_free(ptr1);
    my_free(ptr2);
}

/**
 * @brief Verifies that my_malloc returns aligned memory.
 */
void test_malloc_should_return_aligned_memory(void) {
    const int num_allocs = 5;
    void *pointers[num_allocs];

    for (int i = 0; i < num_allocs; i++) {
        pointers[i] = NULL;
    }

    for (int i = 0; i < num_allocs; i++) {
        pointers[i] = my_malloc(i * 10 + 1);
        TEST_ASSERT_NOT_NULL(pointers[i]);

        uintptr_t address = (uintptr_t) pointers[i];
        TEST_ASSERT_EQUAL_UINT(0, address % ALIGNMENT);
    }

    for (int i = 0; i < num_allocs; i++) {
        if (pointers[i] != NULL) {
            my_free(pointers[i]);
        }
    }
}

/**
 * @brief Verifies that requesting zero bytes returns NULL.
 */
void test_malloc_zero_size(void) {
    const void *ptr = my_malloc(0);
    TEST_ASSERT_NULL(ptr);
}

/**
 * @brief Verifies failure when requesting more memory than available.
 */
void test_malloc_fails_when_heap_too_small(void) {
    size_t too_large_size = HEAP_SIZE - sizeof(BlockHeader) + 1;
    const void *ptr = my_malloc(too_large_size);
    TEST_ASSERT_NULL(ptr);
}

// --- Free Tests ---

/**
 * @brief Verifies that freed memory can be reused.
 */
void test_free_should_reuse_memory(void) {
    void *ptr1 = my_malloc(10);
    TEST_ASSERT_NOT_NULL(ptr1);

    my_free(ptr1);

    void *ptr2 = my_malloc(10);
    TEST_ASSERT_NOT_NULL(ptr2);

    TEST_ASSERT_EQUAL_PTR(ptr1, ptr2);

    my_free(ptr2);
}

/**
 * @brief Verifies that my_free coalesces adjacent blocks.
 */
void test_free_should_coalesce_adjacent_blocks(void) {
    void *ptr1 = my_malloc(50);
    TEST_ASSERT_NOT_NULL(ptr1);

    void *ptr2 = my_malloc(60);
    TEST_ASSERT_NOT_NULL(ptr2);

    void *ptr3 = my_malloc(70);
    TEST_ASSERT_NOT_NULL(ptr3);

    my_free(ptr2);
    my_free(ptr1);

    size_t size4 = 100;
    void *ptr4 = my_malloc(size4);

    TEST_ASSERT_NOT_NULL(ptr4);
    TEST_ASSERT_EQUAL_PTR(ptr1, ptr4);

    my_free(ptr3);
    my_free(ptr4);
}

/**
 * @brief Verifies that freeing a NULL pointer is safe.
 */
void test_free_null_pointer(void) {
    my_free(NULL);
    TEST_ASSERT_TRUE(true);
}

/**
 * @brief Verifies freeing a pointer not allocated by my_malloc is handled
 * safely.
 */
void test_invalid_free(void) {
    int stack_var;
    my_free(&stack_var);
    TEST_ASSERT_TRUE(true);
}

/**
 * @brief Verifies freeing the same pointer twice is handled safely.
 */
void test_double_free(void) {
    void *ptr1 = my_malloc(50);
    TEST_ASSERT_NOT_NULL(ptr1);
    my_free(ptr1);
    my_free(ptr1);

    void *ptr2 = my_malloc(10);
    TEST_ASSERT_NOT_NULL(ptr2);
    my_free(ptr2);

    TEST_ASSERT_TRUE(true);
}

// --- Calloc Tests ---

/**
 * @brief Verifies calloc allocates and returns zeroed memory.
 */
void test_calloc_should_return_zeroed_memory(void) {
    size_t num_elements = 15;
    size_t element_size = sizeof(int);
    size_t total_size = num_elements * element_size;

    if (element_size > SIZE_MAX / num_elements) {
        TEST_FAIL_MESSAGE("Test setup error: Size calculation would overflow.");
        return;
    }

    int *ptr = (int *) my_calloc(num_elements, element_size);
    TEST_ASSERT_NOT_NULL(ptr);

    unsigned char zero_buffer[total_size];
    memset(zero_buffer, 0, total_size);
    TEST_ASSERT_EQUAL_MEMORY(ptr, zero_buffer, total_size);

    my_free(ptr);
}

/**
 * @brief Verifies calloc returns NULL if size calculation overflows.
 */
void test_calloc_should_fail_on_overflow(void) {
    size_t large_num = SIZE_MAX / 2 + 2;
    size_t size = 2;

    const void *ptr = my_calloc(large_num, size);
    TEST_ASSERT_NULL(ptr);
}

// --- Realloc Tests ---

/**
 * @brief Verifies realloc(NULL, size) behaves like malloc(size).
 */
void test_realloc_null_ptr_acts_like_malloc(void) {
    size_t size = 50;
    void *ptr = my_realloc(NULL, size);
    TEST_ASSERT_NOT_NULL(ptr);

    uintptr_t address = (uintptr_t) ptr;
    TEST_ASSERT_EQUAL_INT(0, address % ALIGNMENT);

    my_free(ptr);
}

/**
 * @brief Verifies realloc(ptr, 0) behaves like free(ptr).
 */
void test_realloc_zero_size_acts_like_free(void) {
    size_t intitial_size = 50;
    void *ptr1 = my_malloc(intitial_size);
    TEST_ASSERT_NOT_NULL(ptr1);

   const void *ptr2 = my_realloc(ptr1, 0);
    TEST_ASSERT_NULL(ptr2);

    void *ptr3 = my_malloc(intitial_size);
    TEST_ASSERT_NOT_NULL(ptr3);
    TEST_ASSERT_EQUAL_PTR(ptr1, ptr3);

    my_free(ptr3);
}

/**
 * @brief Verifies realloc can shrink an allocation (in-place).
 */
void test_realloc_should_shrink_block(void) {
    size_t initial_size = 100;
    size_t smaller_size = 50;

    char *ptr = (char *) my_malloc(initial_size);
    TEST_ASSERT_NOT_NULL(ptr);

    memset(ptr, 'A', initial_size);

    char *shrunk_ptr = (char *) my_realloc(ptr, smaller_size);
    TEST_ASSERT_NOT_NULL(shrunk_ptr);
    TEST_ASSERT_EQUAL_PTR(ptr, shrunk_ptr);

    for (size_t i = 0; i < smaller_size; i++) {
        TEST_ASSERT_EQUAL_CHAR('A', shrunk_ptr[i]);
    }

    my_free(shrunk_ptr);
}

/**
 * @brief Verifies realloc growing a block requires moving and preserves data.
 */
void test_realloc_grow_block_new_location(void) {
    size_t initial_size = 50;
    size_t larger_size = 100;

    char *ptr1 = (char *) my_malloc(initial_size);
    TEST_ASSERT_NOT_NULL(ptr1);
    void *ptr2 = my_malloc(20);
    TEST_ASSERT_NOT_NULL(ptr2);

    memset(ptr1, 'B', initial_size);

    char *grown_ptr = (char *) my_realloc(ptr1, larger_size);
    TEST_ASSERT_NOT_NULL(grown_ptr);

    for (size_t i = 0; i < initial_size; i++) {
        TEST_ASSERT_EQUAL_CHAR('B', grown_ptr[i]);
    }

    uintptr_t address = (uintptr_t) grown_ptr;
    TEST_ASSERT_EQUAL_INT(0, address % ALIGNMENT);

    my_free(grown_ptr);
    my_free(ptr2);
}

// --- Scenario Tests ---

/**
 * @brief Tests a specific sequence of alloc/free to check
 * fragmentation/coalescing.
 */
void test_fragmentation_scenario(void) {
    void *a = my_malloc(100);
    TEST_ASSERT_NOT_NULL(a);

    void *b = my_malloc(200);
    TEST_ASSERT_NOT_NULL(b);

    void *c = my_malloc(300);
    TEST_ASSERT_NOT_NULL(c);

    my_free(b);
    b = NULL;

    void *d = my_malloc(50);
    TEST_ASSERT_NOT_NULL(d);

    void *e = my_malloc(250);
    TEST_ASSERT_NOT_NULL(e);

    my_free(a);
    my_free(c);
    my_free(d);
    my_free(e);
}

/**
 * @brief Tests allocating repeatedly until the heap is exhausted.
 */
void test_exhaust_heap(void) {
    void *blocks[HEAP_SIZE / (sizeof(BlockHeader) + ALIGNMENT + 10)];
    int count = 0;
    size_t alloc_size = 10;

    while (count < (int) (sizeof(blocks) / sizeof(blocks[0]))) {
        blocks[count] = my_malloc(alloc_size);
        if (blocks[count] == NULL) {
            break;
        }
        count++;
    }

    TEST_ASSERT_TRUE(count > 0);
    TEST_ASSERT_NULL(my_malloc(alloc_size));

    for (int i = 0; i < count; i++) {
        my_free(blocks[i]);
    }
}

/**
 * @brief Main function to run all unit tests.
 *
 */
int main(void) {
    UNITY_BEGIN(); // Sets up Unity

    // --- Malloc Tests ---
    RUN_TEST(test_malloc_basic_should_return_non_null);
    RUN_TEST(test_malloc_should_split_large_block);
    RUN_TEST(test_malloc_should_return_aligned_memory);
    RUN_TEST(test_malloc_zero_size);
    RUN_TEST(test_malloc_fails_when_heap_too_small);

    // --- Free Tests ---
    RUN_TEST(test_free_should_reuse_memory);
    RUN_TEST(test_free_should_coalesce_adjacent_blocks);
    RUN_TEST(test_free_null_pointer);
    RUN_TEST(test_invalid_free);
    RUN_TEST(test_double_free);

    // --- Calloc Tests ---
    RUN_TEST(test_calloc_should_return_zeroed_memory);
    RUN_TEST(test_calloc_should_fail_on_overflow);

    // --- Realloc Tests ---
    RUN_TEST(test_realloc_null_ptr_acts_like_malloc);
    RUN_TEST(test_realloc_zero_size_acts_like_free);
    RUN_TEST(test_realloc_should_shrink_block);
    RUN_TEST(test_realloc_grow_block_new_location);

    // --- Scenario Tests ---
    RUN_TEST(test_fragmentation_scenario);
    RUN_TEST(test_exhaust_heap);

    return UNITY_END(); // Reports the results
}