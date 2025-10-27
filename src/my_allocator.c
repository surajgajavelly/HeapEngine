/**
 * @file my_allocator.c 
 * @author Gajavelly Sai Suraj (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-10-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "my_allocator.h"
 #include <stddef.h> // For NULL
 #include <stdint.h>
 #include <stdio.h>
 #include <string.h>

 static char heap[HEAP_SIZE];
 static BlockHeader *free_list_head = NULL;

#ifndef ALIGNMENT
#define ALIGNMENT 8
#endif

 // --- Helper Functions ---

 /**
  * @brief Checks if the given pointer is within the heap.
  * 
  * @param ptr 
  * @return int 
  */
 static int is_within_heap(const void *ptr) 
 {
    return ptr != NULL && ((const char *)ptr >= heap) && ((const char *)ptr < heap + HEAP_SIZE);
 }

/**
 * @brief Finds the first free block large enough to hold 'size' bytes.
 * 
 * @param size The minimum data size needed.
 * @param prev_out A pointer to a BlockHeader pointer, which will be updated
 * to a point to the block *before* the found block(or NULL if found block is the head).
 * @return A pointer to the found block's header, or NULL if no suitable block.
 */
static BlockHeader *find_free_block(size_t size, BlockHeader **prev_out)
{
   BlockHeader *current = free_list_head;
   *prev_out = NULL;

   while (current)
   {
      if(current->is_free && current->size >= size)
      {
         return current;
      }
      *prev_out = current;
      current = current->next;
   }
   return NULL;
}

static void split_and_prepare_block(BlockHeader *block_to_split, size_t requested_size, BlockHeader *prev)
{
   const size_t min_block_data_size = ALIGNMENT;
   const size_t min_block_total_size = sizeof(BlockHeader) + min_block_data_size;

   size_t original_block_size = block_to_split->size;

   if ((original_block_size >= requested_size) && (original_block_size - requested_size >= min_block_total_size))
   {
      BlockHeader *new_free_block = (BlockHeader *)((char*)(block_to_split + 1) + requested_size);
      new_free_block->size = original_block_size - requested_size - sizeof(BlockHeader);
      new_free_block->is_free = 1;
      new_free_block->next = block_to_split->next;
      new_free_block->magic = BLOCK_MAGIC;

      block_to_split->size = requested_size;
      block_to_split->is_free = 0;
      block_to_split->next = NULL;
      block_to_split->magic = BLOCK_MAGIC;

      if (prev)
      {
         prev->next = new_free_block;
      }
      else
      {
         free_list_head = new_free_block;
      }
   }
   else
   {
      block_to_split->is_free = 0;
      block_to_split->magic = BLOCK_MAGIC;

      if (prev)
      {
         prev->next = block_to_split->next;
      }
      else
      {
         free_list_head = block_to_split->next;
      }

      block_to_split->next = NULL;
   }

}

static BlockHeader *coalesce_block(BlockHeader *block_to_free)
 {
   BlockHeader *next_block = (BlockHeader *)((char*)(block_to_free + 1) + block_to_free->size);

   if((char*)next_block < (heap + HEAP_SIZE))
   {
      if(next_block->is_free)
      {
         BlockHeader *current = free_list_head;
         BlockHeader *prev = NULL;

         while (current)
         {
            if(current == next_block)
            {
               if(prev)
               {
                  prev->next = current->next;
               }
               else
               {
                  free_list_head = current->next;
               }
               break;
            }
            prev = current;
            current = current->next;
         }

         if(free_list_head == next_block)
         {
            free_list_head = NULL;
         }

         block_to_free->size += next_block->size + sizeof(BlockHeader);
      }
   }
   return block_to_free;
 }


void allocator_dump(void)
{
   printf("--- Heap Dump ---\n");
   BlockHeader *current = (BlockHeader*)heap;
   int block_num = 0;

   while((char*)current < (heap + HEAP_SIZE))
   {
      printf("  Block %d @ %p | Header Size: %zu | Data Size: %zu | Free: %d | Next Free: %p\n",
         block_num++,
         (void*)current,
         sizeof(BlockHeader),
         current->size,
         current->is_free,
         (void*)current->next);

      if (current->size == 0 && current->is_free)
      {
         printf("Warning: Encountered block with size 0, stopping dump.\n");
         break;
      }

      BlockHeader *next = (BlockHeader*)((char*)(current + 1) + current->size);
      if (next == current)
      {
         printf("Error: Block pointer did not advance, aborting.\n");
         break;
      }

      current = next;
   }

   printf("--- End of Heap Dump ---\n");
}
 // --- Core Allocator Functions ---

