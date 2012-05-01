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

#include "com/diag/amigo/target/Console.h"

namespace com {
namespace diag {
namespace amigo {

#define A2DCL		COM_DIAG_AMIGO_MMIO_8(adcbase, 0)
#define A2DCH		COM_DIAG_AMIGO_MMIO_8(adcbase, 1)
#define A2DCSRA		COM_DIAG_AMIGO_MMIO_8(adcbase, 2)
#define A2DCSRB		COM_DIAG_AMIGO_MMIO_8(adcbase, 3)
#define A2DMUX		COM_DIAG_AMIGO_MMIO_8(adcbase, 4)
#define A2DIDR2		COM_DIAG_AMIGO_MMIO_8(adcbase, 5) // Not present on, for example, the ATmega328p, but the address is reserved.
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
, errors(0)
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

A2D::~A2D() {
	if (converter < countof(a2d)) {
		Uninterruptible uninterruptible;
		A2DCSRA = 0;
		a2d[converter] = 0;
	}
}

void A2D::start(Trigger trigger, Divisor divisor) {
	uint8_t adts;
	uint8_t adate;
	switch (trigger) {

	default:
	case ON_DEMAND:
		adts = 0;
		break;

	case FREE_RUNNING:
		adts = 0;
		break;

	case ANALOG_COMPARATOR:
		adts = _BV(ADTS0);
		break;

	case EXTINT0:
		adts = _BV(ADTS1);
		break;

	case MATCH0A:
		adts = _BV(ADTS1) | _BV(ADTS0);
		break;

	case OVERFLOW0:
		adts = _BV(ADTS2);
		break;

	case MATCH1B:
		adts = _BV(ADTS2) | _BV(ADTS0);
		break;

	case OVERFLOW1:
		adts = _BV(ADTS2) | _BV(ADTS1);
		break;

	case CAPTURE1:
		adts = _BV(ADTS2) | _BV(ADTS1) | _BV(ADTS0);
		break;

	}
	adate = (trigger == ON_DEMAND) ? 0 : _BV(ADATE);

	uint8_t adps;
	switch (divisor) {

	case D2:
		adps = _BV(ADPS0);
		break;

	case D4:
		adps = _BV(ADPS1);
		break;

	case D8:
		adps = _BV(ADPS1) | _BV(ADPS0);
		break;

	case D16:
		adps = _BV(ADPS2);
		break;

	case D32:
		adps = _BV(ADPS2) | _BV(ADPS0);
		break;

	case D64:
		adps = _BV(ADPS2) | _BV(ADPS1);
		break;

	default:
	case D128:
		adps = _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
		break;

	}

	Uninterruptible uninterruptible;

	A2DCSRA = adate | adps | _BV(ADEN);
	A2DCSRB = adts;

	begin();
}

void A2D::stop() {
	Uninterruptible uninterruptible;
	A2DCSRA &= ~(_BV(ADEN) | _BV(ADIE));
}

void A2D::restart() {
	Uninterruptible uninterruptible;
	A2DCSRA |= _BV(ADEN);
	begin();
}

void A2D::begin() {
	Uninterruptible uninterruptible;
	uint8_t request;
	if ((A2DCSRA & (_BV(ADEN) | _BV(ADIE))) != _BV(ADEN)) {
		// Do nothing: stopped or busy.
	} else if (!requesting.receive(&request, IMMEDIATELY)) {
		// Do nothing: stalled.
	} else {
		adc(request);
	}
}

void A2D::adc(uint8_t request) {
	uint8_t reference = request >> 4;
	uint8_t channel = request & 0x0f;

#if defined(__AVR_ATmega32U4__)
	switch (channel) {
	case PIN_0:		channel = 7;	break;
	case PIN_1:		channel = 6;	break;
	case PIN_2:		channel = 5;	break;
	case PIN_3:		channel = 4;	break;
	case PIN_4:		channel = 1;	break;
	case PIN_5:		channel = 0;	break;
	case PIN_6:		channel = 8;	break;
	case PIN_7:		channel = 10;	break;
	case PIN_8:		channel = 11;	break;
	case PIN_9:		channel = 12;	break;
	case PIN_10:	channel = 13;	break;
	case PIN_11:	channel = 9;	break;
	default:		return;			break;
	}
#endif

	uint8_t refs;
	switch (reference) {

	case AREF:
#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
		refs = _BV(REFS0);
#else
		refs = 0;
#endif
		break;

	default:
	case AVCC:
#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
		refs = 0;
#else
		refs = _BV(REFS0);
#endif
		break;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	case V_1_1:
		refs = _BV(REFS1);
		break;

	case V_2_56:
		refs = _BV(REFS1) | _BV(REFS0);
		break;
#endif

	case V_INTERNAL:
#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
		refs = _BV(REFS1);
#else
		refs = _BV(REFS1) | _BV(REFS0);
#endif
		break;

	}

	A2DCSRB = (A2DCSRB & ~(1 << MUX5)) | (((channel >> 3) & 0x01) << MUX5);
	A2DMUX = refs | (channel & 0x07);
	A2DCSRA |= _BV(ADIE);
	A2DCSRA |= _BV(ADSC);
}

inline void A2D::complete(Converter converter) {
	// Only called from an ISR hence implicitly uninterruptible.
	if (a2d[converter] != 0) {
		a2d[converter]->complete();
	}
}

void A2D::complete() {
	uint8_t adcl = ADCL; // Must read ADCL, then ADCH.
	uint8_t adch = ADCH;
	uint16_t sample = (adch << 8) | adcl;

	bool woken = false;
	if (converted.sendFromISR(&sample, woken)) {
		// Do nothing.
	} else if (errors < ~static_cast<uint8_t>(0)) {
		++errors;
	} else {
		// Do nothing.
	}

	uint8_t request;
	if (requesting.receiveFromISR(&request)) {
		adc(request);
	} else {
		A2DCSRA &= ~_BV(ADIE);
	}

	if (woken) {
		Task::yield();
	}

}

A2D & A2D::operator=(uint8_t value) {
	// It is fun to think about why this has to be uninterruptible.
	Uninterruptible uninterruptible;
	errors = value;
	return *this;
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
