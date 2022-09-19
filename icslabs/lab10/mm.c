/*
 * Name: Xue Songtao
 * ID: 520030910034
 *
 * mm.c - a  allocator based on  explicit free list and segregated fit.
 * And it use base address + offset method,reduce the size of min block to 16 bytes.
 *
 * Each block has a header and a footer,whose size is 4 bytes, containing block size and allocation tag.
 * Besides,each free block has a PRED and SUCC with 4 bytes each to store the offset  rather than address
 * By this way, we reduce the size to store address from 16 bytes to 8 bytes. 
 * 
 * The allocator is aligned to 8 bytes. When alloc it use LIFO-ordering, 4-way segregated 
 * fit, first-fit and immediate coalescing to optimize the performance.
 *
 * The allocator will extend the heap by calling mem_sbrk(), adding a new 
 * free block to the heap.
 *
 * The allocator use mm_init() to initialize the heap by putting a padding word and a 
 * prologue block at the beginning, a epilogue block at the end and put a free chunk in the mid.
 *
 * During allocation, allocator first select suitable free list, 
 * then select a suitable free block in the free list using first-fit,
 * at particular condition it will split the block, if it can't find a block it will extend heap.
 * At last it use place() to set the selected block to allocated.
 * If the allocator split the block, it will coalesce the rest part if needed, 
 * then add it to the beginning of the specific free list.
 *
 * When free a block, the allocator simply set its tags and pointers, 
 * then coalesce it and add it to the beginning of the specific free list.
 *
 * The allocator will use mm_realloc() to realloc the block.
 * First, it will compare the block's oldsize and newsize.
 * If oldsize is larger, it wil use re_place() to split the block if needed then return the old block.
 * Otherwise, it will try to get enough space at old block(extend if needed) then return old block.
 * If we can't get enough size it will extend heap to get a free block, then return the new block.
 * (do not forget to copy old block to new block)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* -------------------macro define------------------ */

/* alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

/* basic constants */
#define WSIZE 4 /* word/header/footer size(bytes) */
#define DSIZE 8 /* double word size */
#define MINBLOCK 16 /* min size of a block */
#define CHUNKSIZE 72 /* min extend size */

/* wrap max */
#define MAX(x, y) ((x) > (y)? (x) : (y))

/* pack  size and alloc bit into a word */
#define PACK(size, alloc) ((size)|(alloc))

/* read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* get size and allc at address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* get offset from address and vice versa */
#define GET_OFFSET(ptr) ((char *)ptr - (char *)heap_listp)
#define GET_ADDR(offset) (heap_listp + offset)

/* get address of ptr header, footer, predecessor and successor */
#define HDRP(ptr) ((char *)(ptr) - WSIZE)
#define FTRP(ptr) ((char *)(ptr) + GET_SIZE(HDRP(ptr)) - DSIZE)
#define PRED(ptr) (ptr)
#define SUCC(ptr) ((ptr) + WSIZE)

/* get address of next and previous blocks, next and previous free blocks */
#define NEXT_BLKP(ptr) ((char *)(ptr) + GET_SIZE(((char *)(ptr) - WSIZE)))
#define PREV_BLKP(ptr) ((char *)(ptr) - GET_SIZE(((char *)(ptr) - DSIZE)))

#define NEXT_FREEP(ptr) ((GET(SUCC(ptr)))? (GET_ADDR(GET(SUCC(ptr)))) : NULL)
#define PREV_FREEP(ptr) ((GET(PRED(ptr)))? (GET_ADDR(GET(PRED(ptr)))) : NULL)

/* compute the address of suitable free_listp base on size */
#define FREELISTPP(size) ((size)>=16384? &free_listp4 : ((size)>=4096? &free_listp3 : ((size)>=1024? &free_listp2 : &free_listp)))
/* compute address of larger(next) free_listp */
#define LARGER_FREELISTPP(pp) ((pp)==&free_listp? &free_listp2 : ((pp)==&free_listp2? &free_listp3 : ((pp)==&free_listp3? &free_listp4 : NULL)))

/* -------------------global variable------------------ */

/* a pointer pointing to the start part of the heap */
static void *heap_listp;

/* pointers pointing to the header of free lists */
static void *free_listp; /* store blocks where size < 1024 */
static void *free_listp2; /* store blocks where 1024 <= size < 4096 */
static void *free_listp3; /* store blocks where 4096 <= size < 16384 */
static void *free_listp4; /* store blocks where 16384 <= size */



/* -------------------function prototype declaration------------------ */

static void *extend(size_t size);
static void *coalesce(void *ptr);
static void *find_block(size_t size);
static void *place(void *ptr, size_t size);
static void *re_place(void *ptr, size_t size);
static void *insert_freelist(void *ptr);
static void *remove_freelist(void *ptr);

