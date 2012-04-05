/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/fatal.h"
#include "com/diag/amigo/littleendian.h"
#include "com/diag/amigo/target/Console.h"
#include "com/diag/amigo/target/Uninterruptable.h"

CXXCAPI void amigo_fatal(const char * file, int line) {
	com::diag::amigo::Uninterruptable uninterruptable;
	if (com::diag::amigo::littleendian()) {
		// This just makes the hexadecimal line number more obvious.
		line = ((line >> 8) & 0xff) | ((line & 0xff) << 8);
	}
	com::diag::amigo::Console::instance().start().write("FATAL: ").write(file).write("@0x").dump(&line, sizeof(line)).write("!\r\n").flush().stop();
	while (!0) {}
}