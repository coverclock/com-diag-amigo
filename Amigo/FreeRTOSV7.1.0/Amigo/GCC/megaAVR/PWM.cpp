/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by and cribbed from wiring.c,
 * wiring_analog.c, and pins_arduino.h by David A. Mellis and Mark Sproul.
 */

#include "com/diag/amigo/countof.h"
#include "com/diag/amigo/target/harvard.h"
#include "com/diag/amigo/target/PWM.h"

#define TCCR		COM_DIAG_AMIGO_MMIO_8(controlbase, 0)
#define OCR8		COM_DIAG_AMIGO_MMIO_8(outputcompare8base, 0)
#define OCR16		COM_DIAG_AMIGO_MMIO_16(outputcompare16base, 0)

namespace com {
namespace diag {
namespace amigo {

/*******************************************************************************
 * MAP PWM PIN TO CONTROL REGISTER
 ******************************************************************************/

static volatile void * const CONTROL[] PROGMEM = {
#if !defined(portUSE_TIMER0)
#	if defined(TCCR0)
		&TCCR0,
#	else
		0,
#	endif
#	if defined(TCCR0A)
		&TCCR0A,
#	else
		0,
#	endif
#	if defined(TCCR0B)
		&TCCR0A,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
#endif
#if !defined(portUSE_TIMER1)
#	if defined(TCCR1A)
		&TCCR1A,
#	else
		0,
#	endif
#	if defined(TCCR1B)
		&TCCR1A,
#	else
		0,
#	endif
#	if defined(TCCR1C)
		&TCCR1A,
#	else
		0,
#	endif
#	if defined(TCCR1D)
		&TCCR1A,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
		0,
#endif
#if !defined(portUSE_TIMER2)
#	if defined(TCCR2)
		&TCCR2,
#	else
		0,
#	endif
#	if defined(TCCR2A)
		&TCCR2A,
#	else
		0,
#	endif
#	if defined(TCCR2B)
		&TCCR2A,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
#endif
#if !defined(portUSE_TIMER3)
#	if defined(TCCR3A)
		&TCCR3A,
#	else
		0,
#	endif
#	if defined(TCCR3B)
		&TCCR3A,
#	else
		0,
#	endif
#	if defined(TCCR3C)
		&TCCR3A,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
#endif
#if !defined(portUSE_TIMER4)
#	if defined(TCCR4A)
		&TCCR4A,
#	else
		0,
#	endif
#	if defined(TCCR4B)
		&TCCR4A,
#	else
		0,
#	endif
#	if defined(TCCR4C)
		&TCCR4A,
#	else
		0,
#	endif
#	if defined(TCCR4D)
		&TCCR4A,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
		0,
#endif
#if !defined(portUSE_TIMER5)
#	if defined(TCCR5A)
		&TCCR5A,
#	else
		0,
#	endif
#	if defined(TCCR5B)
		&TCCR5A,
#	else
		0,
#	endif
#	if defined(TCCR5C)
		&TCCR5A,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
#endif
};

/*******************************************************************************
 * MAP PWM PIN TO EIGHT-BIT OUTPUT COMPARE REGISTER
 ******************************************************************************/

static volatile void * const OUTPUTCOMPARE8[] PROGMEM = {
#if !defined(portUSE_TIMER0)
#	if defined(OCR0) && !defined(OCR0L)
		&OCR0,
#	else
		0,
#	endif
#	if defined(OCR0A) && !defined(OCR0AL)
		&OCR0A,
#	else
		0,
#	endif
#	if defined(OCR0B) && !defined(OCR0BL)
		&OCR0B,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
#endif
#if !defined(portUSE_TIMER1)
#	if defined(OCR1A) && !defined(OCR1AL)
		&OCR1A,
#	else
		0,
#	endif
#	if defined(OCR1B) && !defined(OCR1BL)
		&OCR1B,
#	else
		0,
#	endif
#	if defined(OCR1C) && !defined(OCR1CL)
		&OCR1C,
#	else
		0,
#	endif
#	if defined(OCR1D) && !defined(OCR1DL)
		&OCR1D,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
		0,
#endif
#if !defined(portUSE_TIMER2)
#	if defined(OCR2) && !defined(OCR2L)
		&OCR2,
#	else
		0,
#	endif
#	if defined(OCR2A) && !defined(OCR2AL)
		&OCR2A,
#	else
		0,
#	endif
#	if defined(OCR2B) && !defined(OCR2BL)
		&OCR2B,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
#endif
		0,
		0,
		0,
#if !defined(portUSE_TIMER4)
#	if defined(OCR4A) && !defined(OCR4AL)
		&OCR4A,
#	else
		0,
#	endif
#	if defined(OCR4B) && !defined(OCR4BL)
		&OCR4B,
#	else
		0,
#	endif
#	if defined(OCR4C) && !defined(OCR4CL)
		&OCR4C,
#	else
		0,
#	endif
#	if defined(OCR4D) && !defined(OCR4DL)
		&OCR4D,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
		0,
#endif
		0,
		0,
		0,
};

/*******************************************************************************
 * MAP PWM PIN TO SIXTEEN-BIT OUTPUT COMPARE REGISTER
 ******************************************************************************/

static volatile void * const OUTPUTCOMPARE16[] PROGMEM = {
#if !defined(portUSE_TIMER0)
		0,
#	if defined(OCR0AL)
		&OCR0A,
#	else
		0,
#	endif
#	if defined(OCR0BL)
		&OCR0B,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
#endif
#if !defined(portUSE_TIMER1)
#	if defined(OCR1AL)
		&OCR1A,
#	else
		0,
#	endif
#	if defined(OCR1BL)
		&OCR1B,
#	else
		0,
#	endif
#	if defined(OCR1CL)
		&OCR1C,
#	else
		0,
#	endif
#	if defined(OCR1DL)
		&OCR1D,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
		0,
#endif
		0,
		0,
		0,
#if !defined(portUSE_TIMER3)
#	if defined(OCR3AL)
		&OCR3A,
#	else
		0,
#	endif
#	if defined(OCR3BL)
		&OCR3B,
#	else
		0,
#	endif
#	if defined(OCR3CL)
		&OCR3C,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
#endif
#if !defined(portUSE_TIMER4)
#	if defined(OCR4AL)
		&OCR4A,
#	else
		0,
#	endif
#	if defined(OCR4BL)
		&OCR4B,
#	else
		0,
#	endif
#	if defined(OCR4CL)
		&OCR4C,
#	else
		0,
#	endif
		0,
#else
		0,
		0,
		0,
		0,
#endif
#if !defined(portUSE_TIMER5)
#	if defined(OCR5AL)
		&OCR5A,
#	else
		0,
#	endif
#	if defined(OCR5BL)
		&OCR5B,
#	else
		0,
#	endif
#	if defined(OCR5CL)
		&OCR5C,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
#endif
};

/*******************************************************************************
 * MAP PWM PIN TO BIT OFFSET
 ******************************************************************************/

static const uint8_t OFFSET[] PROGMEM = {
#if !defined(portUSE_TIMER0)
#	if defined(COM00)
		COM00,
#	else
		~0,
#	endif
#	if defined(COM0A1)
		COM0A1,
#	else
		~0,
#	endif
#	if defined(COM0B1)
		COM0B1,
#	else
		~0,
#	endif
#else
		~0,
		~0,
		~0,
#endif
#if !defined(portUSE_TIMER1)
#	if defined(COM1A1)
		COM1A1,
#	else
		~0,
#	endif
#	if defined(COM1B1)
		COM1B1,
#	else
		~0,
#	endif
#	if defined(COM1C1)
		COM1C1,
#	else
		~0,
#	endif
#	if defined(COM1D1)
		COM1D1,
#	else
		~0,
#	endif
#else
		~0,
		~0,
		~0,
		~0,
#endif
#if !defined(portUSE_TIMER2)
#	if defined(COM21)
		COM21,
#	else
		~0,
#	endif
#	if defined(COM2A1)
		COM2A1,
#	else
		~0,
#	endif
#	if defined(COM2B1)
		COM2B1,
#	else
		~0,
#	endif
#else
		~0,
		~0,
		~0,
#endif
#if !defined(portUSE_TIMER3)
#	if defined(COM3A1)
		COM3A1,
#	else
		~0,
#	endif
#	if defined(COM3B1)
		COM3B1,
#	else
		~0,
#	endif
#	if defined(COM3C1)
		COM3C1,
#	else
		~0,
#	endif
#else
		~0,
		~0,
		~0,
#endif
#if !defined(portUSE_TIMER4)
#	if defined(COM4A1)
		COM4A1,
#	else
		~0,
#	endif
#	if defined(COM4B1)
		COM4B1,
#	else
		~0,
#	endif
#	if defined(COM4C1)
		COM4C1,
#	else
		~0,
#	endif
#	if defined(COM4D1)
		COM4D1,
#	else
		~0,
#	endif
#else
		~0,
		~0,
		~0,
		~0,
#endif
#if !defined(portUSE_TIMER5)
#	if defined(COM5A1)
		COM5A1,
#	else
		~0,
#	endif
#	if defined(COM5B1)
		COM5B1,
#	else
		~0,
#	endif
#	if defined(COM5C1)
		COM5C1,
#	else
		~0,
#	endif
#else
		~0,
		~0,
		~0,
#endif
};

/*******************************************************************************
 * MAP PWM PIN TO GPIO PIN
 ******************************************************************************/

static const uint8_t GPIOPIN[] PROGMEM = {
#if defined(__AVR_ATmega2560__)
	GPIO::INVALID,	// PIN_0
	GPIO::PIN_B7,	// PIN_0A
	GPIO::PIN_G5,	// PIN_0B
	GPIO::PIN_B5,	// PIN_1A
	GPIO::PIN_B6,	// PIN_1B
	GPIO::PIN_B7,	// PIN_1C
	GPIO::INVALID,	// PIN_1D
	GPIO::INVALID,	// PIN_2
	GPIO::PIN_B4,	// PIN_2A
	GPIO::PIN_H6,	// PIN_2B
	GPIO::PIN_E3,	// PIN_3A
	GPIO::PIN_E4,	// PIN_3B
	GPIO::PIN_E5,	// PIN_3C
	GPIO::PIN_H3,	// PIN_4A
	GPIO::PIN_H4,	// PIN_4B
	GPIO::PIN_H5,	// PIN_4C
	GPIO::INVALID,	// PIN_4D
	GPIO::PIN_L3,	// PIN_5A
	GPIO::PIN_L4,	// PIN_5B
	GPIO::PIN_L5,	// PIN_5C
#elif defined(__AVR_ATmega328P__)
	GPIO::INVALID,	// PIN_0
	GPIO::PIN_D6,	// PIN_0A
	GPIO::PIN_D5,	// PIN_0B
	GPIO::PIN_B1,	// PIN_1A
	GPIO::PIN_B2,	// PIN_1B
	GPIO::INVALID,	// PIN_1C
	GPIO::INVALID,	// PIN_1C
	GPIO::INVALID,	// PIN_2
	GPIO::PIN_B3,	// PIN_2A
	GPIO::PIN_D3,	// PIN_2B
	GPIO::INVALID,	// PIN_3A
	GPIO::INVALID,	// PIN_3B
	GPIO::INVALID,	// PIN_3C
	GPIO::INVALID,	// PIN_4A
	GPIO::INVALID,	// PIN_4B
	GPIO::INVALID,	// PIN_4C
	GPIO::INVALID,	// PIN_4D
	GPIO::INVALID,	// PIN_5A
	GPIO::INVALID,	// PIN_5B
	GPIO::INVALID,	// PIN_5C
#else
#	error PWM must be modified for this microcontroller!
#endif
};

/*******************************************************************************
 * MAP ARDUINO DIGITAL PIN TO PWM PIN
 ******************************************************************************/

static const uint8_t ARDUINOPIN[] PROGMEM = {
#if defined(__AVR_ATmega2560__)
	// Gratefully adapted from variants/mega/pins_arduino.h in Arduino 1.0.
	PWM::INVALID,   // 0
	PWM::INVALID,   // 1
#if !defined(portUSE_TIMER3)
	PWM::PIN_3B,    // 2
	PWM::PIN_3C,    // 3
#else
	PWM::INVALID,   // 2
	PWM::INVALID,   // 3
#endif
#if !defined(portUSE_TIMER0)
	PWM::PIN_0B,    // 4
#else
	PWM::INVALID,   // 4
#endif
#if !defined(portUSE_TIMER3)
	PWM::PIN_3A,    // 5
#else
	PWM::INVALID,   // 5
#endif
#if !defined(portUSE_TIMER4)
	PWM::PIN_4A,    // 6
	PWM::PIN_4B,    // 7
	PWM::PIN_4C,    // 8
#else
	PWM::INVALID,   // 6
	PWM::INVALID,   // 7
	PWM::INVALID,   // 8
#endif
#if !defined(portUSE_TIMER2)
	PWM::PIN_2B,    // 9
	PWM::PIN_2A,    // 10
#else
	PWM::INVALID,   // 9
	PWM::INVALID,   // 10
#endif
#if !defined(portUSE_TIMER1)
	PWM::PIN_1A,    // 11
	PWM::PIN_1B,    // 12
#else
	PWM::INVALID,   // 11
	PWM::INVALID,   // 12
#endif
#if !defined(portUSE_TIMER0)
	PWM::PIN_0A,    // 13
#else
	PWM::INVALID,   // 13
#endif
	PWM::INVALID,	// 14
	PWM::INVALID,	// 15
	PWM::INVALID,	// 16
	PWM::INVALID,	// 17
	PWM::INVALID,	// 18
	PWM::INVALID,	// 19
	PWM::INVALID,	// 20
	PWM::INVALID,	// 21
	PWM::INVALID,	// 22
	PWM::INVALID,	// 23
	PWM::INVALID,	// 24
	PWM::INVALID,	// 25
	PWM::INVALID,	// 26
	PWM::INVALID,	// 27
	PWM::INVALID,	// 28
	PWM::INVALID,	// 29
	PWM::INVALID,	// 30
	PWM::INVALID,	// 31
	PWM::INVALID,	// 32
	PWM::INVALID,	// 33
	PWM::INVALID,	// 34
	PWM::INVALID,	// 35
	PWM::INVALID,	// 36
	PWM::INVALID,	// 37
	PWM::INVALID,	// 38
	PWM::INVALID,	// 39
	PWM::INVALID,	// 40
	PWM::INVALID,	// 41
	PWM::INVALID,	// 42
	PWM::INVALID,	// 43
#if !defined(portUSE_TIMER5)
	PWM::PIN_5C,    // 44
	PWM::PIN_5B,    // 45
	PWM::PIN_5A,    // 46
#else
	PWM::INVALID,   // 44
	PWM::INVALID,   // 45
	PWM::INVALID,   // 46
#endif
	PWM::INVALID,	// 47
	PWM::INVALID,	// 48
	PWM::INVALID,	// 49
	PWM::INVALID,	// 50
	PWM::INVALID,	// 51
	PWM::INVALID,	// 52
	PWM::INVALID,	// 53
	PWM::INVALID,	// 54
	PWM::INVALID,	// 55
	PWM::INVALID,	// 56
	PWM::INVALID,	// 57
	PWM::INVALID,	// 58
	PWM::INVALID,	// 59
	PWM::INVALID,	// 60
	PWM::INVALID,	// 61
	PWM::INVALID,	// 62
	PWM::INVALID,	// 63
	PWM::INVALID,	// 64
	PWM::INVALID,	// 65
	PWM::INVALID,	// 66
	PWM::INVALID,	// 67
	PWM::INVALID,	// 68
	PWM::INVALID,	// 69
#elif defined(__AVR_ATmega328P__)
	// Gratefully adapted from variants/standard/pins_arduino.h in Arduino 1.0.
	PWM::INVALID,   // 0
	PWM::INVALID,   // 1
	PWM::INVALID,   // 2
#if !defined(portUSE_TIMER2)
	PWM::PIN_2B,    // 3
#else
	PWM::INVALID,   // 3
#endif
	PWM::INVALID,   // 4
#if !defined(portUSE_TIMER0)
	PWM::PIN_0B,    // 5
	PWM::PIN_0A,    // 6
#else
	PWM::INVALID,   // 5
	PWM::INVALID,   // 6
#endif
	PWM::INVALID,   // 7
	PWM::INVALID,   // 8
#if !defined(portUSE_TIMER1)
	PWM::PIN_1A,    // 9
	PWM::PIN_1B,    // 10
#else
	PWM::INVALID,   // 9
	PWM::INVALID,   // 10
#endif
#if !defined(portUSE_TIMER2)
	PWM::PIN_2A,    // 11
#else
	PWM::INVALID,   // 11
#endif
	PWM::INVALID,   // 12
	PWM::INVALID,   // 13
	PWM::INVALID,   // 14
	PWM::INVALID,   // 15
	PWM::INVALID,   // 16
	PWM::INVALID,   // 17
	PWM::INVALID,   // 18
	PWM::INVALID,   // 19
#else
#	error PWM must be modified for this microcontroller!
#endif
};

/*******************************************************************************
 * MAP PWM PIN TO TIMER
 ******************************************************************************/

static const uint8_t TIMER[] PROGMEM = {
#if !defined(portUSE_TIMER0)
#	if defined(TCCR0)
		PWM::TIMER_0,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR0A)
		PWM::TIMER_0,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR0B)
		PWM::TIMER_0,
#	else
		PWM::TIMER_0,
#	endif
#else
		PWM::NONE,
		PWM::NONE,
		PWM::NONE,
#endif
#if !defined(portUSE_TIMER1)
#	if defined(TCCR1A)
		PWM::TIMER_1,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR1B)
		PWM::TIMER_1,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR1C)
		PWM::TIMER_1,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR1D)
		PWM::TIMER_1,
