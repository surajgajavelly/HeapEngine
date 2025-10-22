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

 static size_t bytes_allocated = 0;

 void allocator_init(void) 
 {
   bytes_allocated = 0;
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

   if ((bytes_allocated + size) > HEAP_SIZE)
   {
      return NULL;
   }

   void *ptr = &heap[bytes_allocated];

   bytes_allocated += size;

   return ptr;
 }

 void my_free(void *ptr) 
 {
    (void)ptr;
 }

 void *my_calloc(size_t nmemb, size_t size) {
    (void)nmemb;
    (void)size;
    return NULL;
 }

 void *my_realloc(void *ptr, size_t size) {
    (void)ptr;
    (void)size;
    return NULL;
 }