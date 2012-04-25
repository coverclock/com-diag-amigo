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
 * that support it. PWM can be used to give the appearance of varying analog
 * voltage under digital control. It generates a square wave with a caller
 * specified duty cycle (the ratio of the duration of its on-pulse to its total
 * period). The square wave signal is effectively averaged by most devices such
 * that the effective analog voltage is the duty cycle times Vcc. This trick
 * of physics can be used to control motor speed, LED brightness, etc. Pins
 * whose hardware timer/counter conflict with the hardware timer/counter used
 * to provide FreeRTOS with a system tick are not supported and their
 * enumerated values are not defined. (Meaning: if you try to use pins that
 * can't be used on your FreeRTOS configuration, your code won't compile).
 */
class PWM {

public:

	/**
	 * Pin numbers. These names can be matched to those in the data sheet by,
	 * for example, translating PIN_0A to to OC0A. (I think OC stands for output
	 * counter). Pins whose timers conflict with the timer used by FreeRTOS for
	 * its system tick are deliberately not defined.
	 */
	enum Pin {
#if !defined(portUSE_TIMER0)
#	if defined(TCCR0) && defined(COM00)
		PIN_0 = 0,
#	endif
#	if defined(TCCR0A) && defined(COM0A1)
		PIN_0A = 1,
#	endif
#	if defined(TCCR0A) && defined(COM0B1)
		PIN_0B = 2,
#	endif
#endif
#if !defined(portUSE_TIMER1)
#	if defined(TCCR1A) && defined(COM1A1)
		PIN_1A = 3,
#	endif
#	if defined(TCCR1A) && defined(COM1B1)
		PIN_1B = 4,
#	endif
#	if defined(TCCR1A) && defined(COM1C1)
		PIN_1C = 5,
#	endif
#	if defined(TCCR1A) && defined(COM1D1)
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
#	if defined(TCCR2A) && defined(COM2B1)
		PIN_2B = 9,
#	endif
#endif
#if !defined(portUSE_TIMER3)
#	if defined(TCCR3A) && defined(COM3A1)
		PIN_3A = 10,
#	endif
#	if defined(TCCR3A) && defined(COM3B1)
		PIN_3B = 11,
#	endif
#	if defined(TCCR3A) && defined(COM3C1)
		PIN_3C = 12,
#	endif
#endif
#if !defined(portUSE_TIMER4)
#	if defined(TCCR4A) && defined(COM4A1)
		PIN_4A = 13,
#	endif
#	if defined(TCCR4A) && defined(COM4B1)
		PIN_4B = 14,
#	endif
#	if defined(TCCR4A) && defined(COM4C1)
		PIN_4C = 15,
#	endif
#	if defined(TCCR4A) && defined(COM4D1)
		PIN_4D = 16,
#	endif
#endif
#if !defined(portUSE_TIMER5)
#	if defined(TCCR5A) && defined(COM5A1)
		PIN_5A = 17,
#endif
#	if defined(TCCR5A) && defined(COM5B1)
		PIN_5B = 18,
#endif
#	if defined(TCCR5A) && defined(COM5C1)
		PIN_5C = 19,
#	endif
#endif
		INVALID = 255
	};

	/**
	 * Every PIN pin is associated with a hardware timer that is used to
	 * automatically generate the square wave for Pulse Code Modulation without
	 * any software involvement. This is an numeration of the timers that may
	 * be available. Not all microcontroller models have all times. The
	 * ATmega328p used in the Arduino Uno only implements timers 0, 1, and 2.
	 * The ATmega2560 used in the Freetronics EtherMega implements 0 through 5.
	 */
	enum Timer {
		TIMER_0 = 0,
		TIMER_1 = 1,
		TIMER_2 = 2,
		TIMER_3 = 3,
		TIMER_4 = 4,
		TIMER_5 = 5,
		NONE = 255
	};

	/**
	 * This is the minimum duty cycle and is equivalent to a DC low signal.
	 * (I actually haven't tested this with a logic analyzer yet.)
	 */
	static const uint8_t LOW = 0;

	/**
	 * This is the maximum duty cycle and is equivalent to a DC high signal.
	 * (I actually haven't tested this with a logic analyzer yet.)
	 */
	static const uint8_t HIGH = ~0;

	/***************************************************************************
	 * MAPPING CLASS METHODS
	 **************************************************************************/

	/**
	 * Map a pin to a control register address.
	 * @param pin is a Pin enumerated value.
	 * @return a control register address or NULL if invalid.
	 */
	static volatile void * pwm2control(Pin pin);

