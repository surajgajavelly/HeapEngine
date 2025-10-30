/**
 * @file my_allocator.c
 * @author Gajavelly Sai Suraj (saisurajgajavelly@gmail.com)
 * @brief Implementation of a simple custom memory allocator.
 * @version 1.0
 * @date 2025-10-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "my_allocator.h"
#include <stdbool.h>
#include <stddef.h> // For NULL
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static char heap[HEAP_SIZE]; ///< The raw heap memory managed by the allocator.

static BlockHeader *free_list_head = NULL; ///< The head of the free list.

// --- Helper Functions ---

/**
 * @brief Checks if the given pointer is within the heap.
 *
 * @param ptr Pointer to validate.
 * @return int Non-zero if the pointer is within the heap, zero otherwise.
 */
static int is_within_heap(const void *ptr) {
    // Check if the pointer is within the heap.
    const char *cptr = (const char *) ptr;
    return ptr != NULL && cptr >= heap && cptr < (heap + HEAP_SIZE);
}

/**
 * @brief Finds the first free block large enough to hold 'size' bytes.
 *
 * @param size Minimum required size.
 * @param prev_out A pointer to a BlockHeader pointer, which will be updated
 * to a point to the block *before* the found block(or NULL if found block is
 * the head).
 * @return Pointer to a suitable free block, or NULL if none found.
 */
static BlockHeader *find_free_block(size_t size, BlockHeader **prev_out) {
    BlockHeader *current = free_list_head;
    *prev_out = NULL;

    while (current) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        *prev_out = current;
        current = current->next;
    }
    return NULL;
}

/**
 * @brief Splits a free block to fit the requested size and prepare it for use.
 *
 * @param block_to_split Block to split and prepare.
 * @param requested_size Size of the requested block in bytes.
 * @param prev Previous block in the free list.
 */
static void split_and_prepare_block(BlockHeader *block_to_split,
                                    size_t requested_size, BlockHeader *prev) {
    // Minimum data size for a usable block after splitting.
    const size_t min_block_data_size = ALIGNMENT;

    // Minimum total size for a usable block after splitting.
    const size_t min_block_total_size =
        sizeof(BlockHeader) + min_block_data_size;

    size_t original_block_size =
        block_to_split->size; // Get original block size

    // Check if splitting leaves enough space for a new free block.
    if ((original_block_size >= requested_size) &&
        (original_block_size - requested_size >= min_block_total_size)) {
        // Calculate new header position.
        BlockHeader *new_free_block =
            (BlockHeader *) ((char *) (block_to_split + 1) + requested_size);
        // Setup new free block.
        new_free_block->size =
            original_block_size - requested_size - sizeof(BlockHeader);
        new_free_block->is_free = true;
        new_free_block->next = block_to_split->next; // Add to free list chain
        new_free_block->magic = BLOCK_MAGIC;

        // Adjust original block.
        block_to_split->size = requested_size; // Update block size
        block_to_split->is_free = false;
        block_to_split->next = NULL;         // Remove from free list chain
        block_to_split->magic = BLOCK_MAGIC; // Update the magic number

        // Update free list links to insert new_free_block.
        if (prev) {
            prev->next = new_free_block;
        } else {
            free_list_head = new_free_block; // New block becomes head.
        }
    } else {

        // mark entire block as allocated.
        block_to_split->is_free = false;
        block_to_split->magic = BLOCK_MAGIC; // Update the magic number

        // Remove from free list chain.
        if (prev) {
            prev->next = block_to_split->next;
        } else {
            free_list_head = block_to_split->next; // New block becomes head.
        }

        block_to_split->next = NULL; // Remove from free list chain
    }
}

/**
 * @brief Coalesces adjacent free blocks.
 *
 * If the next block in memory is free, merge them into a single larger block.
 *
 * @param block_to_free Block to coalesce.
 * @return BlockHeader* Pointer to the coalesced block.
 */
