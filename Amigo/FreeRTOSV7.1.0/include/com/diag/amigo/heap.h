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
 * @param size is the desired amount of memory in bytes. This is in support
 * of the C++ new operator.
 * @return a pointer to the newly allocated memory block or 0 if fail.
 */
void * operator new(size_t size);

/**
 * Initialize a block of memory that has already been allocated. This is in
 * support of the C++ placement new operator.
 * @param size is the desired amount of memory in bytes.
 * @paran ptr points to the already allocated block of memory.
 * @return a pointer to the newly allocated memory block or 0 if fail.
 */
void * operator new(size_t size, void * ptr);

/**
 * Return a block of memory to the heap. This is in support of the C++
 * placement delete operator
 * @param ptr points to the previously newed memory block to be deleted.
 */
void operator delete(void * ptr);

// I've seen blog comments suggesting there is a placement delete, and
// the C++ <new> header suggests that there is. But Stroustrup says no!

/**
 * Allocate a block of memory from the heap. No alignment is guaranteed. The
 * memory block represents an array. This is in support of the C++ new[]
 * operator.
 * @param size is the desired total amount of memory in bytes.
 * @return pointer to the newly allocated memory block or 0 if fail.
 */
void * operator new[](size_t size);

/**
 * Return a block of memory to the heap. The memory block represents an array.
 * This is in support of the C++ delete[] operator.
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

// We don't implement realloc() because it has to know the internal structure
// of the allocated blocks so that it can get the size of the block being
// released in order to copy data from the old block to the new block. That
// obviously could be done but would require realloc() to know about the
// internal structure of FreeRTOS than I'm comfortable with for a routine that
// I seldom if ever have used.

#endif /* _COM_DIAG_AMIGO_HEAP_H_ */