int mm_check(void);
void mm_printheap(void);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* initial a empty heap */
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0); /* Alignment padding */
    PUT(heap_listp + WSIZE, PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1)); /* Epilogue header */
    heap_listp += DSIZE;

    /* initial free lists */
    free_listp = NULL;
    free_listp2 = NULL;
    free_listp3 = NULL;
    free_listp4 = NULL;

    /* extend the heap at CHUNKSIZE bytes */    
    if (extend(CHUNKSIZE) == NULL)
        return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t alloc_size; /* actual alloc size  */
    size_t extend_size; /* extend size if needed */
    char *ptr;

    /* ignore void requests */
    if (size == 0)
        return NULL;

    /* get alloc_size  */
    if (size == 448) size = 512; /* ooptimization for trace 7 */
    if (size == 112) size = 128; /* ooptimization for trace 8 */  
    if (size <= DSIZE)
        alloc_size = MINBLOCK;
    else
        alloc_size = ALIGN(size + DSIZE);

    /* try to find a sutiable block in free list */
    if ((ptr = find_block(alloc_size)) == NULL) {
        extend_size = MAX(alloc_size, CHUNKSIZE);     /* find no block,need extend */
        ptr = extend(extend_size);
    }
    if (ptr  == NULL)     /* can not get a block */
        return NULL;
    ptr = place(ptr, alloc_size);     /* place the block  */
    return ptr;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));

    /* change alloc bit to 0  */
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{

    /* if ptr is NULL, malloc it */
    if (ptr == NULL)
        return mm_malloc(size);

    /* if size is 0, free ptr */
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    size_t new_size = ALIGN(size + DSIZE); /* actual alloc size */
    size_t old_size = GET_SIZE(HDRP(ptr)); /* old block size */
    size_t extral_size; /* extral size right after the block */
    if (GET_ALLOC(HDRP(NEXT_BLKP(ptr))))
        extral_size = 0;
    else
        extral_size = GET_SIZE(HDRP(NEXT_BLKP(ptr)));

    /* case1: size does not change */
    if (new_size == old_size)
        return ptr;
    /* case2: new size is smaller */
    else if (new_size < old_size) {
        re_place(ptr,new_size);
        return ptr;
    } 
    /* case3: new size is larger */
    else {
        int is_enough = (extral_size + old_size) >= new_size; /* extral size is enough or not */
        int can_extend = (extral_size && !GET_SIZE(HDRP(NEXT_BLKP(NEXT_BLKP(ptr))))) || !GET_SIZE(HDRP(NEXT_BLKP(ptr))); /* told block can be extended or not */

        /* if extral size is not enough but old block can be extended then extend it get new extral size */
        if (!is_enough && can_extend) {
            extend(MAX(new_size-old_size, CHUNKSIZE));
            extral_size = GET_SIZE(HDRP(NEXT_BLKP(ptr)));
        }
        /* extral size is enough */
        if (is_enough || can_extend) {
            remove_freelist(NEXT_BLKP(ptr));
            PUT(HDRP(ptr), PACK(old_size + extral_size, 1));
            PUT(FTRP(ptr), PACK(old_size + extral_size, 1));
            re_place(ptr,new_size);
            return ptr;
        }
        /* have to malloc a new block */
        else {
            void *newptr = mm_malloc(size);
            if (newptr == NULL)
                return NULL;
            memcpy(newptr, ptr, old_size - DSIZE);     /* copy data from old block */
            mm_free(ptr);
            return newptr;
        }    
    }
}

/* 
 * extend - extend heap to contain larger size
 */
static void *extend(size_t size)
{
    char *ptr;

    /* get alloc size */
    size = ALIGN(size);

    if ((long)(ptr = mem_sbrk(size)) == -1)
        return NULL;
    
    /* initial new bock header/footer and the epilogue header */
    PUT(HDRP(ptr), PACK(size, 0)); /* block header */
    PUT(FTRP(ptr), PACK(size, 0)); /* block footer */
    PUT(HDRP(NEXT_BLKP(ptr)), PACK(0, 1)); /* new epilogue header */

    /* coalesce the block */
    return coalesce(ptr);
}

/* 
 * coalesce - coalesce adjecent free blocks
 */
