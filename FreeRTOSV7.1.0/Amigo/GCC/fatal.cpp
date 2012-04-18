/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/fatal.h"
#include "com/diag/amigo/byteorder.h"
#include "com/diag/amigo/target/Console.h"
#include "com/diag/amigo/target/Uninterruptible.h"

CXXCAPI void amigo_fatal(PGM_P file, int line) {
	com::diag::amigo::Uninterruptible uninterruptible;
	line = com::diag::amigo::htons(line); // This just makes the hexadecimal line number more obvious.
	com::diag::amigo::Console::instance().start().write_P(PSTR("\r\nFATAL: ")).write_P(file).write_P(PSTR("@0x")).dump(&line, sizeof(line)).write_P(PSTR("!\r\n")).flush().stop();
	// This is a hard infinite loop with interrupts disabled. We aren't going
	// anywhere once we get here.
	while (!0) {}
}
