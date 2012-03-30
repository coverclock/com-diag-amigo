#ifndef _COM_DIAG_AMIGO_IO_H_
#define _COM_DIAG_AMIGO_IO_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/cxxcapi.h"

#define AMIGO_MMIO_8(_BASE_, _OFFSET_)  (*CXXCSTATICCAST(volatile uint8_t *,CXXCSTATICCAST(volatile uint8_t *, (_BASE_))  + (_OFFSET_)))

#endif /* _COM_DIAG_AMIGO_IO_H_ */
