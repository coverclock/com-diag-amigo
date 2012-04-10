#ifndef _COM_DIAG_AMIGO_MEGAAVR_COUNTOF_H_
#define _COM_DIAG_AMIGO_MEGAAVR_COUNTOF_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/cxxcapi.h"

/**
 *  @def    countof(_ARRAY_)
 *
 *  Generates the number of elements in the array @a _ARRAY_ whose single
 *  dimension is known at compile time.
 */
#define countof(_ARRAY_) CXXCSTATICCAST(size_t, sizeof(_ARRAY_) / sizeof(_ARRAY_[0]))

#endif /* _COM_DIAG_AMIGO_MEGAAVR_COUNTOF_H_ */