static BlockHeader *coalesce_block(BlockHeader *block_to_free) {
    // Calculate the address of the expected next physical block's header.
    const BlockHeader *next_block =
        (BlockHeader *) ((char *) (block_to_free + 1) + block_to_free->size);

    // Check if the next block is within the heap bounds.
    if (is_within_heap(next_block)) {
        // Check if the next physical block is valid (magic) and marked as free
        if (next_block->magic == BLOCK_MAGIC && next_block->is_free) {
            // Remove the next_block from the free list.
            BlockHeader *current = free_list_head;
            BlockHeader *prev = NULL;

            while (current != NULL) {
                if (current == next_block) {
                    if (prev != NULL) {
                        prev->next = current->next;
                    } else {
                        free_list_head = current->next;
                    }
                    break;
                }
                prev = current;
                current = current->next;
            }

            // Edge case: ensure head is NULL if next_block was the only item.
            if (free_list_head == next_block) {
                free_list_head = NULL;
            }

            // Merge the blocks.
            block_to_free->size += next_block->size + sizeof(BlockHeader);
        }
    }

    // TODO: Implement backward coalescing.

    // Return the block.
    return block_to_free;
}

// --- Core Allocator Functions ---

/**
 * @brief Initializes/resets the allocator.
 *
 * Sets up the entire heap as a single, large free block.
 */
void allocator_init(void) {
    // Setup free list.
    free_list_head = (BlockHeader *) heap;
    free_list_head->size = HEAP_SIZE - sizeof(BlockHeader);
    free_list_head->is_free = true;
    free_list_head->next = NULL;
    free_list_head->magic = BLOCK_MAGIC;
}

/**
 * @brief Allocates 'size' bytes of uninitialized memory.
 *
 * Aligns the request, finds a suitable free block, splits if needed,
 * and returns a pointer to the allocated memory.
 *
 * @return void* Pointer to the allocated memory, or NULL if the request fails.
 */
void *my_malloc(size_t size) {

    if (size == 0) {
        return NULL;
    }

    size_t total_size = size + ALIGNMENT - 1 + sizeof(size_t);

    // Find a suitable free block.
    BlockHeader *prev = NULL;
    BlockHeader *block = find_free_block(total_size, &prev);
    if (block == NULL) {
        return NULL;
    }

    // Align the data pointer.
    void *raw_data_ptr = (void *) (block + 1);
    uintptr_t raw_addr = (uintptr_t) raw_data_ptr;

    // Calculate the aligned address after space for the offset storage.
    uintptr_t aligned_addr_with_offset =
        (raw_addr + sizeof(size_t) + ALIGNMENT - 1) &
        ~(uintptr_t) (ALIGNMENT - 1);
    void *aligned_data_ptr =
        (void *) aligned_addr_with_offset; // Pointer to return to user.
    void *offset_storage_ptr =
        (void *) (aligned_addr_with_offset -
                  sizeof(size_t)); // Pointer to store offset.

    // Calculate the actual size of the used data.
    size_t actual_used_data_size =
        (size_t) ((char *) aligned_data_ptr + size - (char *) raw_data_ptr);

    // Split the block if necessary.
    split_and_prepare_block(block, actual_used_data_size, prev);

    // Calculate and store the offset.
    size_t offset = (size_t) ((char *) offset_storage_ptr - (char *) block);
    *(size_t *) offset_storage_ptr = offset;

    // Return the aligned data pointer.
    return aligned_data_ptr;
}

/**
 * @brief Frees a block of memory previously allocated by my_malloc.
 *
 * Validates the pointer, marks the block free, coalesces with neighbor,
 * and reinserts into the free list.
 */
