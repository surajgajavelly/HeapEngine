/**
 * @file my_allocator.h
 * @author Gajavelly Sai Suraj (saisurajgajavelly@gmail.com)
 * @brief Custom memory allocator interface.
 *
 * A simple heap allocator providing malloc, calloc, realloc, and free
 * equivalents, along with debugging utilities.
 *
 * @version 1.0
 * @date 2025-10-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H

#include <stdbool.h> // For bool
#include <stddef.h>  // For size_t
#include <stdint.h>  // For uint32_t, uintptr_t

// --- Congfiguration Constants ---

#define HEAP_SIZE (1024 * 10) ///< Total size of the heap in bytes.
#define ALIGNMENT 8           ///< Alignment for memory blocks.
#define BLOCK_MAGIC 0xC0FFEE  ///< Magic number for block validation.

// --- Data Structures ---

/**
 * @brief Metadata header for each memory block.
 *
 * Each allocated or free block in the heap begins with this header.
 * - size: number of usable bytes in the block (not including header).
 * - is_free: true if the block is currently free.
 * - next: pointer to the next free block in the free list.
 * - magic: sentinel value for corruption detection.
 */
typedef struct BlockHeader {
    size_t size;              ///< Size of the data area in bytes
    bool is_free;             ///< Whether this block is free
    struct BlockHeader *next; ///< Next block in the free list
    uint32_t magic;           ///< Magic number for validation
} BlockHeader;

// --- Function Prototypes ---

/**
 * @brief Initializes the memory allocator.
 * Must be called once before any other allocator functions are used.
 * Sets up the initial free block covering the entire heap.
 */
void allocator_init(void);

/**
 * @brief Allocates 'size' bytes of uninitialized memory.
 *
 * @param size The number of bytes to allocate.
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
 * @brief Changes the size of the memory block pointed to by 'ptr' to 'size'
 * bytes.
 *
 * @param ptr pointer to the memory block to be resized.
 * @param new_size The new size for the memory block.
 * @return A pointer to the resized memory block, or NULL if the request fails.
 *
 */
void *my_realloc(void *ptr, size_t new_size);

#endif // MY_ALLOCATOR_H