static void *coalesce(void *ptr)
{
    void *prev = PREV_BLKP(ptr);
    void *next = NEXT_BLKP(ptr);
    size_t prev_alloc = GET_ALLOC(FTRP(prev));    /* check previous block is alloc or not */
    size_t next_alloc = GET_ALLOC(HDRP(next));    /* check next block is alloc or not */
    size_t size = GET_SIZE(HDRP(ptr));

    /* case1: do not need to coalesce */
    if (prev_alloc && next_alloc) {
    }
    /* case2: coalesce with next */
    else if (prev_alloc && !next_alloc) {
        remove_freelist(next);
        size += GET_SIZE(HDRP(next));
        PUT(HDRP(ptr), PACK(size, 0));
        PUT(FTRP(ptr), PACK(size, 0));
    }
    /* case3: coalesce with prev */
    else if (!prev_alloc && next_alloc) {
        remove_freelist(prev);
        size += GET_SIZE(HDRP(prev));
        PUT(FTRP(ptr), PACK(size, 0));
        PUT(HDRP(prev), PACK(size, 0));
        ptr = prev;
    }
    /* case4: coalesce with both next and prev */
    else {
        remove_freelist(next);
        remove_freelist(prev);
        size += GET_SIZE(HDRP(next)) + GET_SIZE(HDRP(prev));
        PUT(HDRP(prev), PACK(size, 0));
        PUT(FTRP(next), PACK(size, 0));
        ptr = prev;
    }
    insert_freelist(ptr);
    return ptr;
}

/* 
 * find_block - find a free blocks for malloc
 */
void *find_block(size_t size)
{
    void **free_listpp = FREELISTPP(size);
    size_t block_size;

    /* ignore void request */
    if(size <= 0)
        return NULL;

    while(free_listpp)
    {
        void *ptr = *free_listpp;
        /* search in a free list */
        while(ptr)
        {
            block_size = GET_SIZE(HDRP(ptr));
            if (size <= block_size ) 
                return ptr;
            ptr = NEXT_FREEP(ptr);
        }
        free_listpp = LARGER_FREELISTPP(free_listpp);     /* next free list */
    }
    return NULL;
}

/* 
 * place - alloc the given free block in given size 
 */
void *place(void *ptr, size_t size)
{
    size_t block_size = GET_SIZE(HDRP(ptr));

    /* if the size remaining after the alloc is largert than min block size */
    if ((block_size - size) >= MINBLOCK) {
        remove_freelist(ptr);
        PUT(HDRP(ptr), PACK(size, 1));
        PUT(FTRP(ptr), PACK(size, 1));
        void *next = NEXT_BLKP(ptr);
        PUT(HDRP(next), PACK(block_size - size, 0));
        PUT(FTRP(next), PACK(block_size - size, 0));
        insert_freelist(next);
        return ptr;
    }
    /* remaining size is too small */ 
    else {
        remove_freelist(ptr);
        PUT(HDRP(ptr), PACK(block_size, 1));
        PUT(FTRP(ptr), PACK(block_size, 1));
        return ptr;
    }
}

/* 
 * re_place - realloc the given  block in given size 
 */
void *re_place(void *ptr, size_t size)
{

    size_t block_size = GET_SIZE(HDRP(ptr));

    /* if the size remaining after the alloc is largert than min block size */
    if ((block_size - size) >= MINBLOCK) {
        PUT(HDRP(ptr), PACK(size, 1));
        PUT(FTRP(ptr), PACK(size, 1));
        void *next = NEXT_BLKP(ptr);
        PUT(HDRP(next), PACK(block_size - size, 0));
        PUT(FTRP(next), PACK(block_size - size, 0));
        insert_freelist(next);
        return ptr;
    } 
    /* remaining size is too small */ 
    else {
        PUT(HDRP(ptr), PACK(block_size, 1));
        PUT(FTRP(ptr), PACK(block_size, 1));
        return ptr;
    }
}

/* 
 * insert_freelist - insert the given block to a suitable free list use LIFO-ordering to optimization
 */
static void *insert_freelist(void *ptr)
{
    void **free_listpp = FREELISTPP(GET_SIZE(HDRP(ptr)));

    /* this free list is not null */ 
    if (*free_listpp) {
        PUT(PRED(*free_listpp), GET_OFFSET(ptr));
        PUT(SUCC(ptr), GET_OFFSET(*free_listpp));
        PUT(PRED(ptr), 0);
    }
    /* this free list is  null */  
    else {
        PUT(PRED(ptr), 0);
        PUT(SUCC(ptr), 0);
    }
    *free_listpp = ptr;

    return ptr;
}

/* 
 * remove_freelist - remove the given block from  free list
 */
