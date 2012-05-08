/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include "com/diag/amigo/target/watchdog.h"
#include "com/diag/amigo/target/Uninterruptible.h"

CXXCAPI void amigo_watchdog_enable(void) {
	com::diag::amigo::Uninterruptible uninterruptible;
	// Note that the AVR libc function expects all four prescaler bits to be
	// contiguous, like they are portrayed in the data sheet table, not split
	// one and three as they actually are in the control register.
	//
	// Reference: Atmel, 8-bit Atmel Microcontroller, ATmega2560,
	//            2549N-AVR-05/11, 12.5.2, p. 67
	//
	// This nominally gives us about eight seconds before the WDT reset. That
	// gives the ATmega2560 to reset, the bootloader to time out and restart
	// our application, and the main program to disable the WDT.
	wdt_enable(WDTO_8S);
}

CXXCAPI uint8_t amigo_watchdog_disable(void) {
	com::diag::amigo::Uninterruptible uninterruptible;
	wdt_reset();
	uint8_t reason = MCUSR;
	MCUSR = 0;
	wdt_disable();
	return reason;
}

CXXCAPI void amigo_watchdog_reset(void) {
	com::diag::amigo::Uninterruptible uninterruptible;
	wdt_reset();
}
