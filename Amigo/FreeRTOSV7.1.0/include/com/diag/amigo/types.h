#ifndef _COM_DIAG_AMIGO_TYPES_H_
#define _COM_DIAG_AMIGO_TYPES_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#if !defined(__BEOS__)
#	define __BEOS__
#	define _COM_DIAG_AMIGO_UNDEF_BEOS_
#endif
#if !defined(__need_NULL)
#	define __need_NULL
#	define _COM_DIAG_AMIGO_UNDEF_NEED_NULL_
#endif
#if !defined(__need_size_t)
#	define __need_size_t
#	define _COM_DIAG_AMIGO_UNDEF_NEED_SIZE_T_
#endif

#include <stddef.h>

#if defined(_COM_DIAG_AMIGO_UNDEF_NEED_SIZE_T_)
#	undef _COM_DIAG_AMIGO_UNDEF_NEED_SIZE_T_
#	undef __need_size_t
#endif
#if defined(_COM_DIAG_AMIGO_UNDEF_NEED_NULL_)
#	undef _COM_DIAG_AMIGO_UNDEF_NEED_NULL_
#	undef __BEOS__
#endif
#if defined(_COM_DIAG_AMIGO_UNDEF_BEOS_)
#	undef _COM_DIAG_AMIGO_UNDEF_BEOS_
#	undef __BEOS__
#endif

#if !defined(NULL)
    /*
     * I have recently been disabused of the notion that NULL is now
     * a pre-defined symbol in later versions of the GCC compiler.
     */
#   if defined(__cplusplus)
        /**
         * @def NULL
         *
         * NULL is the value of the null pointer. The use of this manifest
         * constant can be avoided in C++, but may be necessary when
         * writing header files that may be included in either C or C++
         * translation units.
         */
#       define NULL (0)
#   else
        /**
         * @def NULL
         *
         * NULL is the value of the null pointer. The use of this manifest
         * constant can be avoided in C++, but may be necessary when
         * writing header files that may be included in either C or C++
         * translation units.
         */
#       define NULL ((void*)0)
#   endif
#endif

#include <stdint.h>
#include "projdefs.h"
#include "FreeRTOSConfig.h"
#include "portable.h"

#if defined(__cplusplus)
namespace com {
namespace diag {
namespace amigo {
#endif

/**
 * Define the type used to hold time duration in units of Ticks.
 */
typedef portTickType Ticks;

/**
 * Define the type used to hold a cardinal value that is the count of something.
 */
typedef unsigned portBASE_TYPE Count;

/**
 * Define the type used to hold the cardinal value that is the size of something
 * in bytes. Note that this is not the same as size_t. In this port, the
 * FreeRTOS portBASE_TYPE is an int which is two bytes, while size_t is an
 * unsigned long, which is four bytes.
 */
typedef unsigned portBASE_TYPE Size;

#if defined(__cplusplus)
}
}
}
#endif

#endif /* _COM_DIAG_AMIGO_TYPES_H_ */
