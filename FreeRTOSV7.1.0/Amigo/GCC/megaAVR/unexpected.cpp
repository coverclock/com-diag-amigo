/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/target/Console.h"

void amigo_unexpected() {
	// I'd really like to display the vector from whence we came. But
	// experiments in unwinding the stack frame to reach the program counter
	// for the RETI have thus far not been successful; it doesn't seem like it
	// would be that hard.
	com::diag::amigo::Console::instance().start().write_P(PSTR("\r\nUNEXPECTED!\r\n")).flush().stop();
	while (!0) {}
}

// placing this in the low text section seems to be occasionally necessary.
// Once in a while I'll get a link error about the linker not being able
// to generate a jump to the default __bad_interrupt function because it is
// too far away.
extern "C" void __vector_default(void) __attribute__ ((signal, section (".lowtext"), __INTR_ATTRS));

extern "C" void __vector_default(void) {
	amigo_unexpected();
}
