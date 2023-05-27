/* bench.c is the benchmarking /test program for mem memory management
   bench.c is implemented as part of HW6, CSE374 22WI
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "mem.h"

/* print_stats is a helper function that prints performance
   information about the memory system.  Requires a clock_t start
   time, and prints elapsed time plus results from get_mem_stats()
*/
void print_stats(clock_t start);

/* fill_mem is a helper function that takes in an address and the
   size of the memory allocation at that address.  It should fill
   the first 16 bytes (or up to size) with hexedecimal values.*/
void fill_mem(void* ptr, uintptr_t size);

/* Synopsis:   bench (main)
   [ntrials] (10000) getmem + freemem calls
   [pctget] (50) % of calls that are get mem
   [pctlarge] (10) % of calls requesting more memory than lower limit
   [small_limit] (200) largest size in bytes of small block
   [large_limit] (20000) largest size in byes of large block
   [random_seed] (time) initial seed for randn
*/
int main(int argc, char** argv ) {
    // Initialize the parameters
    int NTRIALS = 10000;
    int PCTGET = 50;
    int PCTLARGE = 10;
    int SMALL_L = 200;
    int LARGE_L = 20000;
    unsigned int SEED = (unsigned int)time(NULL);

    /* write code to set the above values based on the input arguments
     if there is not a corresponding input argument use the
     default value in the spec. */
    if (argc >= 2) {
       NTRIALS = atoi(argv[1]);
    }
    if (argc >= 3) {
       PCTGET = atoi(argv[2]);
    }
    if (argc >= 4) {
       PCTLARGE = atoi(argv[3]);
    }
    if (argc >= 5) {
       SMALL_L = atoi(argv[4]);
    }
    if (argc >= 6) {
       LARGE_L = atoi(argv[5]);
    }
    if (argc >= 7) {
       SEED = atoi(argv[6]);
    }

    printf("Running bench for %d trials, %d%% getmem calls.\n",
    NTRIALS, PCTGET);

    void* blocks[NTRIALS];  // upperbound block storage
    int ntrials = 0, nblocks = 0;
    clock_t start = clock();

    // perform NTRIALS mem operations
    srand(SEED);
    for (ntrials = 0; ntrials < NTRIALS; ntrials++) {
       if ((rand() % 100) < PCTGET) {
          // Perform getmem operation
          uintptr_t size;
          if ((rand() % 100) < PCTLARGE) {
             size = SMALL_L + rand() % (LARGE_L - SMALL_L + 1);
          } else {
             size = rand() % SMALL_L + 1;
          }
          blocks[nblocks] = getmem(size);
          if (blocks[nblocks]) {
            fill_mem(blocks[nblocks], size);
          }
          nblocks++;
       } else {
          // Perform freemem operation
          if (nblocks == 0) {
             continue;
          }
          int index = rand() % nblocks;
          freemem(blocks[index]);
          nblocks--;
          blocks[index] = blocks[nblocks];
       }
       if (NTRIALS < 10) {
          print_stats(start);
       }
       if (NTRIALS >= 10 && (ntrials + 1) % (NTRIALS / 10) == 0) {
          print_stats(start);
       }
    }

    return EXIT_SUCCESS;
}

/* Define helper functions here. */
void print_stats(clock_t start) {
    clock_t now = clock();
    uintptr_t total_size;
    uintptr_t total_free;
    uintptr_t n_free_blocks;
    uintptr_t avg_bytes_free;
    get_mem_stats(&total_size, &total_free, &n_free_blocks);
    if (n_free_blocks > 0) {
       avg_bytes_free = (total_free / n_free_blocks);
    } else {
       avg_bytes_free = 0;
    }
    printf("Time elapsed: %.5f\n", (float)(now - start) / CLOCKS_PER_SEC);
    printf("Total storage acquired: %lu\n", total_size);
    printf("Total blocks freed: %lu\n", n_free_blocks);
    printf("Average bytes freed per block: %lu\n", avg_bytes_free);
}

void fill_mem(void* ptr, uintptr_t size) {
    unsigned char* bytePtr = (unsigned char*)ptr;
    for (uintptr_t i = 0; i < size && i < 16; i++) {
       bytePtr[i] = 0xFE;
    }
}
