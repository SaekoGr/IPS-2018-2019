// 
/**
 * Implementace My MALloc
 * Demonstracni priklad pro 1. ukol IPS/2018
 * Ales Smrcka
 */

#include "mmal.h"
#include <sys/mman.h> // mmap
#include <stdbool.h> // bool
#include <assert.h> // assert

#ifdef NDEBUG
/**
 * The structure header encapsulates data of a single memory block.
 *   ---+------+----------------------------+---
 *      |Header|DDD not_free DDDDD...free...|
 *   ---+------+-----------------+----------+---
 *             |-- Header.asize -|
 *             |-- Header.size -------------|
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
 *   /--- arena metadata
 *   |     /---- header of the first block
 *   v     v
 *   +-----+------+-----------------------------+
 *   |Arena|Header|.............................|
 *   +-----+------+-----------------------------+
 *
 *   |--------------- Arena.size ---------------|
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

#define PAGE_SIZE (128*1024)

#endif // NDEBUG

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20
#endif

Arena *first_arena = NULL;

/**
 * Return size alligned to PAGE_SIZE
 */
static
size_t allign_page(size_t size)
{
    assert(size > 0);
    // it must be bigger than 0
    // PAGE_SIZE is 128*1024 = 131 072 and we should align it by it
    // Formula is: (size + align_size - 1)/align_size * align_size
    size_t current_size = PAGE_SIZE; 
    return ((size+current_size-1)/current_size*current_size);
}

/**
 * Allocate a new arena using mmap.
 * @param req_size requested size in bytes. Should be alligned to PAGE_SIZE.
 * @return pointer to a new arena, if successfull. NULL if error.
 * @pre req_size > sizeof(Arena) + sizeof(Header)
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
    assert(req_size > sizeof(Arena) + sizeof(Header));
    
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
 * Appends a new arena to the end of the arena list.
 * @param a     already allocated arena
 */
static
void arena_append(Arena *a)
{
    Arena* arena_tmp;
    arena_tmp = first_arena;
    while(arena_tmp->next != NULL){
    arena_tmp = arena_tmp->next;
    }
    arena_tmp->next = a;
}

/**
 * Header structure constructor (alone, not used block).
 * @param hdr       pointer to block metadata.
 * @param size      size of free block
 * @pre size > 0
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
    assert(size > 0);
    hdr->asize = 0;
    hdr->size = size;
}

/**
 * Checks if the given free block should be split in two separate blocks.
 * @param hdr       header of the free block
 * @param size      requested size of data
 * @return true if the block should be split
 * @pre hdr->asize == 0
 * @pre size > 0
 */
static
bool hdr_should_split(Header *hdr, size_t size)
{
    assert(hdr->asize == 0);
    assert(size > 0);
    
    if((hdr->size >= size + 2*sizeof(Header))){
        return true;
    }
    else{
        return false;
    }
}

/**
 * Splits one block in two.
 * @param hdr       pointer to header of the big block
 * @param req_size  requested size of data in the (left) block.
 * @return pointer to the new (right) block header.
 * @pre   (hdr->size >= req_size + 2*sizeof(Header))
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
    assert((hdr->size >= req_size + 2*sizeof(Header)));
    
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
 * Detect if two adjacent blocks could be merged.
 * @param left      left block
 * @param right     right block
 * @return true if two block are free and adjacent in the same arena.
 * @pre left->next == right
 * @pre left != right
 */
