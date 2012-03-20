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
#define __BEOS__
#define _COM_DIAG_AMIGO_UNDEF_BEOS_
#endif
#if !defined(__need_NULL)
#define __need_NULL
#define _COM_DIAG_AMIGO_UNDEF_NEED_NULL_
#endif
#if !defined(__need_size_t)
#define __need_size_t
#define _COM_DIAG_AMIGO_UNDEF_NEED_SIZE_T_
#endif
#include <stddef.h>
#if defined(_COM_DIAG_AMIGO_UNDEF_NEED_SIZE_T_)
#undef _COM_DIAG_AMIGO_UNDEF_NEED_SIZE_T_
#undef __need_size_t
#endif
#if defined(_COM_DIAG_AMIGO_UNDEF_NEED_NULL_)
#undef _COM_DIAG_AMIGO_UNDEF_NEED_NULL_
#undef __BEOS__
#endif
#if defined(_COM_DIAG_AMIGO_UNDEF_BEOS_)
#undef _COM_DIAG_AMIGO_UNDEF_BEOS_
#undef __BEOS__
#endif
#include <stdint.h>
#include "projdefs.h"
#include "portable.h"

namespace com {
namespace diag {
namespace amigo {

typedef portTickType Ticks;

typedef unsigned portBASE_TYPE Count;

typedef unsigned portBASE_TYPE Size;

typedef uint8_t Register;

}
}
}

#endif /* _COM_DIAG_AMIGO_TYPES_H_ */
