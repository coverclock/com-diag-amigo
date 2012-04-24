#ifndef _COM_DIAG_AMIGO_MEGAAVR_PWM_H_
#define _COM_DIAG_AMIGO_MEGAAVR_PWM_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by and cribbed from wiring.c,
 * wiring_analog.c and pins_arduino.h by David A. Mellis and Mark Sproul.
 */

#include <avr/io.h>
#include "com/diag/amigo/target/GPIO.h"
#include "com/diag/amigo/configuration.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * PWM provides support for Pulse Width Modulation on those hardware pins
 * that support it. Pins whose hardware timer/counter conflict with the
 * hardware timer/counter used to provide FreeRTOS with a system tick are
 * not supported and their enumerated values are not defined. (Meaning: if you
 * try to use pins that can't be used on your FreeRTOS configuration, your
 * code won't compile).
 */
class PWM {

public:

	enum Pin {
#if !defined(portUSE_TIMER0)
#	if defined(TCCR0) && defined(COM00)
		PIN_0 = 0,
#	endif
#	if defined(TCCR0A) && defined(COM0A1)
		PIN_0A = 1,
#	endif
#	if defined(TCCR0B) && defined(COM0B1)
		PIN_0B = 2,
#	endif
#endif
#if !defined(portUSE_TIMER1)
#	if defined(TCCR1A) && defined(COM1A1)
		PIN_1A = 3,
#	endif
#	if defined(TCCR1B) && defined(COM1B1)
		PIN_1B = 4,
#	endif
#	if defined(TCCR1C) && defined(COM1C1)
		PIN_1C = 5,
#	endif
#	if defined(TCCR1D) && defined(COM1D1)
		PIN_1D = 6,
#	endif
#endif
#if !defined(portUSE_TIMER2)
#	if defined(TCCR2) && defined(COM21)
		PIN_2 = 7,
#	endif
#	if defined(TCCR2A) && defined(COM2A1)
		PIN_2A = 8,
#	endif
#	if defined(TCCR2B) && defined(COM2B1)
		PIN_2B = 9,
#	endif
#endif
#if !defined(portUSE_TIMER3)
#	if defined(TCCR3A) && defined(COM3A1)
		PIN_3A = 10,
#	endif
#	if defined(TCCR3B) && defined(COM3B1)
		PIN_3B = 11,
#	endif
#	if defined(TCCR3C) && defined(COM3C1)
		PIN_3C = 12,
#	endif
#endif
#if !defined(portUSE_TIMER4)
#	if defined(TCCR4A) && defined(COM4A1)
		PIN_4A = 13,
#	endif
#	if defined(TCCR4B) && defined(COM4B1)
		PIN_4B = 14,
#	endif
#	if defined(TCCR4C) && defined(COM4C1)
		PIN_4C = 15,
#	endif
#	if defined(TCCR4D) && defined(COM4D1)
		PIN_4D = 16,
#	endif
#endif
#if !defined(portUSE_TIMER5)
#	if defined(TCCR5A) && defined(COM5A1)
		PIN_5A = 17,
#endif
#	if defined(TCCR5B) && defined(COM5B1)
		PIN_5B = 18,
#endif
#	if defined(TCCR5C) && defined(COM5C1)
		PIN_5C = 19,
#	endif
#endif
		INVALID = 255
	};

	enum Timer {
		TIMER_0,
		TIMER_1,
		TIMER_2,
		TIMER_3,
		TIMER_4,
		TIMER_5,
		TIMER_NONE
	};

	/***************************************************************************
	 * MAPPING AND CONFIGURATION CLASS METHODS
	 **************************************************************************/

	static bool map(uint8_t pin, volatile void * & mycontrolbase, volatile void * & myoutputcompare8base, volatile void * & myoutputcompare16base, uint8_t & myoffset);

	static volatile void * control(Pin pin);

	static volatile void * outputcompare8(Pin pin);

	static volatile void * outputcompare16(Pin pin);

	static uint8_t offset(Pin pin);

	static uint8_t mask(Pin pin);

	static GPIO::Pin pwm2gpio(Pin pin);

	static Pin arduino2pwm(uint8_t id);

	static Timer setup(Pin pin);

	/***************************************************************************
	 * CREATION AND DESTRUCTION
	 **************************************************************************/

	explicit PWM(Pin mypwmpin)
	: gpio(pwm2gpio(mypwmpin))
	, gpiomask(GPIO::mask(pwm2gpio(mypwmpin)))
	, controlbase(control(mypwmpin))
	, outputcompare8base(outputcompare8(mypwmpin))
	, outputcompare16base(outputcompare16(mypwmpin))
	, pwmmask(mask(mypwmpin))
	{}

	explicit PWM(GPIO::Pin mygpiopin, Pin mypwmpin)
	: gpio(mygpiopin)
	, gpiomask(GPIO::mask(mygpiopin))
	, controlbase(control(mypwmpin))
	, outputcompare8base(outputcompare8(mypwmpin))
	, outputcompare16base(outputcompare16(mypwmpin))
	, pwmmask(mask(mypwmpin))
	{}

	~PWM() {}

	operator bool() const { return (gpio && (controlbase != 0) && ((outputcompare8base != 0) || (outputcompare16base != 0)) && (pwmmask != 0)); }

	/***************************************************************************
	 * STARTING AND STOPPING
	 **************************************************************************/

	void start(uint16_t dutycycle /* 0..65535 or 0..255 if eightbit */, bool eightbit = false);

	void stop(bool activelow = false);

protected:

	GPIO gpio;
	uint8_t gpiomask;
	volatile void * controlbase;
	volatile void * outputcompare8base;
	volatile void * outputcompare16base;
	uint8_t pwmmask;

};

inline volatile void * PWM::control(Pin pin) {
	volatile void * mycontrolbase;
	return (map(pin, mycontrolbase, unused.vvp, unused.vvp, unused.u8) ? mycontrolbase : 0);
}

inline volatile void * PWM::outputcompare8(Pin pin) {
	volatile void * myoutputcompare8base;
	return (map(pin, unused.vvp, myoutputcompare8base, unused.vvp, unused.u8) ? myoutputcompare8base : 0);
}

inline volatile void * PWM::outputcompare16(Pin pin) {
	volatile void * myoutputcompare16base;
	return (map(pin, unused.vvp, unused.vvp, myoutputcompare16base, unused.u8) ? myoutputcompare16base : 0);
}

inline uint8_t PWM::offset(Pin pin) {
	uint8_t myoffset;
	return (map(pin, unused.vvp, unused.vvp, unused.vvp, myoffset) ? myoffset : ~0);
}

inline uint8_t PWM::mask(Pin pin) {
	uint8_t myoffset;
	return (map(pin, unused.vvp, unused.vvp, unused.vvp, myoffset) ? (1 << myoffset) : 0);
}

}
}
}

#endif /* _COM_DIAG_AMIGO_MEGAAVR_PWM_H_ */
