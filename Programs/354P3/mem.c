////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
// Other Files:
// Semester:         CS 354 SPRING 2019
//
// Author:           Runheng Lei
// Email:            rlei5@wisc.edu
// CS Login:         runheng
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct block_header {
    int size_status;
    /*
    * Size of the block is always a multiple of 8.
    * Size is stored in all block headers and free block footers.
    *
    * Status is stored only in headers using the two least significant bits.
    *   Bit0 => least significant bit, last bit
    *   Bit0 == 0 => free block
    *   Bit0 == 1 => allocated block
    *
    *   Bit1 => second last bit
    *   Bit1 == 0 => previous block is free
    *   Bit1 == 1 => previous block is allocated
    *
    * End Mark:
    *  The end of the available memory is indicated using a size_status of 1.
    *
    * Examples:
    *
    * 1. Allocated block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 27
    *      If the previous block is free, size_status should be 25
    *
    * 2. Free block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 26
    *      If the previous block is free, size_status should be 24
    *    Footer:
    *      size_status should be 24
    */
} block_header;

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */

block_header *start_block = NULL;

/*
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block on success.
 * Returns NULL on failure.
 * This function should:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8 and possibly adding padding as a result.
 * - Use BEST-FIT PLACEMENT POLICY to find the block closest to the required block size
 * - Use SPLITTING to divide the chosen free block into two if it is too large.
 * - Update header(s) and footer as needed.
 * Tips: Be careful with pointer arithmetic.
 */
void* Alloc_Mem(int size) {
    // if size is zero, allocate failed, return null
    if(size <= 0){
        return NULL;
    }

    // compute block size as a multiple of 8
    int actualSize = size + 4;
    if((actualSize)%8 != 0){
        actualSize = actualSize + 8 - actualSize % 8;
    }


    block_header *runner = start_block; // temp ptr that traverse through all blocks
    block_header *mostClose = NULL; // mostClose ptr stores the addr for a free block with closest size as needed

    // traverse until ending mark is hit
    while(runner->size_status != 1){

        // current block is allocated, skip current block
        if(((runner->size_status) & 1) == 1){
            runner =  (block_header*)((void*)runner + runner->size_status - ((runner->size_status) & 3));
        }

            // current block is empty
        else{

            // exact match, allocate here
            if((runner->size_status - (runner->size_status & 3)) == actualSize){
                runner->size_status = runner->size_status + 1; // change current header bit0 to 1
                if (((block_header*)((void*)runner + actualSize))->size_status != 1) {
                    ((block_header *) ((void *) runner + actualSize))->size_status += 2;// change next header bit1 to 1
                }
                return ((void*)runner + sizeof(block_header)); // return a pointer to the start of the allocated payload
            }

                // too small, continue searching
            else if((runner->size_status - (runner->size_status & 3)) < actualSize){
                runner = (block_header*)((void*)runner + runner->size_status - ((runner->size_status) & 3));
            }

                //larger enough, save to array for later comparison
            else{

                // if this is first available block, store into mostClose ptr
                if(mostClose == NULL){
                    mostClose = runner;
                }
                    // if it's not first available block, compare size, and store addr of more suitable block into mostClose
                else{
                    // store two blocks' size into two variables and compare to find the smaller one
                    int mostCloseSize = mostClose->size_status - (mostClose->size_status & 3);
                    int currentSize = runner->size_status - (runner->size_status & 3);
                    if(mostCloseSize > currentSize){ // if smaller size found, update mostClose ptr
                        mostClose = runner;
                    }
                    // otherwise, mostClose still stores best fit block addr
                }
                runner = (block_header*)((void*)runner + runner->size_status - ((runner->size_status) & 3));
            }
        }
    }

    // after traverse through the entire memory block, and no perfect match is found, allocate to mostClose block
    // no available free block is found, allocation failed, return null
    if(mostClose == NULL ){
        return NULL;
    }
    else{
        // get this free block's original size
        int originalFreeSize = mostClose->size_status - (mostClose->size_status & 3);
        // compute available size left as the size of new free block after splitting
        int newFreeSize = originalFreeSize - actualSize;

        // update footer
        ((block_header*)((void*)mostClose + originalFreeSize - sizeof(block_header)))->size_status = newFreeSize;

        // update new allocated block's header
        mostClose->size_status = actualSize + (mostClose->size_status & 2) + 1;

        // update new free block's header
        ((block_header*)((void*)mostClose + actualSize))->size_status = newFreeSize + 2 + 0;

        // return start addr of allocated payload
        return ((block_header*)((void*)mostClose + sizeof(block_header)));
    }
}


