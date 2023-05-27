/* freemem.c
   implements freemem (free) for memory system
*/

#include <assert.h>
#include "mem.h"
#include "mem_impl.h"

void freemem(void* p) {
   check_heap();

   if (p == NULL) {
      return;
   }

   freeNode* returnBlock = (freeNode*)((uintptr_t)p - NODESIZE);

   // Find the position to insert the block in the sorted free list
   freeNode* prev = NULL;
   freeNode* curr = freelist;
   while (curr != NULL && (uintptr_t)curr < (uintptr_t)returnBlock) {
      prev = curr;
      curr = curr->next;
   }
   // check for adjacent with previous
   if (prev == NULL) {
      returnBlock->next = freelist;
      freelist = returnBlock;
   } else if ((uintptr_t)prev + prev->size + NODESIZE == (uintptr_t)returnBlock) {
      prev->size += returnBlock->size + NODESIZE;
      returnBlock = prev;
   } else {
      returnBlock->next = curr;
      prev->next = returnBlock;
   }
   // check for adjacent with next
   if (curr != NULL && (uintptr_t)returnBlock + returnBlock->size + NODESIZE == (uintptr_t)curr) {
      returnBlock->size += curr->size + NODESIZE;
      returnBlock->next = curr->next;
   } 

   check_heap();
}