static
bool hdr_can_merge(Header *left, Header *right)
{
    assert(left->next == right);
    assert(left != right);
    
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
 * @pre left->next == right
 * @pre left != right
 */
static
void hdr_merge(Header *left, Header *right)
{
    assert(left->next == right);
    assert(left != right);
    
    if(hdr_can_merge(left, right)){
        int my_size = left->size;
        left->size = my_size + right->size + sizeof(Header);
        left->next = right->next;
    }

}

/**
 * Finds the first free block that fits to the requested size.
 * @param size      requested size
 * @return pointer to the header of the block or NULL if no block is available.
 * @pre size > 0
 */
static
Header *first_fit(size_t size)
{
    assert(size > 0);
    bool found = false;
    Header* first_header = (Header *)&first_arena[1];
    Header* tmp_header = (Header *)&first_arena[1];
    while(tmp_header->next != first_header || !found){
        if(tmp_header->asize == 0){
            if(hdr_should_split(tmp_header, size)){
                found = true;
                break;
            }
        }
        if(tmp_header->next == first_header){
            break;
        }
        tmp_header = tmp_header->next;
    }

    if(found){
        return tmp_header;
    }
    else{
        return NULL;
    }

}

/**
 * Search the header which is the predecessor to the hdr. Note that if 
 * @param hdr       successor of the search header
 * @return pointer to predecessor, hdr if there is just one header.
 * @pre first_arena != NULL
 * @post predecessor->next == hdr
 */
static
Header *hdr_get_prev(Header *hdr)
{
    assert(first_arena != NULL);
    Header* tmp = hdr;
    while(tmp->next != hdr){
        tmp = tmp->next;
    }
    return tmp;
}

/**
 * Allocate memory. Use first-fit search of available block.
 * @param size      requested size for program
 * @return pointer to allocated data or NULL if error or size = 0.
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
    //Arena* arena_tmp;
    //bool found = false;
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
            my_header = (Header*) &first_arena[1];
            hdr_ctor(my_header, size_of_arena - sizeof(Header));    // we are creating the first header
            my_header->next = my_header;                        // it is cyclical list

            my_header = first_fit(size);                        // We put header inside first free place of the allocated place, but at first must skip the arena header

            tmp_header = hdr_split(my_header, size);
            if(tmp_header != NULL)
                return(&my_header[1]);             // Must be only one, need to skip the header, and give user just his space (42)
        }
    }
    else{
        // search through the existing arenas and find the best fit                       
        // if not possible to find, allocate new arenas
        my_header = first_fit(size);
        
        if(my_header != NULL){
            result = hdr_split(my_header, size);
            char *temp_pointer = (char *)result;
            temp_pointer = temp_pointer - size;
            return temp_pointer; // return everything from the previous header up to the current free header
        }
        else{
            // it doesn't fit in the first arena
            last_arena = arena_alloc(size + sizeof(Header));
            // append arena
            arena_append(last_arena);

            last_header = (Header *)&first_arena[1];
            my_header = (Header *)&first_arena[1];
            while(last_header->next != my_header){
                last_header = last_header->next;
            }

            int size_of_arena = allign_page(size + sizeof(Header));
            new_header = (Header*) &last_arena[1];
            hdr_ctor(new_header, size_of_arena - sizeof(Header));

            new_header->next = last_header->next;
            last_header->next = new_header;

            result = first_fit(size);

            tmp_header = hdr_split(result, size);
            if(tmp_header != NULL)
                return(&result[1]); 

            /*
            my_header = first_fit(size);                        // We put header inside first free place of the allocated place, but at first must skip the arena header
            tmp_header = hdr_split(my_header, size);
            if(tmp_header != NULL)
                return(&my_header[1]); 
            */
        }
    }

    // FIXME
    (void)size;
    return NULL;
}

/**
 * Free memory block.
 * @param ptr       pointer to previously allocated data
 * @pre ptr != NULL
 */
void mfree(void *ptr)
{
    assert(ptr != NULL);
    
    Header* my_header; 
    Header* previous;
    Header* next;
    int new_size;

    // current header
    my_header = ptr - sizeof(Header);   // find header of my current pointer
    // next header
    next = my_header->next;                 // next one is easy to find
    // 
    previous = hdr_get_prev(my_header);     // previous has to be found by cycling through the list
    

    // TODO: MERGE and FREE
    new_size = my_header->asize;
    my_header->asize = 0;
    my_header->size = new_size;
    // first, try to merge current and next
    if(my_header->next == next && my_header != next){
        hdr_merge(my_header, next);
    }
    // then, try to merge previous and current
    if(previous->next == my_header && previous != my_header){
        hdr_merge(previous, my_header);
    }

}

/**
 * Reallocate previously allocated block.
 * @param ptr       pointer to previously allocated data
 * @param size      a new requested size. Size can be greater, equal, or less
 * then size of previously allocated block.
 * @return pointer to reallocated space or NULL if size equals to 0.
 */
void *mrealloc(void *ptr, size_t size)
{
    // Get header
    char *temp_pointer = (char *)ptr - sizeof(Header);
    Header *user_place = (Header *)temp_pointer;
    // User wants smaller size, just make new header with free place 
    if(user_place->asize > size)
    {
        user_place->size = (user_place->asize - size);
        user_place->asize = size;
        // Return user pointer to his new space (actually, its the same place lol)
        return ptr;
    }
    // user wants bigger space
    else if(user_place->asize < size)
    {
        // Firstly check if the user can get more in the same block
        if((user_place->size + user_place->asize) >= size)
        {
            user_place->size = (user_place->size + user_place->asize) - size; // size of whole block - requested size stays free
            user_place->asize = size; // allocated size is new size
            // If fitted, return same pointer
            return ptr;
        }
        // User cannot get more space in the same block, need to find new for him and copy his data there
        else
        {
            // Free the pointer and find new place
            char *new_user_place = mmalloc(size);
            char *old_place = (char *)ptr;
            for(size_t i = 0; i < user_place->asize; i++)
            {
                new_user_place[i] = old_place[i];
            }
            mfree(ptr);
            return new_user_place;
        }
    }
    else if(size == 0)
    {
        return NULL;
    }
    // User wants same size, return same pointer
    else 
    {
        return ptr;
    }
}
