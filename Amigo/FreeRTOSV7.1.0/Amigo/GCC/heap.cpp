/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * Note that these functions are in the global name space.\n
 */

#include <FreeRTOS.h>
#include "com/diag/amigo/heap.h"

extern "C" void * malloc(size_t size) {
	return pvPortMalloc(size);
}

extern "C" void free(void * ptr) {
	vPortFree(ptr);
}

extern "C" void * calloc(size_t nmemb, size_t size) {
	return pvPortMalloc(nmemb * size);
}

extern "C" void * realloc(void * ptr, size_t size) {
	vPortFree(ptr);
	return pvPortMalloc(size);
}

void * operator new(size_t size) {
	return pvPortMalloc(size);
}

void operator delete(void * ptr) {
	vPortFree(ptr);
}

void * operator new[](size_t size) {
	return pvPortMalloc(size);
}

void operator delete[](void * ptr) {
	vPortFree(ptr);
}
