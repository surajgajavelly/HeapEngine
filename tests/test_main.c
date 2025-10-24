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
int main(void) {
    UNITY_BEGIN(); // Sets up Unity

    // Run our tests
    RUN_TEST(test_malloc_basic_should_return_non_null);
    RUN_TEST(test_free_should_reuse_memory);
    RUN_TEST(test_malloc_should_split_large_block);
    RUN_TEST(test_free_should_coalesce_adjacent_blocks);

    return UNITY_END(); // Reports the results
}