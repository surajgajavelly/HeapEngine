/**
 * @file my_allocator.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-10-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #ifndef MY_ALLOCATOR_H
 #define MY_ALLOCATOR_H

 #include <stddef.h> //For size_t
 #include <stdint.h> //For uint32_t

 #define HEAP_SIZE (1024 * 10)
 #define ALIGNMENT 8
 #define BLOCK_MAGIC 0xC0FFEE

 /**
  * @brief Metadata header for each memory block.
  * 
  */
 typedef struct BlockHeader
 {
    size_t size;
    int is_free;
    struct BlockHeader *next;
    uint32_t magic;
 } BlockHeader;

 /**
  * @brief Initializes the memory allocator. 
  * 
  */
 void allocator_init(void); 
 
 /**
  * @brief Allocates 'size' bytes of uninitialized memory.
  * 
  * @param size The number of bytes to allocate
  * @return A pointer to the allocated memory, or NULL if the request fails.
  * 
  */
 void *my_malloc(size_t size);

 /**
  * @brief Frees a block of memory previously allocated by my_malloc.
  * 
  * @param ptr A pointer to the memory block to be freed.
  */
 void my_free(void *ptr);

 /**
  * @brief Allocates memory for an array of 'nmemb' elements of 'size' bytes 
  * each and initializes all bits to zero.
  * 
  * @param nmemb Number of elements.
  * @param size Size of each element.
  * @return A pointer to the allocated memory, or NULL if the request fails.
  */
 void *my_calloc(size_t nmemb, size_t size);

 /**
  * @brief Changes the size of the memory block pointed to by 'ptr' to 'size' bytes.
  * 
  * @param ptr pointer to the memory block to be resized.
  * @param size The new size for the memory block.
  * @return A pointer to the resized memory block, or NULL if the request fails.
  * 
  */
 void *my_realloc(void *ptr, size_t size);

 /**
  * @brief Dumps the memory allocator's internal state to the console.
  * 
  */
 void allocator_dump(void);

 #endif // MY_ALLOCATOR_H