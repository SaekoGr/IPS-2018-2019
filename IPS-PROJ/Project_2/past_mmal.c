/**
 * Implementace My MALloc
 * Demonstracni priklad pro 1. ukol IPS/2018
 * Ales Smrcka
 */

#include "mmal.h"
#include <sys/mman.h> // mmap
#include <stdbool.h> // bool
#include <assert.h>
#include <stdio.h>

/**
 * TODO:
 * allocation of new arena if necessary
 * 
 * 
 * 
 */

#ifdef NDEBUG
/**
 * The structure header encapsulates data of a single memory block.
 */
typedef struct header Header;
struct header {

    /**
     * Pointer to the next header. Cyclic list. If there is no other block,
     * points to itself.
     */
    Header *next;

    /// size of the block
    size_t size;

    /**
     * Size of block in bytes allocated for program. asize=0 means the block 
     * is not used by a program.
     */
    size_t asize;
};

/**
 * The arena structure.
 */
typedef struct arena Arena;
struct arena {

    /**
     * Pointer to the next arena. Single-linked list.
     */
    Arena *next;

    /// Arena size.
    size_t size;
};

#define PAGE_SIZE 128*1024

#endif

Arena *first_arena = NULL;

/**
 * Return size alligned to PAGE_SIZE
 */
static
size_t allign_page(size_t size)
{   
    if(size <= 0)
        return 0;
    // it must be bigger than 0
    // PAGE_SIZE is 128*1024 = 131 072 and we should align it by it
    // Formula is: (size + align_size - 1)/align_size * align_size 
    return ((size+131072-1)/131072*131072);
}

/**
 * Allocate a new arena using mmap.
 * @param req_size requested size in bytes. Should be alligned to PAGE_SIZE.
 * @return pointer to a new arena, if successfull. NULL if error.
 */

/**
 *   +-----+------------------------------------+
 *   |Arena|....................................|
 *   +-----+------------------------------------+
 *
 *   |--------------- Arena.size ---------------|
 */
static
Arena *arena_alloc(size_t req_size)
{
    // variable for arena and aligned size
    Arena* new_arena = NULL;
    size_t aligned_size = allign_page(req_size);
    int fd = -1; // TODO - what should this number be?

    /**
     * MMAP is used as: in real malloc:
     * mmap (addr, size, prot, flags|MAP_ANONYMOUS|MAP_PRIVATE, -1, 0)
     * so I leave it like this for now
     */
    new_arena = (Arena *)mmap(0, aligned_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0); // TODO - check tha parameters
    if(new_arena == MAP_FAILED)           // failed mmap
        return NULL;

    new_arena->next = NULL;
    new_arena->size = aligned_size;
    
    return new_arena;
}

/**
 * Header structure constructor (alone, not used block).
 * @param hdr       pointer to block metadata.
 * @param size      size of free block
 */
/**
 *   +-----+------+------------------------+----+
 *   | ... |Header|........................| ...|
 *   +-----+------+------------------------+----+
 *
 *                |-- Header.size ---------|
 */
static
void hdr_ctor(Header *hdr, size_t size)
{
    hdr->asize = 0;
    hdr->size = size;

    // FIXME
    //(void)hdr;
    //(void)size;
}

/**
 * Splits one block into two.
 * @param hdr       pointer to header of the big block
 * @param req_size  requested size of data in the (left) block.
 * @pre   (req_size % PAGE_SIZE) = 0
 * @pre   (hdr->size >= req_size + 2*sizeof(Header))
 * @return pointer to the new (right) block header.
 */
/**
 * Before:        |---- hdr->size ---------|
 *
 *    -----+------+------------------------+----
 *         |Header|........................|
 *    -----+------+------------------------+----
 *            \----hdr->next---------------^
 */
/**
 * After:         |- req_size -|
 *
 *    -----+------+------------+------+----+----
 *     ... |Header|............|Header|....|
 *    -----+------+------------+------+----+----
 *             \---next--------^  \--next--^
 */
static
Header *hdr_split(Header *hdr, size_t req_size)
{   
    // check the pre-conditions first
    if((req_size % PAGE_SIZE == 0)){
        return NULL; // should we return NULL or the right block?
    }
    else if(!(hdr->size >= (req_size + 2*sizeof(Header)))){
        return NULL;
    }
    int new_size = hdr->size - req_size - sizeof(Header);
 
    hdr->asize = req_size; // this is our malloced area
    hdr->size = 0;  // we put next header right next to it, so it is 0
    char *temp_header;
    Header *new_header;
    // we use pointer arithmetic to put it into the mapped area
    // Need to retype it to char so we can 
    temp_header = (char*)&hdr[1] + req_size; // hdr[1] points out of header on the users place and + req_size will skip the users space and there should be next new head
    new_header = (Header *) temp_header;
    hdr_ctor(new_header, new_size);
    

    // it is cyclical list
    new_header->next = hdr->next;
    hdr->next = new_header;

    return new_header;
}