void my_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    // Basic boundary and alignment checks on user pointer.
    if (!is_within_heap(ptr) || ((uintptr_t) ptr % ALIGNMENT != 0)) {
        fprintf(stderr, "Error: Attempting to free invalid pointer %p.\n", ptr);
        return;
    }

    // Find offset storage location and check its bounds.
    void *offset_storage_ptr = (void *) ((uintptr_t) ptr - sizeof(size_t));
    if (!is_within_heap(offset_storage_ptr)) {
        fprintf(stderr,
                "Error: Calculated offset storage pointer %p is out of heap "
                "bounds (original ptr: %p).\n",
                offset_storage_ptr, ptr);
        return;
    }

    // Read offset and calculate header address.
    size_t offset = *(size_t *) offset_storage_ptr;
    BlockHeader *block_to_free =
        (BlockHeader *) ((char *) offset_storage_ptr - offset);

    // 5. Validate header (bounds and magic number)
    if (!is_within_heap(block_to_free) || block_to_free->magic != BLOCK_MAGIC) {
        uint32_t current_magic =
            is_within_heap(block_to_free) ? block_to_free->magic : 0;
        fprintf(stderr,
                "Error: Invalid block header detected (addr: %p, magic: %x != "
                "%x) for pointer %p.\n",
                (void *) block_to_free, current_magic, BLOCK_MAGIC, ptr);
        return;
    }

    // Check for double free
    if (block_to_free->is_free) {
        fprintf(stderr,
                "Warning: Double free detected for pointer %p (block @ %p).\n",
                ptr, (void *) block_to_free);
        return;
    }

    // mark the block as free
    block_to_free->is_free = true;

    // Coalesce with neighbors.
    block_to_free = coalesce_block(block_to_free);

    // Add the block to the free list.
    block_to_free->next = free_list_head;
    free_list_head = block_to_free;
}

/**
 * @brief Allocates memory for an array of nmembq elements of size bytes each
 * and initializes all bits to zero.
 *
 * @return void* Pointer to allocated zeroed memory, or NULL on
 * failure/overflow.
 */
void *my_calloc(size_t nmemb, size_t size) {

    // Check for invalid input.
    if (nmemb == 0 || size == 0) {
        return NULL;
    }

    // Check for multiplication overflow before calculating total size.
    size_t total_size;
    if (nmemb > SIZE_MAX / size) {
        return NULL;
    }

    total_size = nmemb * size;

    // Allocate memory using my_malloc.
    void *ptr = my_malloc(total_size);

    // Initialize the allocated memory to zero.
    if (ptr != NULL) {
        memset(ptr, 0, total_size);
    }

    return ptr;
}

/**
 * @brief Changes the size of the memory block pointed to by 'ptr' to 'size'
 * bytes.
 *
 * @return void* Pointer to the resized memory block, or NULL if fails.
 */
void *my_realloc(void *ptr, size_t new_size) {

    // If ptr is NULL, behave like malloc(new_size)
    if (ptr == NULL) {
        return my_malloc(new_size);
    }

    // If new_size is 0, behave like free(ptr) and return NULL
    if (new_size == 0) {
        my_free(ptr);
        return NULL;
    }

    // Find the original block header using the offset
    void *offset_ptr = (void *) ((uintptr_t) ptr - sizeof(size_t));

    // Basic bound check before dereferencing offset_ptr
    if (!is_within_heap(offset_ptr)) {
        fprintf(stderr,
                "Error(realloc): Offset pointer %p out of bounds for user ptr "
                "%p.\n",
                offset_ptr, ptr);
        return NULL;
    }

    size_t offset = *(size_t *) offset_ptr;
    const BlockHeader *old_block_header =
        (BlockHeader *) ((char *) offset_ptr - offset);

    // Validate the retrieved header (crucial before reading size or freeing)
    if (!is_within_heap(old_block_header) ||
        old_block_header->magic != BLOCK_MAGIC || old_block_header->is_free) {
        fprintf(stderr, "Error(realloc): Invalid header found for ptr %p.\n",
                ptr);
        return NULL;
    }

    size_t old_data_size = old_block_header->size; // Usable data size.

    // Handle shrinking or same size: return original pointer
    if (new_size <= old_data_size) {
        return ptr;
    }

    // Allocate new block
    void *new_ptr = my_malloc(new_size);
    if (new_ptr == NULL) {
        return NULL;
    }

    // Copy the data from the old block to the new block.
    memcpy(new_ptr, ptr, old_data_size);

    // Free the original block.
    my_free(ptr);

    // Return the pointer to the new block.
    return new_ptr;
}