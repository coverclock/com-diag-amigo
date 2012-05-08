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
#include "com/diag/amigo/types.h"

/**
 * Disable the watch dog timer.
 * @return the prior value of the MCUSR indicating the prior RESET reason.
 */
CXXCAPI uint8_t amigo_watchdog_disable(void);

// The function below basically just jumps to the reset vector at address 0x0
// in program (flash) memory. But it doesn't quite work right either. Definitely
// a work in progress.

#if defined(COM_DIAG_AMIGO_WATCHDOG_RESTART)

/**
 * Transfer control to the system reset vector. This is a function from which
 * there is no return. This doesn't actually use the hardware watch dog timer.
 */
CXXCAPI void amigo_watchdog_restart(void);

#endif

// I don't believe there's actually any way to safely use the AVR hardware
// watch dog timer (WDT) with the standard Arduino bootloader. If the system
// is reset via the WDT, the WDT is still enabled, but with the shortest
// prescaler (zero), when it comes out of Watch Dog Reset. A prescaler of zero
// allows for about 16ms before the WDT fires again; it must be disabled within
// this time to prevent another watch dog reset from occurring. But the Arduino
// bootloader has a time out of about a second (depending on the model) before
// it transfers control to the application. This results in a "rolling reset".
// I haven't been able to interrupt this even by power cycling the board (?!);
// I had to resort to reflashing the board using the AVRISP mkII in-system
// programmer device. Searching the web after this debacle, I found several
// folks saying the same thing. The fix is to modify the Arduino bootloader
// to disable the WDT very early in its own initialization.

#if defined(COM_DIAG_AMIGO_WATCHDOG_ENABLE)

/**
 * Enable the hardware watch dog timer. If not periodically patted, the hardware
 * watch dog will reset the target. This is a good way to deliberately reset
 * the target. The underlying implementation currently sets the watch dog
 * timeout to about eight seconds which is the maximum.
 */
CXXCAPI void amigo_watchdog_enable(void);

#endif

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
 * Disable the watch dog timer.
 */
inline uint8_t disable() {
	return amigo_watchdog_disable();
}

#if defined(COM_DIAG_AMIGO_WATCHDOG_RESTART)

/**
 * Transfer control to the system reset vector. This is a function from which
 * there is no return. This doesn't actually use the hardware watch dog timer.
 */
inline void restart() {
	amigo_watchdog_restart();
}

#endif

#if defined(COM_DIAG_AMIGO_WATCHDOG_ENABLE)

/**
 * Enable the hardware watch dog timer. If not periodically patted, the hardware
 * watch dog will reset the target. This is a good way to deliberately reset
 * the target. The underlying implementation currently sets the watch dog
 * timeout to about eight seconds which is the maximum.
 */
inline void enable() {
	amigo_watchdog_enable();
}

#endif

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
