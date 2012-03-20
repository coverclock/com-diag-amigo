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
#define _COM_DIAG_AMIGO_BEOS_
#endif
#include <stddef.h>
#if defined(_COM_DIAG_AMIGO_BEOS_)
#undef _COM_DIAG_AMIGO_BEOS_
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

typedef uint8_t Register;

}
}
}

#endif /* _COM_DIAG_AMIGO_TYPES_H_ */
