#ifndef _COM_DIAG_AMIGO_TYPES_H_
#define _COM_DIAG_AMIGO_TYPES_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * These symbols are in the global name space!\n
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

#endif /* _COM_DIAG_AMIGO_TYPES_H_ */
