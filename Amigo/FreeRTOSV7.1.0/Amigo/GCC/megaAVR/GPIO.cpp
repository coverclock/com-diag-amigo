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

// It seems like making this array type Pin instead of uint8_t would simplify
// matters. But it doesn't. We'd have to decide at run time (although the
// compiler might optimize the extra code away) whether to use pgm_read_byte(),
// pgm_read_word(), or pgm_read_dword(), and then cast the result to Pin anyway.
// So instead we fix the type to uint8_t secure in the knowledge that the result
// will never be more than 255, which not coincidentally also happens to be the
// value of INVALID. BTW, sizeof(Pin) is one, according to the sizeof() unit
// test.
// The contents of this array are determined by conditional compilation based
// on a preprocessor symbol provided by the compiler based on its -mcu command
// line option. Note that in the Arduino IDE, these things are decided by the
// user (that would be you) setting the Board option in the Tools menu. I only
// provide support for those microcontrollers on the boards with which I have
// access to test. __AVR_ATmega2560__ and __AVR_ATmega328P__ covers the
// Arduino Mega, Arduino Mega ADK, and Arduino Uno, all of which I have, and
// clones which use the same microcontrollers. Support for other alternatives
// might be derived from the Arduino 1.0 code, from which I cribbed the
// configurations below.
static const uint8_t PIN[] PROGMEM = {
#if defined(__AVR_ATmega2560__)
	// Gratefully adapted from variants/mega/pins_arduino.h in Arduino 1.0.
	GPIO::PIN_E0,
	GPIO::PIN_E1,
	GPIO::PIN_E4,
	GPIO::PIN_E5,
	GPIO::PIN_G5,
	GPIO::PIN_E3,
	GPIO::PIN_H3,
	GPIO::PIN_H4,
	GPIO::PIN_H5,
	GPIO::PIN_H6,
	GPIO::PIN_B4,
	GPIO::PIN_B5,
	GPIO::PIN_B6,
	GPIO::PIN_B7,
	GPIO::PIN_J1,
	GPIO::PIN_J0,
	GPIO::PIN_H1,
	GPIO::PIN_H0,
	GPIO::PIN_D3,
	GPIO::PIN_D2,
	GPIO::PIN_D1,
	GPIO::PIN_D0,
	GPIO::PIN_A0,
	GPIO::PIN_A1,
	GPIO::PIN_A2,
	GPIO::PIN_A3,
	GPIO::PIN_A4,
	GPIO::PIN_A5,
	GPIO::PIN_A6,
	GPIO::PIN_A7,
	GPIO::PIN_C7,
	GPIO::PIN_C6,
	GPIO::PIN_C5,
	GPIO::PIN_C4,
	GPIO::PIN_C3,
	GPIO::PIN_C2,
	GPIO::PIN_C1,
	GPIO::PIN_C0,
	GPIO::PIN_D7,
	GPIO::PIN_G2,
	GPIO::PIN_G1,
	GPIO::PIN_G0,
	GPIO::PIN_L7,
	GPIO::PIN_L6,
	GPIO::PIN_L5,
	GPIO::PIN_L4,
	GPIO::PIN_L3,
	GPIO::PIN_L2,
	GPIO::PIN_L1,
	GPIO::PIN_L0,
	GPIO::PIN_B3,
	GPIO::PIN_B2,
	GPIO::PIN_B1,
	GPIO::PIN_B0,
	GPIO::PIN_F0,
	GPIO::PIN_F1,
	GPIO::PIN_F2,
	GPIO::PIN_F3,
	GPIO::PIN_F4,
	GPIO::PIN_F5,
	GPIO::PIN_F6,
	GPIO::PIN_F7,
	GPIO::PIN_K0,
	GPIO::PIN_K1,
	GPIO::PIN_K2,
	GPIO::PIN_K3,
	GPIO::PIN_K4,
	GPIO::PIN_K5,
	GPIO::PIN_K6,
	GPIO::PIN_K7,
#elif defined(__AVR_ATmega328P__)
	// Gratefully adapted from variants/standard/pins_arduino.h in Arduino 1.0.
	GPIO::PIN_D0,
	GPIO::PIN_D1,
	GPIO::PIN_D2,
	GPIO::PIN_D3,
	GPIO::PIN_D4,
	GPIO::PIN_D5,
	GPIO::PIN_D6,
	GPIO::PIN_D7,
	GPIO::PIN_B0,
	GPIO::PIN_B1,
	GPIO::PIN_B2,
	GPIO::PIN_B3,
	GPIO::PIN_B4,
	GPIO::PIN_B5,
	GPIO::PIN_C0,
	GPIO::PIN_C1,
	GPIO::PIN_C2,
	GPIO::PIN_C3,
	GPIO::PIN_C4,
	GPIO::PIN_C5,
#else
#	error GPIO must be modified for this microcontroller!
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


GPIO::Pin GPIO::arduino(uint8_t id) {
	return (id < countof(PIN)) ? static_cast<Pin>(pgm_read_byte(&PIN[id])) : INVALID;
}

}
}
}