#	else
		PWM::NONE,
#	endif
#else
		PWM::NONE,
		PWM::NONE,
		PWM::NONE,
		PWM::NONE,
#endif
#if !defined(portUSE_TIMER2)
#	if defined(TCCR2)
		PWM::TIMER_2,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR2A)
		PWM::TIMER_2,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR2B)
		PWM::TIMER_2,
#	else
		PWM::NONE,
#	endif
#else
		PWM::NONE,
		PWM::NONE,
		PWM::NONE,
#endif
#if !defined(portUSE_TIMER3)
#	if defined(TCCR3A)
		PWM::TIMER_3,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR3B)
		PWM::TIMER_3,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR3C)
		PWM::TIMER_3,
#	else
		PWM::NONE,
#	endif
#else
		PWM::NONE,
		PWM::NONE,
		PWM::NONE,
#endif
#if !defined(portUSE_TIMER4)
#	if defined(TCCR4A)
		PWM::TIMER_4,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR4B)
		PWM::TIMER_4,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR4C)
		PWM::TIMER_4,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR4D)
		PWM::TIMER_4,
#	else
		PWM::NONE,
#	endif
#else
		PWM::NONE,
		PWM::NONE,
		PWM::NONE,
		PWM::NONE,
#endif
#if !defined(portUSE_TIMER5)
#	if defined(TCCR5A)
		PWM::TIMER_5,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR5B)
		PWM::TIMER_5,
