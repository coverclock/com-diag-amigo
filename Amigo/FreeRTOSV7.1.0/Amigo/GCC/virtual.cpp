/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/target/Console.h"
#include "com/diag/amigo/target/Uninterruptable.h"

extern "C" void __cxa_pure_virtual() {
	com::diag::amigo::Uninterruptable uninterruptable;
	com::diag::amigo::Console console;
	console.start().write("__cxa_pure_virtual() called!\r\n").flush().stop();
	while (!0) {}
}
