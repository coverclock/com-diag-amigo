/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by and cripped from wiring_analog.c and
 * pins_arduino.h by David A. Mellis and Mark Sproul.
 */

#include "com/diag/amigo/countof.h"
#include "com/diag/amigo/target/harvard.h"
#include "com/diag/amigo/target/PWM.h"

#define COM_DIAG_AMIGO_GPIO_TCCR		COM_DIAG_AMIGO_MMIO_8(controlbase, 0)

#define COM_DIAG_AMIGO_GPIO_OCR8		COM_DIAG_AMIGO_MMIO_8(outputcompare8base, 0)

#define COM_DIAG_AMIGO_GPIO_OCR16		COM_DIAG_AMIGO_MMIO_16(outputcompare16base, 0)

namespace com {
namespace diag {
namespace amigo {

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
		&TCCR0B,
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
		&TCCR1B,
#	else
		0,
#	endif
#	if defined(TCCR1C)
		&TCCR1C,
#	else
		0,
#	endif
#	if defined(TCCR1D)
		&TCCR1D,
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
		&TCCR2B,
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
		&TCCR3B,
#	else
		0,
#	endif
#	if defined(TCCR3C)
		&TCCR3C,
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
		&TCCR4B,
#	else
		0,
#	endif
#	if defined(TCCR4C)
		&TCCR4C,
#	else
		0,
#	endif
#	if defined(TCCR4D)
		&TCCR4D,
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
		&TCCR5B,
#	else
		0,
#	endif
#	if defined(TCCR5C)
		&TCCR5C,
#	else
		0,
#	endif
#else
		0,
		0,
		0,
#endif
};

static volatile void * const OUTPUTCOMPARE8[] PROGMEM = {
#if !defined(portUSE_TIMER0)
#	if defined(OCR0)
		&OCR0,
#	else
		0,
#	endif
#	if defined(OCR0A)
		&OCR0A,
#	else
		0,
#	endif
#	if defined(OCR0B)
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
#	if defined(OCR1A)
		&OCR1A,
#	else
		0,
#	endif
#	if defined(OCR1B)
		&OCR1B,
#	else
		0,
#	endif
#	if defined(OCR1C)
		&OCR1C,
#	else
		0,
#	endif
#	if defined(OCR1D)
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
#	if defined(OCR2)
		&OCR2,
#	else
		0,
#	endif
#	if defined(OCR2A)
		&OCR2A,
#	else
		0,
#	endif
#	if defined(OCR2B)
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
#	if defined(OCR4A)
		&OCR4A,
#	else
		0,
#	endif
#	if defined(OCR4B)
		&OCR4B,
#	else
		0,
#	endif
#	if defined(OCR4C)
		&OCR4C,
#	else
		0,
#	endif
#	if defined(OCR4D)
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

static const uint8_t ARDUINOPIN[] PROGMEM = {
#if defined(__AVR_ATmega2560__)
	// Gratefully adapted from variants/mega/pins_arduino.h in Arduino 1.0.
	PWM::INVALID,
	PWM::INVALID,
#if !defined(portUSE_TIMER3)
	PWM::PIN_3B,
	PWM::PIN_3C,
#else
	PWM::INVALID,
	PWM::INVALID,
#endif
#if !defined(portUSE_TIMER0)
	PWM::PIN_0B,
#else
	PWM::INVALID,
#endif
#if !defined(portUSE_TIMER3)
	PWM::PIN_3A,
#else
	PWM::INVALID,
#endif
#if !defined(portUSE_TIMER4)
	PWM::PIN_4A,
	PWM::PIN_4B,
	PWM::PIN_4C,
#else
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
#endif
#if !defined(portUSE_TIMER2)
	PWM::PIN_2B,
	PWM::PIN_2A,
#else
	PWM::INVALID,
	PWM::INVALID,
#endif
#if !defined(portUSE_TIMER1)
	PWM::PIN_1A,
	PWM::PIN_1B,
#else
	PWM::INVALID,
	PWM::INVALID,
#endif
#if !defined(portUSE_TIMER0)
	PWM::PIN_0A,
#else
	PWM::INVALID,
#endif
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
#if !defined(portUSE_TIMER5)
	PWM::PIN_5C,
	PWM::PIN_5B,
	PWM::PIN_5A,
#else
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
#endif
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
#elif defined(__AVR_ATmega328P__)
	// Gratefully adapted from variants/standard/pins_arduino.h in Arduino 1.0.
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
#if !defined(portUSE_TIMER2)
	PWM::PIN_2B,
#else
	PWM::INVALID,
#endif
	PWM::INVALID,
#if !defined(portUSE_TIMER0)
	PWM::PIN_0B,
	PWM::PIN_0A,
#else
	PWM::INVALID,
	PWM::INVALID,
#endif
	PWM::INVALID,
	PWM::INVALID,
#if !defined(portUSE_TIMER1)
	PWM::PIN_1A,
	PWM::PIN_1B,
#else
	PWM::INVALID,
	PWM::INVALID,
#endif
#if !defined(portUSE_TIMER2)
	PWM::PIN_2A,
#else
	PWM::INVALID,
#endif
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
	PWM::INVALID,
#else
#	error PWM must be modified for this microcontroller!
#endif
};

bool PWM::map(uint8_t pin, volatile void * & mycontrolbase, volatile void * & myoutputcompare8base, volatile void * & myoutputcompare16base, uint8_t & myoffset) {
	volatile void * controlbase = 0;
	volatile void * outputcompare8base = 0;
	volatile void * outputcompare16base = 0;
	uint8_t offset = ~0;
	if (pin >= countof(CONTROL)) {
		// Do nothing.
	} else if ((controlbase = reinterpret_cast<volatile void *>(pgm_read_word(&(CONTROL[pin])))) == 0) {
		// Do nothing.
	} if (pin >= countof(OUTPUTCOMPARE8)) {
		// Do nothing.
	} if (pin >= countof(OUTPUTCOMPARE16)) {
		// Do nothing.
	} else if (((outputcompare16base = reinterpret_cast<volatile void *>(pgm_read_word(&(OUTPUTCOMPARE16[pin])))) == 0) &&
	           ((outputcompare8base = reinterpret_cast<volatile void *>(pgm_read_word(&(OUTPUTCOMPARE8[pin])))) == 0)) {
		// Do nothing.
	} if (pin >= countof(OFFSET)) {
		// Do nothing.
	} else if ((offset = pgm_read_byte(&(OFFSET[pin]))) == static_cast<uint8_t>(~0)) {
		// Do nothing.
	} else {
		mycontrolbase = controlbase;
		myoutputcompare8base = outputcompare8base;
		myoutputcompare16base = outputcompare16base;
		myoffset = offset;
	}
	return (controlbase != 0);
}

GPIO::Pin PWM::pwm2gpio(PWM::Pin pin) {
	return (pin < countof(GPIOPIN)) ? static_cast<GPIO::Pin>(pgm_read_byte(&GPIOPIN[pin])) : GPIO::INVALID;
}

PWM::Pin PWM::arduino2pwm(uint8_t id) {
	return (id < countof(ARDUINOPIN)) ? static_cast<Pin>(pgm_read_byte(&ARDUINOPIN[id])) : INVALID;
}

void PWM::start(uint16_t dutycycle) {
	if (*this) {
		gpio.output(gpiomask);
		if (dutycycle == 0) {
			gpio.clear(gpiomask);
		} else if ((outputcompare16base != 0) && (dutycycle == 0xffff)) {
			gpio.set(gpiomask);
		} else if ((outputcompare8base != 0) && (dutycycle >= 0xff)) {
			gpio.set(gpiomask);
		} else {
			COM_DIAG_AMIGO_GPIO_TCCR |= pwmmask;
			if (outputcompare16base != 0) {
				COM_DIAG_AMIGO_GPIO_OCR16 = dutycycle;
			} else {
				COM_DIAG_AMIGO_GPIO_OCR8 = dutycycle;
			}
		}
	}
}

void PWM::stop(bool activelow) {
	if (*this) {
		COM_DIAG_AMIGO_GPIO_TCCR &= ~pwmmask;
		if (outputcompare16base != 0) {
			COM_DIAG_AMIGO_GPIO_OCR16 = 0;
		} else {
			COM_DIAG_AMIGO_GPIO_OCR8 = 0;
		}
		if (activelow) {
			gpio.set(gpiomask);
		} else {
			gpio.clear(gpiomask);
		}
	}
}

}
}
}