#	else
		PWM::NONE,
#	endif
#	if defined(TCCR5C)
		PWM::TIMER_5,
#	else
		PWM::NONE,
#	endif
#else
		PWM::NONE,
		PWM::NONE,
		PWM::NONE,
#endif
};

/*******************************************************************************
 * MAPPING CLASS METHODS
 ******************************************************************************/

volatile void * PWM::pwm2control(Pin pin) {
	return (pin < countof(CONTROL)) ? reinterpret_cast<volatile void *>(pgm_read_word(&(CONTROL[pin]))) : 0;
}

volatile void * PWM::pwm2outputcompare8(Pin pin) {
	return (pin < countof(OUTPUTCOMPARE8)) ? reinterpret_cast<volatile void *>(pgm_read_word(&(OUTPUTCOMPARE8[pin]))) : 0;
}

volatile void * PWM::pwm2outputcompare16(Pin pin) {
	return (pin < countof(OUTPUTCOMPARE16)) ? reinterpret_cast<volatile void *>(pgm_read_word(&(OUTPUTCOMPARE16[pin]))) : 0;
}

uint8_t PWM::pwm2offset(Pin pin) {
	return (pin < countof(OFFSET)) ? pgm_read_byte(&(OFFSET[pin])) : ~0;
}

GPIO::Pin PWM::pwm2gpio(Pin pin) {
	return (pin < countof(GPIOPIN)) ? static_cast<GPIO::Pin>(pgm_read_byte(&GPIOPIN[pin])) : GPIO::INVALID;
}

