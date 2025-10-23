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

int main(void) {
    UNITY_BEGIN(); // Sets up Unity

    // Run our tests
    RUN_TEST(test_malloc_basic_should_return_non_null);
    RUN_TEST(test_free_should_reuse_memory);
    RUN_TEST(test_malloc_should_split_large_block);

    return UNITY_END(); // Reports the results
}