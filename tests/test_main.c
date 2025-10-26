/**
 * @file test_main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-10-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "unity.h"
#include "my_allocator.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#define ALIGNMENT 8

void setUp(void) 
{
    allocator_init();
}

void tearDown(void) {
    
}

void test_malloc_basic_should_return_non_null(void) {
    void *ptr = my_malloc(10);
    TEST_ASSERT_NOT_NULL(ptr);
}

void test_free_should_reuse_memory(void)
{
    void *ptr1 = my_malloc(10);
    TEST_ASSERT_NOT_NULL(ptr1);

    my_free(ptr1);

    void *ptr2 = my_malloc(10);
    TEST_ASSERT_NOT_NULL(ptr2);

    TEST_ASSERT_EQUAL_PTR(ptr1, ptr2);
}

/**
 * @brief 
 * 
 * @return int 
 */
void test_malloc_should_split_large_block(void)
{
    void *ptr1 = my_malloc(100);
    TEST_ASSERT_NOT_NULL(ptr1);

    void *ptr2 = my_malloc(500);
    TEST_ASSERT_NOT_NULL(ptr2);
}

void test_free_should_coalesce_adjacent_blocks(void)
{
    void *ptr_A = my_malloc(50);
    TEST_ASSERT_NOT_NULL(ptr_A);

    void *ptr_B = my_malloc(60);
    TEST_ASSERT_NOT_NULL(ptr_B);

    void *ptr_C = my_malloc(70);
    TEST_ASSERT_NOT_NULL(ptr_C);

    my_free(ptr_B);
    my_free(ptr_A);

    size_t size_D = 100;
    void *ptr_D = my_malloc(size_D);

    TEST_ASSERT_NOT_NULL(ptr_D);

    TEST_ASSERT_EQUAL_PTR(ptr_A, ptr_D);
}

void test_malloc_should_return_aligned_memory(void)
{
    const int num_allocs = 5;
    void *pointers[num_allocs];

    for(int i = 0; i < num_allocs; i++)
    {
        pointers[i] = my_malloc(i * 10 + 1);
        TEST_ASSERT_NOT_NULL(pointers[i]);

        uintptr_t address = (uintptr_t)pointers[i];
        TEST_ASSERT_EQUAL_INT(0, address % ALIGNMENT);
    }

    for(int i = 0; i < num_allocs; i++)
    {
        if(pointers[i] != NULL)
        {
            my_free(pointers[i]);
        }  
    }
}

void test_calloc_should_return_zeroed_memory(void)
{
    size_t num_elements = 15;
    size_t element_size = sizeof(int);
    size_t total_size = num_elements * element_size;

    if (num_elements > 0 && element_size > SIZE_MAX / num_elements)
    {
        TEST_FAIL_MESSAGE("Test setup error: Size calculation would overflow.");
        return;
    }

    int *ptr = (int*)my_calloc(num_elements, element_size);

    TEST_ASSERT_NOT_NULL(ptr);

    unsigned char zero_buffer[total_size];
    memset(zero_buffer, 0, total_size);
    TEST_ASSERT_EQUAL_INT(0, memcmp(ptr, zero_buffer, total_size));

    my_free(ptr);
}

void test_calloc_should_fail_on_overflow(void)
{
    size_t large_num = SIZE_MAX / 2 + 2;
    size_t size = 2;

    void *ptr = my_calloc(large_num, size);

    TEST_ASSERT_NULL(ptr);
}
int main(void) {
    UNITY_BEGIN(); // Sets up Unity

    // Run our tests
    RUN_TEST(test_malloc_basic_should_return_non_null);
    RUN_TEST(test_free_should_reuse_memory);
    RUN_TEST(test_malloc_should_split_large_block);
    RUN_TEST(test_free_should_coalesce_adjacent_blocks);
    RUN_TEST(test_malloc_should_return_aligned_memory);
    RUN_TEST(test_calloc_should_return_zeroed_memory);
    RUN_TEST(test_calloc_should_fail_on_overflow);

    return UNITY_END(); // Reports the results
}