	/**
	 * Map a pin to a 8-bit output comparator register address.
	 * @param pin is a Pin enumerated value.
	 * @return a control register address or NULL if invalid.
	 */
	static volatile void * pwm2outputcompare8(Pin pin);

	/**
	 * Map a pin to a 16-bit output comparator register address.
	 * @param pin is a Pin enumerated value.
	 * @return a control register address or NULL if invalid.
	 */
	static volatile void * pwm2outputcompare16(Pin pin);

	/**
	 * Map a pin to a control bit offset.
	 * @param pin is a Pin enumerated value.
	 * @return a control bit offset or ~0 if invalid.
	 */
	static uint8_t pwm2offset(Pin pin);

	/**
	 * Map a pin to a control bit mask.
	 * @param pin is a Pin enumerated value.
	 * @return a control bit mask or 0 if invalid.
	 */
	static uint8_t pwm2mask(Pin pin);

	/**
	 * Map a pin to its equivalent GPIO pin.
	 * @param pin is a Pin enumerated value.
	 * @return a GPIO pin enumerated value including GPIO::INVALID if invalid.
	 */
	static GPIO::Pin pwm2gpio(Pin pin);

	/**
	 * Map an Arduino pin number (which is typically printed right on the
	 * printed circuit board) to a Pin enumerated value. The mapping will be
	 * different for different models of Arduinos and AVR microcontrollers.
	 * @param number is an Arduino digital pin number.
	 * @return a Pin enumerated value or INVALID if not a valid pin number.
	 */
	static Pin arduino2pwm(uint8_t number);

	/**
	 * Map a pin to a Timer enumerated value identifying the timer that controls
	 * that pin.
	 * @param pin is a Pin enumerated value.
	 * @return a Timer enumerated value including NONE if invalid.
	 */
	static Timer pwm2timer(Pin pin);

	/***************************************************************************
	 * CREATION AND DESTRUCTION
	 **************************************************************************/

	/**
	 * Constructor.
	 * @param mypwmpin is a PWM pin enumerated value.
	 */
	explicit PWM(Pin mypwmpin)
	: gpio(GPIO::gpio2base(pwm2gpio(mypwmpin)))
	, gpiomask(GPIO::gpio2mask(pwm2gpio(mypwmpin)))
	, controlbase(pwm2control(mypwmpin))
	, outputcompare8base(pwm2outputcompare8(mypwmpin))
	, outputcompare16base(pwm2outputcompare16(mypwmpin))
	, pwmmask(pwm2mask(mypwmpin))
	, timer(pwm2timer(mypwmpin))
	{}

	/**
	 * Destructor.
	 */
	~PWM() {}

	/**
	 * Return true if construction was successful, false otherwise.
	 * @return true of construction was successful, false otherwise.
	 */
	operator bool() const { return (gpio && (controlbase != 0) && ((outputcompare8base != 0) || (outputcompare16base != 0)) && (pwmmask != 0) && (timer != NONE)); }

	/***************************************************************************
	 * STARTING AND STOPPING
	 **************************************************************************/

	/**
	 * Configure the GPIO pin and applicable timer for this pin for PWM
	 * operation. This overrides any other configuration the timer may have.
	 * The only way this method should fail is if the underlying code is wrong
	 * for the microcontroller model on which it is running. This operation
	 * only needs to be done once per pin, but it does need to be done for each
	 * pin even if the pins share a timer.
	 * @param high if true initializes the GPIO pin to high, otherwise low.
	 * @return true if successful, false otherwise.
	 */
	bool configure(bool high = false);

	/**
	 * Start generating square wave for Pulse Width Modulation at the specified
	 * duty cycle where 0 if a duty cycle of 0% and 255 is a duty cycle of 100%
	 * @param dutycycle is the duty cycle in the range 0 to 255.
	 */
	void start(uint8_t dutycycle /* 0..255 */);

	/**
	 * Stop generating a square wave.
	 * @param high if true leave the GPIO pin set to high, otherwise low.
	 */
	void stop(bool high = false);

protected:

	GPIO gpio;
	uint8_t gpiomask;
	volatile void * controlbase;
	volatile void * outputcompare8base;
	volatile void * outputcompare16base;
	uint8_t pwmmask;
	Timer timer;

};

inline uint8_t PWM::pwm2mask(Pin pin) {
	uint8_t offset = pwm2offset(pin);
	return (offset != static_cast<uint8_t>(~0)) ? (1 << offset) : 0;
}

}
}
}

#endif /* _COM_DIAG_AMIGO_MEGAAVR_PWM_H_ */
