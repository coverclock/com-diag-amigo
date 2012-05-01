/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/interrupt.h>
#include "com/diag/amigo/fatal.h"

// Placing this in the low text section seems to be occasionally necessary.
// Once in a while I'll get a link error about the linker not being able
// to generate a jump to the default __bad_interrupt function because it is
// too far away.
CXXCAPI void __vector_default(void) __attribute__ ((signal, section (".lowtext"), __INTR_ATTRS));
CXXCAPI void __vector_default(void) {
	// I'd really like to display the vector from whence we came. But
	// experiments in unwinding the stack frame to reach the program counter
	// for the RETI have thus far not been successful; it doesn't seem like it
	// should be that hard.
	com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
}
