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

 #define HEAP_SIZE (1024 * 10)

 static char heap[HEAP_SIZE];

 static BlockHeader *free_list_head = NULL;

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
   
   BlockHeader *current = free_list_head;
   BlockHeader *prev = NULL;

   while(current)
   {
      if(current->is_free && current->size >= size)
      {
         current->is_free = 0;

         if(prev)
         {
            prev->next = current->next;
         }
         else
         {
            free_list_head = current->next;
         }

         current->next = NULL;

         return (void*)(current + 1);
      }

      prev = current;
      current = current->next;
   }

   return NULL;
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

   BlockHeader *block_to_free = (BlockHeader*)ptr - 1;

   block_to_free->is_free = 1;

   block_to_free->next = free_list_head;
   free_list_head = block_to_free;


 }

 /**
  * @brief 
  * 
  * @param nmemb 
  * @param size 
  * @return void* 
  */
 void *my_calloc(size_t nmemb, size_t size) {
    (void)nmemb;
    (void)size;
    return NULL;
 }

 /**
  * @brief 
  * 
  * @param ptr 
  * @param size 
  * @return void* 
  */
 void *my_realloc(void *ptr, size_t size) {
    (void)ptr;
    (void)size;
    return NULL;
 }