/**
 * Detect if two blocks adjacent blocks could be merged.
 * @param left      left block
 * @param right     right block
 * @return true if two block are free and adjacent in the same arena.
 */
static
bool hdr_can_merge(Header *left, Header *right)
{
    char* left_adress = (char *) left + sizeof(Header) + left->size;
    Header* address = (Header*) left_adress;
    
    if(address == right){
        if(left->asize == 0 && right->asize == 0){
            return true;
        }
        else{
            return false;
        }    
    }
    else{
        return false;
    }
}

/**
 * Merge two adjacent free blocks.
 * @param left      left block
 * @param right     right block
 */
static
void hdr_merge(Header *left, Header *right)
{
    if(hdr_can_merge(left, right)){
        int my_size = left->size;
        left->size = my_size + right->size + sizeof(Header);
        left->next = right->next;
    }

}

/**
 * Allocate memory. Use first-fit search of available block.
 * @param size      requested size for program
 * @return pointer to allocated data or NULL if error.
 */
void *mmalloc(size_t size)
{
    Header* my_header;
    Header* tmp_header;
    Header* result;
    Header* last_header;
    Header* new_header;
    Arena* found_arena;
    Arena* last_arena;
    Arena* arena_tmp;
    bool found = false;
    int size_of_arena;
    //my_header = first_arena;


    // we should add the algoritms as follows:
    if(first_arena == NULL){    // first allocation
        found_arena = arena_alloc(size + sizeof(Header));               // allocating the first arena
        first_arena = found_arena;
        first_arena->next = NULL;
        if(first_arena == NULL){                            // check allocation success
            return NULL;
        }
        else{
            size_of_arena = allign_page(size + sizeof(Header));
            my_header = (char *)&found_arena[1];                        // We put header inside first free place of the allocated place, but at first must skip the arena header
            hdr_ctor(my_header, size_of_arena - sizeof(Header));    // we are creating the first header
            my_header->next = my_header;                        // it is cyclical list

            tmp_header = hdr_split(my_header, size);
            if(tmp_header != NULL)
                return(&my_header[1]);             // Must be only one, need to skip the header, and give user just his space (42)
        }
    }
    else{                       // search through the existing arenas and find the best fit
        ;                       // if not possible to find, allocate new arenas
        // TODO allocate new area
        tmp_header = &first_arena[1];  // this is the first header
        my_header = &first_arena[1];

        while(tmp_header->next != my_header || !found){   // go through it until you come back
            result = hdr_split(tmp_header, size);
            if(result != NULL){
                found = true;
            }
            if(tmp_header->next == my_header){
                break;
            }
            tmp_header = tmp_header->next;
        }

        if(result != NULL){
            char *temp_pointer = (char *)result;
            temp_pointer = temp_pointer - size;
            return temp_pointer; // return everything from the previous header up to the current free header
        }
        else{
            printf("Prisiel som namapovat arenu\n");
            // it doesn't fit in the first arena
            last_arena = arena_alloc(size + sizeof(Header));

            arena_tmp = first_arena;
            while(arena_tmp->next != NULL){
                arena_tmp = arena_tmp->next;
            }
            arena_tmp->next = last_arena;

            last_header = (Header *)&first_arena[1];
            my_header = (Header *)&first_arena[1];
            while(last_header->next != my_header){
                last_header = last_header->next;
            }

            int size_of_arena = allign_page(size + sizeof(Header));
            new_header = (Header*) &last_arena[1];
            hdr_ctor(new_header, size_of_arena - sizeof(Header));

            new_header = last_header->next;
            last_header->next = size_of_arena;

            tmp_header = hdr_split(new_header, size);
            if(tmp_header != NULL)
                return(&my_header[1]); 

        }
    }
    

    
    //my_header = found_arena;

    //my_header->next = my_header;

    // FIXME
    (void)size;
    return NULL;
}

/**
 * Free memory block.
 * @param ptr       pointer to previously allocated data
 */
void mfree(void *ptr)
{
    Header* my_header; 
    Header* previous;
    Header* next;
    int new_size;

    // current header
    my_header = ptr - sizeof(Header);   // find header of my current pointer
    // next header
    next = my_header->next;                 // next one is easy to find
    // 
    previous = my_header;                   // previous has to be found by cycling through the list
    while(previous->next != my_header){
        previous = previous->next;
    }

    // TODO: MERGE and FREE
    new_size = my_header->asize;
    my_header->asize = 0;
    my_header->size = new_size;
    // first, try to merge current and next
    hdr_merge(my_header, next);

    // then, try to merge previous and current
    hdr_merge(previous, my_header);
   
    printf("Freeing\n");
}

/**
 * Reallocate previously allocated block.
 * @param ptr       pointer to previously allocated data
 * @param size      a new requested size. Size can be greater, equal, or less
 * then size of previously allocated block.
 * @return pointer to reallocated space.
 */
void *mrealloc(void *ptr, size_t size)
{
    // FIXME
    (void)ptr;
    (void)size;
    return NULL;
}
