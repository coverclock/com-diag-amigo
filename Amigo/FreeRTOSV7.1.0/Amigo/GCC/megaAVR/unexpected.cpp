/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/interrupt.h>
#include "com/diag/amigo/cxxcapi.h"
#include "com/diag/amigo/fatal.h"

// Placing this in the low text section seems to be occasionally necessary.
// Once in a while I'll get a link error about the linker not being able
// to generate a jump to the default __bad_interrupt function because it is
// too far away.
CXXCAPI void __vector_default(void) __attribute__ ((signal, section (".lowtext"), __INTR_ATTRS));
CXXCAPI void __vector_default(void) {
	// I'd really like to print the vector we came through to get here, but I
	// don't see a way to do that. The PC on the stack is that of the code
	// that got the spurious interrupt. AFAICT it would require a separate
	// function like this one for every single unused vector.
	com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
}
