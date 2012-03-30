#ifndef _COM_DIAG_AMIGO_VALUES_H_
#define _COM_DIAG_AMIGO_VALUES_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"

#if defined(__cplusplus)
namespace com {
namespace diag {
namespace amigo {
#endif

static const Ticks IMMEDIATELY = 0;

static const Ticks NEVER = ~IMMEDIATELY;

static const Count NONE = 0;

static const Count UNLIMITED = ~NONE;

#if defined(__cplusplus)
}
}
}
#endif

#endif /* _COM_DIAG_AMIGO_VALUES_H_ */
