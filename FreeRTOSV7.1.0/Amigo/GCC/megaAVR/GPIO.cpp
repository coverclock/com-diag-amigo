/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/countof.h"
#include "com/diag/amigo/target/harvard.h"
#include "com/diag/amigo/target/GPIO.h"

namespace com {
namespace diag {
namespace amigo {

/*******************************************************************************
 * MAP GPIO PIN TO REGISTER BASE
 ******************************************************************************/

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

/*******************************************************************************
 * MAP ARDUINO PIN TO GPIO PIN
 ******************************************************************************/

static const uint8_t ARDUINOPIN[] PROGMEM = {
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

/*******************************************************************************
 * MAPPING CLASS METHODS
 ******************************************************************************/

volatile void * GPIO::gpio2base(Pin pin) {
	uint8_t index = pin / 8;
	return (index < countof(BASE)) ? reinterpret_cast<volatile void *>(pgm_read_word(&(BASE[index]))) : 0;
}

uint8_t GPIO::gpio2offset(Pin pin) {
	uint8_t index = pin / 8;
	return (index < countof(BASE)) ? (pin % 8) : ~0;
}

GPIO::Pin GPIO::arduino2gpio(uint8_t id) {
	return (id < countof(ARDUINOPIN)) ? static_cast<Pin>(pgm_read_byte(&ARDUINOPIN[id])) : INVALID;
}

}
}
}