/**
 * @brief Initializes/resets the allocator.
 * Sets up the entire heap as a single, large free block.
 * 
 */
 void allocator_init(void) 
 {
   free_list_head = (BlockHeader*)heap;
   free_list_head->size = HEAP_SIZE - sizeof(BlockHeader);
   free_list_head->is_free = 1;
   free_list_head->next = NULL;
   free_list_head->magic = BLOCK_MAGIC;
 }

 /**
  * @brief Allocates 'size' bytes of uninitialized memory.
  * 
  * @param size 
  * @return void* 
  */
 void *my_malloc(size_t size) {

   if(size == 0)
   {
      return NULL;
   }
   
   size_t requested_data_size = size;

   size_t total_needed_in_block = requested_data_size + ALIGNMENT - 1 + sizeof(size_t);

   BlockHeader *prev = NULL;
   BlockHeader *block = find_free_block(total_needed_in_block, &prev);

   if(block == NULL)
   {
      return NULL;
   }

   void* raw_data_ptr = (void*)(block + 1);
   uintptr_t raw_addr = (uintptr_t)raw_data_ptr;

   uintptr_t aligned_addr_with_offset = (raw_addr + sizeof(size_t) + ALIGNMENT - 1) & ~(uintptr_t)(ALIGNMENT - 1);
   void* aligned_data_ptr = (void*)aligned_addr_with_offset;
   void* offset_storage_ptr = (void*)(aligned_addr_with_offset - sizeof(size_t));

   size_t actual_used_data_size = (size_t)((char*)aligned_data_ptr + requested_data_size - (char*)raw_data_ptr);

   split_and_prepare_block(block, actual_used_data_size, prev);

   size_t offset = (size_t)((char*)offset_storage_ptr - (char*)block);
   *(size_t*)offset_storage_ptr = offset;

   return aligned_data_ptr;
 }

 /**
  * @brief 
  * 
  * @param ptr 
  */
 void my_free(void *ptr) 
 {
   if(ptr == NULL)
   {
      return;
   }

   if (!is_within_heap(ptr)) {
         fprintf(stderr, "Error: Attempting to free pointer %p outside heap bounds.\n", ptr);
         return;
    }

    if ((uintptr_t)ptr % ALIGNMENT != 0) {
         fprintf(stderr, "Error: Attempting to free unaligned pointer %p.\n", ptr);
         return;
     }

   void* offset_storage_ptr = (void*)((uintptr_t)ptr - sizeof(size_t));

   if (!is_within_heap(offset_storage_ptr)) {
         fprintf(stderr, "Error: Calculated offset storage pointer %p is out of heap bounds (original ptr: %p).\n", offset_storage_ptr, ptr);
         return;
     }

   size_t offset = *(size_t*)offset_storage_ptr;

   BlockHeader *block_to_free = (BlockHeader*)((char*)offset_storage_ptr - offset);

   if (!is_within_heap(block_to_free) || block_to_free->magic != BLOCK_MAGIC) 
   {
         uint32_t current_magic = is_within_heap(block_to_free) ? block_to_free->magic : 0;
         fprintf(stderr, "Error: Invalid block header detected (addr: %p, magic: %x != %x) for pointer %p.\n",
                 (void*)block_to_free, current_magic, BLOCK_MAGIC, ptr);
         return;
    }

   if(block_to_free->is_free)
   {
      fprintf(stderr, "Error: Attempt to free a block that is already free.\n");
      return;
   }

   block_to_free->is_free = 1;
   block_to_free = coalesce_block(block_to_free);
   block_to_free->next = free_list_head;
   free_list_head = block_to_free;
 }

 /**
  * @brief Allocates memory for an array of nmembq elements of size bytes each
  * and initializes all bits to zero.
  * 
  * @param nmemb Number of elements.
  * @param size Size of the each element.
  * @return void* Pointer to allocated zeroed memory, or NULL on failure/overflow.
  */
 void *my_calloc(size_t nmemb, size_t size) {

   if (nmemb == 0 || size == 0)
   {
      return NULL;
   }

   size_t total_size;

   if (size > SIZE_MAX / nmemb)
   {
      return NULL;
   }

   total_size = nmemb * size;

   void *ptr = my_malloc(total_size);

   if (ptr != NULL)
   {
      memset(ptr, 0, total_size);
   }

   return ptr;
 }

 /**
  * @brief Changes the size of the memory block pointed to by 'ptr' to 'size' bytes.
  * 
  * @param ptr Pointer to the memory block to be resize, or NULL.
  * @param size The new size for the memory block, in bytes.
  * @return void* Pointer to the resized memory block, or NULL if fails.
  */
 void *my_realloc(void *ptr, size_t new_size) {

   if (ptr == NULL)
   {
      return my_malloc(new_size);
   }

   if (new_size == 0)
   {
      my_free(ptr);
      return NULL;
   }

   void *offset_storage_ptr = (void*)((uintptr_t)ptr - sizeof(size_t));

   size_t offset = *(size_t*)offset_storage_ptr;

   BlockHeader *old_block_header = (BlockHeader*)((char*)offset_storage_ptr - offset);

   size_t old_data_size = old_block_header->size;

   if (new_size <= old_data_size)
   {
      return ptr;
   }

   void *new_ptr = my_malloc(new_size);

   if (new_ptr == NULL)
   {
      return NULL;
   }

   size_t copy_size = (new_size > old_data_size) ? old_data_size : new_size;

   memcpy(new_ptr, ptr, copy_size);

   my_free(ptr);

   return new_ptr;

 }