/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by and cribbed from wiring.c,
 * wiring_analog.c and pins_arduino.h by David A. Mellis and Mark Sproul.
 */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "FreeRTOS.h"
#include "task.h"
#include "com/diag/amigo/target/A2D.h"
#include "com/diag/amigo/target/Uninterruptible.h"
#include "com/diag/amigo/io.h"
#include "com/diag/amigo/Task.h"
#include "com/diag/amigo/countof.h"

namespace com {
namespace diag {
namespace amigo {

#define A2DCL		COM_DIAG_AMIGO_MMIO_8(adcbase, 0)
#define A2DCH		COM_DIAG_AMIGO_MMIO_8(adcbase, 1)
#define A2DCSRA		COM_DIAG_AMIGO_MMIO_8(adcbase, 2)
#define A2DCSRB		COM_DIAG_AMIGO_MMIO_8(adcbase, 3)
#define A2DMUX		COM_DIAG_AMIGO_MMIO_8(adcbase, 4)
#if defined(DIDR2)
// Not present on, for example, the ATmega328p, but the address is reserved.
#define A2DIDR2		COM_DIAG_AMIGO_MMIO_8(adcbase, 5)
#endif
#define A2DIDR0		COM_DIAG_AMIGO_MMIO_8(adcbase, 6)
#define A2DIDR1		COM_DIAG_AMIGO_MMIO_8(adcbase, 7)

/**
 * This table in SRAM will be filled in with the this pointer from a specific
 * A2D object when it is instantiated. Only one A2D object per controller should
 * be instantiated; otherwise it will overwrite the this pointer of any previous
 * A2D object for that same controller. (Yes, there is only one ADC, but
 * this follows the pattern used in other drivers.)
 */
static A2D * a2d[] = {
	0
};

/*******************************************************************************
 * MAP A2D PIN TO DISABLE REGISTER
 ******************************************************************************/

static volatile void * const DISABLE[] PROGMEM = {
#if defined(ADC3D) && defined(DIDR0)
	&DIDR0,
	&DIDR0,
	&DIDR0,
	&DIDR0,
#else
	0,
	0,
	0,
	0,
#endif
#if defined(ADC5D) && defined(DIDR0)
	&DIDR0,
	&DIDR0,
#else
	0,
	0,
#endif
#if defined(ADC6D) && defined(DIDR0)
	&DIDR0,
#else
	0,
#endif
#if defined(ADC7D) && defined(DIDR0)
	&DIDR0,
#else
	0,
#endif
#if defined(ADC10D) && defined(DIDR2)
	&DIDR2,
	&DIDR2,
	&DIDR2,
#else
	0,
	0,
	0,
#endif
#if defined(ADC11D) && defined(DIDR2)
	&DIDR2,
#else
	0,
#endif
#if defined(ADC13D) && defined(DIDR2)
	&DIDR2,
	&DIDR2,
#else
	0,
	0,
#endif
#if defined(ADC15D) && defined(DIDR2)
	&DIDR2,
	&DIDR2
#else
	0,
	0,
#endif
};

/*******************************************************************************
 * MAP A2D PIN TO BIT OFFSET
 ******************************************************************************/

static const uint8_t OFFSET[] PROGMEM = {
#if defined(ADC3D)
	ADC0D,
	ADC1D,
	ADC2D,
	ADC3D,
#else
	~0,
	~0,
	~0,
	~0,
#endif
#if defined(ADC5D)
	ADC4D,
	ADC5D,
#else
	~0,
	~0,
#endif
#if defined(ADC6D)
	ADC6D,
#else
	~0,
#endif
#if defined(ADC7D)
	ADC7D,
#else
	~0,
#endif
#if defined(ADC10D)
	ADC8D,
	ADC9D,
	ADC10D,
#else
	~0,
	~0,
	~0,
#endif
#if defined(ADC11D)
	ADC11D,
#else
	~0,
#endif
#if defined(ADC13D)
	ADC12D,
	ADC13D,
#else
	~0,
	~0,
#endif
#if defined(ADC15D)
	ADC14D,
	ADC15D,
#else
	~0,
	~0,
#endif
};

/*******************************************************************************
 * MAP A2D PIN TO GPIO PIN
 ******************************************************************************/

static const uint8_t GPIOPIN[] PROGMEM = {
#if defined(__AVR_ATmega2560__)
	GPIO::PIN_F0,	// PIN_0
	GPIO::PIN_F1,	// PIN_1
	GPIO::PIN_F2,	// PIN_2
	GPIO::PIN_F3,	// PIN_3
	GPIO::PIN_F4,	// PIN_4
	GPIO::PIN_F5,	// PIN_5
	GPIO::PIN_F6,	// PIN_6
	GPIO::PIN_F7,	// PIN_7
	GPIO::PIN_K0,	// PIN_8
	GPIO::PIN_K1,	// PIN_9
	GPIO::PIN_K2,	// PIN_10
	GPIO::PIN_K3,	// PIN_11
	GPIO::PIN_K4,	// PIN_12
	GPIO::PIN_K5,	// PIN_13
	GPIO::PIN_K6,	// PIN_14
	GPIO::PIN_K7,	// PIN_15
#elif defined(__AVR_ATmega328P__)
	GPIO::PIN_C0,	// PIN_0
	GPIO::PIN_C1,	// PIN_1
	GPIO::PIN_C2,	// PIN_2
	GPIO::PIN_C3,	// PIN_3
	GPIO::PIN_C4,	// PIN_4
	GPIO::PIN_C5,	// PIN_5
#else
#	error A2D must be modified for this microcontroller!
#endif
};

/*******************************************************************************
 * MAP ARDUINO PIN TO A2D PIN
 ******************************************************************************/

static const uint8_t ARDUINOPIN[] PROGMEM = {
#if defined(__AVR_ATmega2560__)
	// Gratefully adapted from variants/mega/pins_arduino.h in Arduino 1.0.
	A2D::INVALID,	// 0
	A2D::INVALID,	// 1
	A2D::INVALID,	// 2
	A2D::INVALID,	// 3
	A2D::INVALID,	// 4
	A2D::INVALID,	// 5
	A2D::INVALID,	// 6
	A2D::INVALID,	// 7
	A2D::INVALID,	// 8
	A2D::INVALID,	// 9
	A2D::INVALID,	// 10
	A2D::INVALID,	// 11
	A2D::INVALID,	// 12
	A2D::INVALID,	// 13
	A2D::INVALID,	// 14
	A2D::INVALID,	// 15
	A2D::INVALID,	// 16
	A2D::INVALID,	// 17
	A2D::INVALID,	// 18
	A2D::INVALID,	// 19
	A2D::INVALID,	// 20
	A2D::INVALID,	// 21
	A2D::INVALID,	// 22
	A2D::INVALID,	// 23
	A2D::INVALID,	// 24
	A2D::INVALID,	// 25
	A2D::INVALID,	// 26
	A2D::INVALID,	// 27
	A2D::INVALID,	// 28
	A2D::INVALID,	// 29
	A2D::INVALID,	// 30
	A2D::INVALID,	// 31
	A2D::INVALID,	// 32
	A2D::INVALID,	// 33
	A2D::INVALID,	// 34
	A2D::INVALID,	// 35
	A2D::INVALID,	// 36
	A2D::INVALID,	// 37
	A2D::INVALID,	// 38
	A2D::INVALID,	// 39
	A2D::INVALID,	// 40
	A2D::INVALID,	// 41
	A2D::INVALID,	// 42
	A2D::INVALID,	// 43
	A2D::INVALID,	// 44
	A2D::INVALID,	// 45
	A2D::INVALID,	// 46
	A2D::INVALID,	// 47
	A2D::INVALID,	// 48
	A2D::INVALID,	// 49
	A2D::INVALID,	// 50
	A2D::INVALID,	// 51
	A2D::INVALID,	// 52
	A2D::INVALID,	// 53
	A2D::PIN_0,		// 54
	A2D::PIN_1,		// 55
	A2D::PIN_2,		// 56
	A2D::PIN_3,		// 57
	A2D::PIN_4,		// 58
	A2D::PIN_5,		// 59
	A2D::PIN_6,		// 60
	A2D::PIN_7,		// 61
	A2D::PIN_8,		// 62
	A2D::PIN_9,		// 63
	A2D::PIN_10,	// 64
	A2D::PIN_11,	// 65
	A2D::PIN_12,	// 66
	A2D::PIN_13,	// 67
	A2D::PIN_14,	// 68
	A2D::PIN_15,	// 69
#elif defined(__AVR_ATmega328P__)
	// Gratefully adapted from variants/standard/pins_arduino.h in Arduino 1.0.
	A2D::INVALID,	// 0
	A2D::INVALID,	// 1
	A2D::INVALID,	// 2
	A2D::INVALID,	// 3
	A2D::INVALID,	// 4
	A2D::INVALID,	// 5
	A2D::INVALID,	// 6
	A2D::INVALID,	// 7
	A2D::INVALID,	// 8
	A2D::INVALID,	// 9
	A2D::INVALID,	// 10
	A2D::INVALID,	// 11
	A2D::INVALID,	// 12
	A2D::INVALID,	// 13
	A2D::PIN_0,		// 14
	A2D::PIN_1,		// 15
	A2D::PIN_2,		// 16
	A2D::PIN_3,		// 17
	A2D::PIN_4,		// 18
	A2D::PIN_5,		// 19
#else
#	error A2D must be modified for this microcontroller!
#endif
};

/*******************************************************************************
 * MAPPING CLASS METHODS
 ******************************************************************************/

volatile void * A2D::a2d2disable(Pin pin) {
	return (pin < countof(DISABLE)) ? reinterpret_cast<volatile void *>(pgm_read_word(&(DISABLE[pin]))) : 0;
}

uint8_t A2D::a2d2offset(Pin pin) {
	return (pin < countof(OFFSET)) ? pgm_read_byte(&(OFFSET[pin])) : ~0;
}

GPIO::Pin A2D::a2d2gpio(Pin pin) {
	return (pin < countof(GPIOPIN)) ? static_cast<GPIO::Pin>(pgm_read_byte(&GPIOPIN[pin])) : GPIO::INVALID;
}

A2D::Pin A2D::arduino2a2d(uint8_t number) {
	return (number < countof(ARDUINOPIN)) ? static_cast<Pin>(pgm_read_byte(&ARDUINOPIN[number])) : INVALID;
}

/*******************************************************************************
 * INSTANCE METHODS
 ******************************************************************************/

A2D::A2D(Converter myconverter, size_t requests, size_t conversions)
: adcbase(0)
, converted(conversions)
, requesting(requests)
, converter(myconverter)
{
	if (converter >= countof(a2d)) {
		// FAIL!
		return;
	}

	a2d[converter] = this;

	switch (converter) {

	default:
	case CONVERTER0:
		adcbase = &ADCL;
		break;

	}
}

inline void A2D::complete(Converter converter) {
	// Only called from an ISR hence implicitly uninterruptible.
	if (a2d[converter] != 0) {
		a2d[converter]->complete();
	}
}

void A2D::complete() {
}

}
}
}

// These are the ISR routines which are jumped to from an ISR vector in low
// memory. Note that the class methods are inlined, so there is really only
// two levels of indirection: a jump from the vector to this routine, then an
// inline class method call (which doesn't count because it's inline), then
// a call to the instance method that actually implements the ISR for a
// specific instance of the SPI object. Note that these functions have C
// linkage.

extern "C" {

ISR(ADC_vect) {
	com::diag::amigo::A2D::complete(com::diag::amigo::A2D::CONVERTER0);
}

}
