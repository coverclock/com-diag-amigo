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

static void log(PGM_P label, PGM_P file, int line) {
	line = com::diag::amigo::byteorder::swapbytesif(line); // This just makes the hexadecimal line number more obvious.
	com::diag::amigo::Console::instance().start().write_P(PSTR("\r\n")).write_P(label).write_P(PSTR(": ")).write_P(file).write_P(PSTR("@0x")).dump(&line, sizeof(line)).write_P(PSTR("!\r\n")).flush().stop();
}

CXXCAPI void amigo_event(PGM_P file, int line) {
	com::diag::amigo::Uninterruptible uninterruptible;
	log(PSTR("EVENT"), file, line);
}

CXXCAPI void amigo_panic(PGM_P file, int line) {
	com::diag::amigo::Uninterruptible uninterruptible;
	log(PSTR("PANIC"), file, line);
	// Attempt to get the watch dog timer to reset the system. This is different
	// from merely branching to the reset vector, which does not reset all of
	// the hardware in the processor.
	com::diag::amigo::watchdog::enable();
	// Eventually the watch dog timer will perform software reset of the target.
	// If it doesn't, this is equivalent to amigo_fatal().
	while (!0) {}
}

CXXCAPI void amigo_fatal(PGM_P file, int line) {
	com::diag::amigo::Uninterruptible uninterruptible;
	log(PSTR("FATAL"), file, line);
	// This is a hard infinite loop with interrupts disabled.
	while (!0) {}
}