/*
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - USE IMMEDIATE COALESCING if one or both of the adjacent neighbors are free.
 * - Update header(s) and footer as needed.
 */
int Free_Mem(void *ptr) {

    // compute heap size by loop through heap
    block_header* runner = start_block; // set runner ptr to start of heap
    int heap_size = 0;
    while(runner->size_status != 1){
        heap_size = heap_size + (runner->size_status - (runner->size_status & 3));
        runner = (block_header*)((void*)runner + runner->size_status - (runner->size_status & 3));
    }

    // if ptr is NULL, free failed, return -1
    if(ptr == NULL){
        return -1;
    }

    // if ptr is not multiple of 8, wrong ptr value, return -1
    if(((int)ptr) % 8 != 0){
        return -1;
    }

    // if ptr is outside of the heap space, return -1
    if(((int)ptr) < ((int)start_block) ||((int)ptr) >= ((int)((void*)start_block + heap_size))){
        return -1;
    }

    // if ptr is already freed, return -1
    if((((block_header*)((void*)ptr - sizeof(block_header)))->size_status & 1) == 0){
        return -1;
    }
    // get current block_header
    block_header* current = (void *) ptr - sizeof(block_header);
    current->size_status -= 1; // change current status
    int current_space = current->size_status - (current->size_status & 3);
    // update footer
    ((block_header*)((void*)current + current_space - sizeof(block_header)))->size_status = current_space;

    // update next block header
    block_header* next = (void*)current + current_space;
    next->size_status -= 2;
    // if next block is free, coalesce
    if((next->size_status & 1) == 0){
        int next_space = next->size_status - (next->size_status & 3);
        // update header
        current->size_status += next_space;
        // update footer
        block_header* footer = (void*)next + next_space - sizeof(block_header);
        footer->size_status = current_space + next_space;
    }

    // check if previous block is also free
    if((current->size_status & 2) == 0){
        int prev_space = ((block_header*)((void*)current - sizeof(block_header)))->size_status;
        block_header* prev = (void*)current - prev_space;
        // update header
        prev->size_status = current->size_status + prev_space + 2;
        // update footer
        block_header* final_footer = (void*)current + current->size_status - sizeof(block_header);
        final_footer->size_status = current->size_status + prev_space;
    }

    return 0;
}


/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */
int Init_Mem(int sizeOfRegion) {
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    block_header* end_mark;
    static int allocated_once = 0;

    if (0 != allocated_once) {
        fprintf(stderr,
        "Error:mem.c: Init_Mem has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE,
                    fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }

    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that start block meets
    // double word alignement requirement
    start_block = (block_header*) space_ptr + 1;
    end_mark = (block_header*)((void*)start_block + alloc_size);

    // Setting up the header
    start_block->size_status = alloc_size;

    // Marking the previous block as used
    start_block->size_status += 2;

    // Setting up the end mark and marking it as used
    end_mark->size_status = 1;

    // Setting up the footer
    block_header *footer = (block_header*) ((char*)start_block + alloc_size - 4);
    footer->size_status = alloc_size;

    return 0;
}

/*
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts)
 * t_End    : address of the last byte in the block
 * t_Size   : size of the block as stored in the block header
 */
void Dump_Mem() {
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    block_header *current = start_block;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");

    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;

        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "used");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "used");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_used)
            used_size += t_size;
        else
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status,
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

        current = (block_header*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total used size = %d\n", used_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", used_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}