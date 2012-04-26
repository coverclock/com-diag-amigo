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
	GPIO::PIN_E0,	// 0
	GPIO::PIN_E1,	// 1
	GPIO::PIN_E4,	// 2
	GPIO::PIN_E5,	// 3
	GPIO::PIN_G5,	// 4
	GPIO::PIN_E3,	// 5
	GPIO::PIN_H3,	// 6
	GPIO::PIN_H4,	// 7
	GPIO::PIN_H5,	// 8
	GPIO::PIN_H6,	// 9
	GPIO::PIN_B4,	// 10
	GPIO::PIN_B5,	// 11
	GPIO::PIN_B6,	// 12
	GPIO::PIN_B7,	// 13
	GPIO::PIN_J1,	// 14
	GPIO::PIN_J0,	// 15
	GPIO::PIN_H1,	// 16
	GPIO::PIN_H0,	// 17
	GPIO::PIN_D3,	// 18
	GPIO::PIN_D2,	// 19
	GPIO::PIN_D1,	// 20
	GPIO::PIN_D0,	// 21
	GPIO::PIN_A0,	// 22
	GPIO::PIN_A1,	// 23
	GPIO::PIN_A2,	// 24
	GPIO::PIN_A3,	// 25
	GPIO::PIN_A4,	// 26
	GPIO::PIN_A5,	// 27
	GPIO::PIN_A6,	// 28
	GPIO::PIN_A7,	// 29
	GPIO::PIN_C7,	// 30
	GPIO::PIN_C6,	// 31
	GPIO::PIN_C5,	// 32
	GPIO::PIN_C4,	// 33
	GPIO::PIN_C3,	// 34
	GPIO::PIN_C2,	// 35
	GPIO::PIN_C1,	// 36
	GPIO::PIN_C0,	// 37
	GPIO::PIN_D7,	// 38
	GPIO::PIN_G2,	// 39
	GPIO::PIN_G1,	// 40
	GPIO::PIN_G0,	// 41
	GPIO::PIN_L7,	// 42
	GPIO::PIN_L6,	// 43
	GPIO::PIN_L5,	// 44
	GPIO::PIN_L4,	// 45
	GPIO::PIN_L3,	// 46
	GPIO::PIN_L2,	// 47
	GPIO::PIN_L1,	// 48
	GPIO::PIN_L0,	// 49
	GPIO::PIN_B3,	// 50
	GPIO::PIN_B2,	// 51
	GPIO::PIN_B1,	// 52
	GPIO::PIN_B0,	// 53
	GPIO::PIN_F0,	// 54
	GPIO::PIN_F1,	// 55
	GPIO::PIN_F2,	// 56
	GPIO::PIN_F3,	// 57
	GPIO::PIN_F4,	// 58
	GPIO::PIN_F5,	// 59
	GPIO::PIN_F6,	// 60
	GPIO::PIN_F7,	// 61
	GPIO::PIN_K0,	// 62
	GPIO::PIN_K1,	// 63
	GPIO::PIN_K2,	// 64
	GPIO::PIN_K3,	// 65
	GPIO::PIN_K4,	// 66
	GPIO::PIN_K5,	// 67
	GPIO::PIN_K6,	// 68
	GPIO::PIN_K7,	// 69
#elif defined(__AVR_ATmega328P__)
	// Gratefully adapted from variants/standard/pins_arduino.h in Arduino 1.0.
	GPIO::PIN_D0,	// 0
	GPIO::PIN_D1,	// 1
	GPIO::PIN_D2,	// 2
	GPIO::PIN_D3,	// 3
	GPIO::PIN_D4,	// 4
	GPIO::PIN_D5,	// 5
	GPIO::PIN_D6,	// 6
	GPIO::PIN_D7,	// 7
	GPIO::PIN_B0,	// 8
	GPIO::PIN_B1,	// 9
	GPIO::PIN_B2,	// 10
	GPIO::PIN_B3,	// 11
	GPIO::PIN_B4,	// 12
	GPIO::PIN_B5,	// 13
	GPIO::PIN_C0,	// 14
	GPIO::PIN_C1,	// 15
	GPIO::PIN_C2,	// 16
	GPIO::PIN_C3,	// 17
	GPIO::PIN_C4,	// 18
	GPIO::PIN_C5,	// 19
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

GPIO::Pin GPIO::arduino2gpio(uint8_t number) {
	return (number < countof(ARDUINOPIN)) ? static_cast<Pin>(pgm_read_byte(&ARDUINOPIN[number])) : INVALID;
}

}
}
}
