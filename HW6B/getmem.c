/* getmem.c
   implements getmem (malloc) for memory system
   CSE 374 HW6
*/

#include <assert.h>
#include "mem.h"
#include "mem_impl.h"

/* This code will be filled in by the students.  Your primary job is to
   implement getmem */

void splitBlock(freeNode* previous, freeNode* block, uintptr_t size);

/* initialize global variables ?*/
freeNode* freelist = NULL;
uintptr_t totalmalloc = 0;

void* getmem(uintptr_t size) {
   check_heap();
  /* make sure you return a pointer to the usable memory that
     is at least 'size' bytes long.*/
   if (size <= 0) {
      return NULL;
   }
   // checn for alignment
   size = (size + 15) & (~15);

   freeNode* curr = freelist;
   freeNode* prev = NULL;
   // Search for appropiate block
   while (curr != NULL) {
      if (curr->size - NODESIZE >= size) {
         if (prev == NULL) {
            freelist = curr->next;
         } else {
            prev->next = curr->next;
         }
         // If the block is too large, split it
         if (curr->size - NODESIZE > size + SPARE) {
            splitBlock(prev, curr, size);
         }
         return (void*)((uintptr_t)curr + NODESIZE);
      }
      prev = curr;
      curr = curr->next;
   }
   // If no block satisfies the requirement, allocate a new block of memory
   uintptr_t allocSize = (size + NODESIZE > 8000) ? (size + NODESIZE) : 8000;
   void* newBlock = malloc(allocSize);
   if (newBlock == NULL) {
      return NULL;
   }
   totalmalloc += allocSize;

   freeNode* newNode = (freeNode*) newBlock;
   newNode->size = allocSize - NODESIZE;

   prev = NULL;
   curr = freelist;
   // find the appropiate address for the new block
   while (curr != NULL && (uintptr_t)curr < (uintptr_t)newNode) {
      prev = curr;
      curr = curr->next;
   }
   if (newNode->size - NODESIZE > size + SPARE) {
      splitBlock(prev, newNode, size);
   }
   return (void*)((uintptr_t)newNode + NODESIZE);
}

void splitBlock(freeNode* previous, freeNode* block, uintptr_t size) {
   freeNode* remainNode = (freeNode*)((uintptr_t)block + NODESIZE + size);
   remainNode->size = block->size - size - NODESIZE;
   if (previous == NULL) {
      remainNode->next = freelist;
      freelist = remainNode;
   } else{
      remainNode->next = previous->next;
      previous->next = remainNode;
   }
   block->size = size;

   check_heap();
}