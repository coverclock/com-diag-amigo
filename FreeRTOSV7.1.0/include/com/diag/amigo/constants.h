#ifndef _COM_DIAG_AMIGO_CONSTANTS_H_
#define _COM_DIAG_AMIGO_CONSTANTS_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * Defines the timeout value in ticks that causes the application to never
 * block waiting for ring buffer space or data, but instead be returned an
 * error.
 */
static const ticks_t IMMEDIATELY = 0;

/**
 * Defines the timeout value in ticks that causes the application to block
 * indefinitely waiting for ring buffer space or data. The FreeRTOS
 * timeout mechanism actually checks for this value specifically if task
 * suspension is enabled, so it does not just mean the maximum amount of
 * time that can be specified.
 */
static const ticks_t NEVER = portMAX_DELAY; // Nominally ~0.

}
}
}

#endif /* _COM_DIAG_AMIGO_CONSTANTS_H_ */
