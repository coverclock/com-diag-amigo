/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include "com/diag/amigo/countof.h"
#include "com/diag/amigo/target/harvard.h"
#include "com/diag/amigo/target/GPIO.h"
#include "com/diag/amigo/target/Console.h"

namespace com {
namespace diag {
namespace amigo {

// Remarkably, some megaAVRs, notably the ATmega328p used in the Arduino Uno,
// start at PINB instead of PINA.
static volatile void * const BASE[] PROGMEM = {
#if defined(PINA)
	&PINA,
#else
	0,
#endif
#if defined(PINB)
	&PINB,
#else
	0,
#endif
#if defined(PINC)
	&PINC,
#else
	0,
#endif
#if defined(PIND)
	&PIND,
#else
	0,
#endif
#if defined(PINE)
	&PINE,
#else
	0,
#endif
#if defined(PINF)
	&PINF,
#else
	0,
#endif
#if defined(PING)
	&PING,
#else
	0,
#endif
#if defined(PINH)
	&PINH,
#else
	0,
#endif
#if defined(PINJ)
	&PINJ,
#else
	0,
#endif
#if defined(PINK)
	&PINK,
#else
	0,
#endif
#if defined(PINL)
	&PINL,
#else
	0,
#endif
};

bool GPIO::map(uint8_t pin, volatile void * & mybase, uint8_t & myoffset) {
	volatile void * base = 0;
	uint8_t index = pin / 8;
	if (index >= countof(BASE)) {
		// Do nothing.
	} else if ((base = reinterpret_cast<volatile void *>(pgm_read_word(&(BASE[index])))) == 0) {
		// Do nothing.
	} else {
		mybase = base;
		myoffset = pin % 8;
	}
	return (base != 0);
}

GPIO::GPIO(volatile void * mybase)
: gpiobase(mybase)
{}

GPIO::~GPIO() {}

}
}
}
