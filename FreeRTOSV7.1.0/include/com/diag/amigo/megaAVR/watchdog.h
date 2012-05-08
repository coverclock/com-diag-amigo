#ifndef _COM_DIAG_AMIGO_MEGAAVR_WATCHDOG_H_
#define _COM_DIAG_AMIGO_MEGAAVR_WATCHDOG_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/cxxcapi.h"

/**
 * Enable the hardware watch dog timer. If not periodically patted, the hardware
 * watch dog will reset the target. This is a good way to deliberately reset
 * the target. The underlying implementation currently sets the watch dog
 * timeout to about eight seconds which is the maximum.
 */
CXXCAPI void amigo_watchdog_enable(void);

/**
 * Disable the watch dog timer.
 */
CXXCAPI void amigo_watchdog_disable(void);

/**
 * Pat the watch dog timer by resetting the timer.
 */
CXXCAPI void amigo_watchdog_reset(void);

#if defined(__cplusplus)

namespace com {
namespace diag {
namespace amigo {
namespace watchdog {

/**
 * Enable the hardware watch dog timer. If not periodically patted, the hardware
 * watch dog will reset the target. This is a good way to deliberately reset
 * the target. The underlying implementation currently sets the watch dog
 * timeout to about eight seconds which is the maximum.
 */
inline void enable() {
	amigo_watchdog_enable();
}

/**
 * Disable the watch dog timer.
 */
inline void disable() {
	amigo_watchdog_disable();
}

/**
 * Pat the watch dog timer by resetting the timer.
 */
inline void reset() {
	amigo_watchdog_reset();
}

}
}
}
}

#endif

#endif /* _COM_DIAG_AMIGO_MEGAAVR_WATCHDOG_H_ */