PWM::Pin PWM::arduino2pwm(uint8_t number) {
	return (number < countof(ARDUINOPIN)) ? static_cast<Pin>(pgm_read_byte(&ARDUINOPIN[number])) : INVALID;
}

PWM::Timer PWM::pwm2timer(Pin pin) {
	return (pin < countof(TIMER)) ? static_cast<Timer>(pgm_read_byte(&TIMER[pin])) : NONE;
}

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/

PWM::Timer PWM::prestart(Timer timer) {
	Timer result = timer;

	// Arduino initializes every available timer up front in its init()
	// function. Amigo requires the application to do this, and only configures
	// the the timer specified (or in the instance method version, the timer
	// associated with the object's pin). I would like to have specified the
	// prescale factor as an argument and then have it be a default. But this
	// code borrowed from Arduino 1.0 supports a lot of platforms that I do not
	// have and hence cannot test.

	switch (timer) {

#if !defined(portUSE_TIMER0)
	case TIMER_0:
#	if defined(TCCR0A) && defined(WGM01) && defined(WGM00)
		TCCR0A = _BV(WGM01) | _BV(WGM00);	// Put timer 0 in 8-bit fast PWM mode.
#	endif
#	if defined(__AVR_ATmega128__)
		TCCR0 = _BV(CS02);					// Set timer 0 prescale factor to 64.
#	elif defined(TCCR0) && defined(CS01) && defined(CS00)
		TCCR0 = _BV(CS01) | _BV(CS00);		// Set timer 0 prescale factor to 64.
#	elif defined(TCCR0B) && defined(CS01) && defined(CS00)
		TCCR0B = _BV(CS01) | _BV(CS00);		// Set timer 0 prescale factor to 64.
#	elif defined(TCCR0A) && defined(CS01) && defined(CS00)
		TCCR0A |= _BV(CS01) | _BV(CS00);	// Set timer 0 prescale factor to 64.
#	else
		result = NONE;
#	endif
		break;
#endif

#if !defined(portUSE_TIMER1)
	case TIMER_1:
#	if defined(TCCR1B) && defined(CS11) && defined(CS10)
#		if F_CPU >= 8000000L
		TCCR1B = _BV(CS11) | _BV(CS10);		// Set timer 1 prescale factor to 64.
#		else
		TCCR1B = _BV(CS11);					// Set timer 1 prescale factor to 8.
#		endif
#	elif defined(TCCR1) && defined(CS11) && defined(CS10)
#		if F_CPU >= 8000000L
		TCCR1 = _BV(CS11) | _BV(CS10);		// Set timer 1 prescale factor to 64.
#		else
		TCCR1 = _BV(CS11);					// Set timer 1 prescale factor to 8.
#		endif
#	else
		result = NONE;
#	endif
#	if defined(TCCR1A) && defined(WGM10)
		TCCR1A = _BV(WGM10);				// Put timer 1 in 8-bit phase correct PWM mode.
#	endif
		break;
#endif

#if !defined(portUSE_TIMER2)
	case TIMER_2:
#	if defined(TCCR2) && defined(CS22)
		TCCR2 = _BV(CS22);					// Set timer 2 prescale factor to 64.
#	elif defined(TCCR2B) && defined(CS22)
		TCCR2B = _BV(CS22);					// Set timer 2 prescale factor to 64.
#	else
		result = NONE;
#	endif
#	if defined(TCCR2) && defined(WGM20)
		TCCR2 = _BV(WGM20);					// Put timer 2 in 8-bit phase correct PWM mode.
#	elif defined(TCCR2A) && defined(WGM20)
		TCCR2A = _BV(WGM20);				// Put timer 2 in 8-bit phase correct PWM mode.
#	else
		result = NONE;
#	endif
		break;
#endif

#if !defined(portUSE_TIMER3)
	case TIMER_3:
#	if defined(TCCR3B) && defined(CS31) && defined(WGM30)
		TCCR3B = _BV(CS31) | _BV(CS30);		// Set timer 3 prescale factor to 64.
		TCCR3A = _BV(WGM30);				// Put timer 3 in 8-bit phase correct PWM mode.
#	else
		result = NONE;
#	endif
		break;
#endif

#if !defined(portUSE_TIMER4)
	case TIMER_4:
#	if defined(TCCR4B) && defined(CS41) && defined(WGM40)
		TCCR4B = _BV(CS41) | _BV(CS40);		// Set timer 4 prescale factor to 64.
		TCCR4A = _BV(WGM40);				// Put timer 4 in 8-bit phase correct PWM mode.
#	else
		result = NONE;
#	endif
		break;
#	endif

#if !defined(portUSE_TIMER5)
	case TIMER_5:
#	if defined(TCCR5B) && defined(CS51) && defined(WGM50)
		TCCR5B = _BV(CS51) | _BV(CS50);		// Set timer 5 prescale factor to 64.
		TCCR5A = _BV(WGM50);				// Put timer 5 in 8-bit phase correct PWM mode.
#	else
		result = NONE;
#	endif
		break;
#endif

	case NONE:
		break;

	default:
		result = NONE;
		break;

	}

	// It should be impossible this to fail unless this code is somehow
	// wrong, the Pin enumeration is wrong, the TIMER table is wrong, the
	// <io*.h> header file provided by AVR libc for this microcontroller is
	// wrong, or the -mmcu command line argument is wrong.

	return result;
}

/*******************************************************************************
 * INSTANCE METHODS
 ******************************************************************************/

void PWM::start(uint8_t dutycycle) {
	if (!*this) {
		// FAIL!
		return;
	}

	gpio.output(gpiomask);

	if (outputcompare16base != 0) {
		// (0% < duty cycle < 100%) so we generate a outverted square wave...
		TCCR |= pwmmask;
		// ... using eight-bits out of the sixteen-bit resolution.
		OCR16 = dutycycle;
	} else if (outputcompare8base != 0) {
		// (0% < duty cycle < 100%) so we generate a outverted square wave...
		TCCR |= pwmmask;
		// ... using eight-bit resolution.
		OCR8 = dutycycle;
	} else {
		// Should be impossible.
	}
}

void PWM::stop(bool high) {
	if (!*this) {
		// FAIL!
		return;
	}

	if (outputcompare16base != 0) {
		TCCR &= ~pwmmask;
		OCR16 = 0;
	} else if (outputcompare8base != 0) {
		TCCR &= ~pwmmask;
		OCR8 = 0;
	} else {
		// Should be impossible.
	}

	if (high) {
		gpio.set(gpiomask);
	} else {
		gpio.clear(gpiomask);
	}
}

}
}
}
