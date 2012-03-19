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

void * malloc(size_t size);

void free(void * ptr);

void * calloc(size_t nmemb, size_t size);

void * realloc(void * ptr, size_t size);

void * operator new(size_t size);

void operator delete(void * ptr);

void * operator new[](size_t size);

void operator delete[](void * ptr);

#endif /* _COM_DIAG_AMIGO_HEAP_H_ */
