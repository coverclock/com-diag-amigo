#ifndef _COM_DIAG_AMIGO_HEAP_H_
#define _COM_DIAG_AMIGO_HEAP_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * These symbols are in the global name space!\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/cxxcapi.h"

#if defined(__cplusplus)

/**
 * Allocate a block of memory from the heap. No alignment is guaranteed.
 * @param size is the desired amount of memory in bytes.
 * @return a pointer to the newly allocated memory block or 0 if fail.
 */
void * operator new(size_t size);

/**
 * Return a block of memory to the heap.
 * @param ptr points to the previously newed memory block to be deleted.
 */
void operator delete(void * ptr);

/**
 * Allocate a block of memory from the heap. No alignment is guaranteed. The
 * memory block represents an array.
 * @param size is the desired total amount of memory in bytes.
 * @return pointer to the newly allocated memory block or 0 if fail.
 */
void * operator new[](size_t size);

/**
 * Return a block of memory to the heap. The memory block represents an array.
 * @param ptr points to the previously newed memory block to be deleted.
 */
void operator delete[](void * ptr);

#endif

/**
 * Allocate a block of memory from the heap. No alignment is guaranteed.
 * @param size is the desired amount of memory in bytes.
 * @return pointer to the newly allocated memory block or 0 if fail.
 */
CXXCAPI void * malloc(size_t size);

/**
 * Return a block of memory to the heap.
 * @param ptr points to the previously malloced memory block to be freed.
 */
CXXCAPI void free(void * ptr);

/**
 * Allocate a block of memory from the heap. No alignment is guaranteed. The
 * size of the memory block is represented as a number of contiguous members
 * of an array each member of which is of a specified size in bytes.
 * @param nmemb is the desired number of members.
 * @param size is the desired number of bytes in each member.
 * @return pointer to the newly allocated memory block or 0 if fail.
 */
CXXCAPI void * calloc(size_t nmemb, size_t size);

/**
 * Release a previously allocated block of memory and allocate a new block.
 * @param ptr points to the previously allocated memory block to be freed.
 * @param size is the desired amount of memory to be reallocated in bytes.
 * @return pointer to the newly allocated memory block or 0 if fail.
 */
CXXCAPI void * realloc(void * ptr, size_t size);

#endif /* _COM_DIAG_AMIGO_HEAP_H_ */
