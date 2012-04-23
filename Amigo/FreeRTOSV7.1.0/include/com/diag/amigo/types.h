#ifndef _COM_DIAG_AMIGO_TYPES_H_
#define _COM_DIAG_AMIGO_TYPES_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This header file may define preprocessor symbols, and C and C++ symbols
 * that are in the global namespace.
 */

#include <stddef.h>
#include <stdint.h>

#if 0
	/*
	 * Change 0 to 1 if ssize_t is already defined elsewhere. There is no
	 * standard portable way the preprocessor can know this.
	 */
#elif !defined(___SIZEOF_SIZE_T__)
	typedef unsigned int ssize_t;
#elif (__SIZEOF_SIZE_T__ == __SIZEOF_INT__)
	typedef signed int ssize_t;
#elif (__SIZEOF_SIZE_T__ == __SIZEOF_LONG__)
	typedef signed long ssize_t;
#else
	typedef signed int ssize_t;
#endif

#if defined(NULL)
	/* NULL is already defined. */
#elif defined(__cplusplus)
	/**
	 * @def NULL
	 *
	 * NULL is the value of the null pointer.
	 */
#	define NULL (0)
#else
	/**
	 * @def NULL
	 *
	 * NULL is the value of the null pointer.
	 */
#	define NULL ((void*)0)
#endif

#include "com/diag/amigo/configuration.h"

#if defined(__cplusplus)
namespace com {
namespace diag {
namespace amigo {
#endif

/**
 * Define the type used to hold time duration in units of system ticks.
 */
typedef portTickType ticks_t;

#if defined(__cplusplus)
}
}
}
#endif

#endif /* _COM_DIAG_AMIGO_TYPES_H_ */
