/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include "com/diag/amigo/fatal.h"
#include "com/diag/amigo/byteorder.h"
#include "com/diag/amigo/target/watchdog.h"
#include "com/diag/amigo/target/Console.h"
#include "com/diag/amigo/target/Uninterruptible.h"

static void log(PGM_P label, PGM_P file, long line) {
	line = com::diag::amigo::byteorder::swapbytesif(line); // This just makes the hexadecimal line number more obvious.
	com::diag::amigo::Console::instance().start().write_P(PSTR("\r\n")).write_P(label).write_P(PSTR(": ")).write_P(file).write_P(PSTR("@0x")).dump(&line, sizeof(line)).write_P(PSTR("!\r\n")).flush().stop();
}

CXXCAPI void amigo_event(PGM_P file, long line) {
	com::diag::amigo::Uninterruptible uninterruptible;
	log(PSTR("EVENT"), file, line);
}

CXXCAPI void amigo_panic(PGM_P file, long line) {
	com::diag::amigo::Uninterruptible uninterruptible;
	log(PSTR("PANIC"), file, line);
#if defined(COM_DIAG_AMIGO_PANIC_USES_ENABLE)
	// ONLY ENABLE THIS MECHANISM IF YOU HAVE INSTALLED THE EXPERIMENTAL
	// STK500V2 BOOTLOADER THAT DISABLES THE WATCHDOG TIMER. OTHERWISE YOUR
	// MICROCONTROLLER WILL CONTINUIOUSLY RESET AND CAN ONLY BE RECOVERED WITH
	// A HARDWARE PROGRAMMER. THIS HAS BEEN TESTED WITH THE MODIFIED BOOTLOADER
	// AND VERIFIED TO WORK.
	// Attempt to get the watch dog timer to reset the system. This is different
	// from merely jumping to the reset vector, which does not reset all of
	// the hardware in the microcontroller.
	com::diag::amigo::watchdog::enable();
	// Eventually the watchdog timer will perform software reset of the target.
#	warning You MUST be using a bootloader that disables the watchdog timer!
#elif  defined(COM_DIAG_AMIGO_PANIC_USES_RESTART)
	// This just jumps to the reset vector. This is not a desirable mechanism.
	// Unlike the watchdog reset, it does not reset any of the hardware
	// including any of the I/O controllers. This leaves the system in a
	// questionable state.
	com::diag::amigo::watchdog::restart();
	// The function above should transfer control to the system reset vector.
#	warning This will restart the software without resetting the hardware!
#else
	// This is equivalent to amigo_fatal().
#endif
	while (!0) {}
}

CXXCAPI void amigo_fatal(PGM_P file, long line) {
	com::diag::amigo::Uninterruptible uninterruptible;
	log(PSTR("FATAL"), file, line);
	// This is a hard infinite loop with interrupts disabled.
	while (!0) {}
}

CXXCAPI void amigo_unexpected(long vector) {
	com::diag::amigo::Uninterruptible uninterruptible;
	log(PSTR("UNEXPECTED"), PSTR("vector"), vector);
}