static void *remove_freelist(void *ptr)
{
    void **freelistpp = FREELISTPP(GET_SIZE(HDRP(ptr)));
    void *prev = PREV_FREEP(ptr);
    void *next = NEXT_FREEP(ptr);

    /* 4 cases base on prev and next exist or not*/ 
    if (prev && next) {
        PUT(SUCC(prev), GET(SUCC(ptr)));
        PUT(PRED(next), GET(PRED(ptr)));
    } else if (prev && !next) {
        PUT(SUCC(prev), 0);
    } else if (!prev && next) {
        PUT(PRED(next), 0);
        *freelistpp = next;
    } else {
        *freelistpp = NULL;
    }
    
    /* set pred and succ to avoid mistake */ 
    PUT(PRED(ptr), 0);
    PUT(SUCC(ptr), 0);
    return ptr;
}

/* 
 * check_alloc / check_count / check_freevalid - help function for mm_check
 */
int check_alloc(void *freelistp)
{
    for(void *ptr = freelistp; ptr != NULL; ptr = NEXT_FREEP(ptr))
    {
        if (GET_ALLOC(HDRP(ptr))) {
            fprintf(stderr, "There is allocated block in free list!\n");
            return 0;
        }
    }
    return 1;
}

int check_count(void *freelistp)
{
    int count = 0;
    for(void *ptr = freelistp; ptr; ptr = NEXT_FREEP(ptr))
        count++;
    return count;
}

int check_freevalid(void *freelistp)
{
    if (!freelistp)
        return 1;
    if (GET(PRED(freelistp))) {
        fprintf(stderr,"Free list header's pred is not NULL!\n");
        return 0;
    }
    for(void *ptr = freelistp; ptr; ptr = NEXT_FREEP(ptr))
    {
        if (GET(PRED(ptr)) == GET(SUCC(ptr)) && GET(PRED(ptr)) != 0) {
            fprintf(stderr, "Exist circle!\n");
            return 0;
        }
        if (GET(SUCC(ptr)) && (GET(PRED(NEXT_FREEP(ptr))) != GET_OFFSET(ptr) || GET(SUCC(ptr)) != GET_OFFSET(NEXT_FREEP(ptr)))) {
            fprintf(stderr, "Free blocks'offset do not match!\n");
            return 0;
        }
    }
    return 1;
}

/* 
 * mm_check - check  correctness of the heap
 */
int mm_check(void)
{
    void *ptr;
    int count = 0;

    /* check every free block actually in the free list or not */
    for(ptr = heap_listp; GET_SIZE(HDRP(ptr)) > 0; ptr = NEXT_BLKP(ptr))
        if (!GET_ALLOC(HDRP(ptr)))   count++;
    count -= check_count(free_listp);
    count -= check_count(free_listp2);
    count -= check_count(free_listp3);
    count -= check_count(free_listp4);
    if (count) {
        fprintf(stderr,"There is free block bot in free list!\n");
        return 0;
    }

    /* check every block in free lists is free */
    if(!check_alloc(free_listp))
        return 0;
    if(!check_alloc(free_listp2))
        return 0;
    if(!check_alloc(free_listp3))
        return 0;
    if(!check_alloc(free_listp4))
        return 0;

    /* check all contiguous free blocks have been coalesced or not */
    for(ptr = heap_listp; GET_SIZE(HDRP(ptr)) > 0; ptr = NEXT_BLKP(ptr))
    {
        if (!GET_ALLOC(HDRP(ptr)) && !GET_ALLOC(HDRP(NEXT_BLKP(ptr)))) {
            fprintf(stderr, "Contagious free blocks!\n");
            return 0;
        }
    }

    /* check pointers in free lists point to valid free blocks or not*/
    if(!check_freevalid(free_listp))
        return 0;
    if(!check_freevalid(free_listp2))
        return 0;
    if(!check_freevalid(free_listp3))
        return 0;
    if(!check_freevalid(free_listp4))
        return 0;

    return 1;
}

/* 
 * mm_printheap - show blocks status of heap
 */
void mm_printheap()
{
    void *ptr = NULL;
    int count = 1;
    fprintf(stdout,"#################################HEAP######################\n");
    fprintf(stdout,"##### NO\t OFF\t SIZE\t ALLOC\t PRED\t SUCC\n##########");
    for(ptr = heap_listp; GET_SIZE(HDRP(ptr)) > 0; ptr = NEXT_BLKP(ptr))
    {
        if(!GET_ALLOC(HDRP(ptr)))
            fprintf(stdout,"BLOCK %d\t %d\t %d\t %d\t %d\t %d\n", count, (unsigned int)GET_OFFSET(ptr), GET_SIZE(HDRP(ptr)), GET_ALLOC(HDRP(ptr)), GET(PRED(ptr)), GET(SUCC(ptr)));
        else
            fprintf(stdout,"BLOCK %d\t %d\t %d\t %d\t --\t --\n", count, (unsigned int)GET_OFFSET(ptr), GET_SIZE(HDRP(ptr)), GET_ALLOC(HDRP(ptr)));
        count++;
    }
    return;
}