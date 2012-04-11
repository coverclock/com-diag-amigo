#ifndef _COM_DIAG_AMIGO_MORSE_H_
#define _COM_DIAG_AMIGO_MORSE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/target/GPIO.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * Morse blinks an LED according to a caller-specified pattern. It uses busy
 * waiting, does not depend on FreeRTOS, and talks directly to the memory-mapped
 * general purpose I/O (GPIO) registers. This is useful for debugging when
 * absolutely nothing else is working. Ask me how I know this. Because it is so
 * low level, and because it is using an LED wired to a GPIO pin, it depends not
 * just on the underlying microcontroller model, but the specific board and
 * hardware configuration on which it is being used. The constructor parameters
 * default to the values appropriate for blinking the red LED on the Freetronics
 * EtherMega 2560 board. Your mileage may vary.
 *
 * The basic timing follows the traditional Morse rules:
 *
 * A dot ('.') is a short blink of about 125ms.\n
 * A dash ('-') is a long blink of three dots.\n
 * An implicit pause between dots and dashes is one dot.\n
 * An explicit pause (',') between letters is three dots.\n
 * A explicit pause (' ') between words is seven dots.\n
 *
 * For example, the sequence " .. .- -. -- " counts from zero to three.
 */
class Morse {

protected:

	static const GPIO::Pin LED = GPIO::PIN_B7;

	static const double DOT = 125.0; // milliseconds

	static const double DASH = 375.0; // milliseconds

	static const double PAUSE = 125.0; // milliseconds

	static const double LETTER = 250.0; // (3 * DOT) - PAUSE

	static const double WORD = 750.0; // (7 * DOT) - PAUSE.

public:

	/**
	 * Constructor.
	 * @param mybase points to the base address of the GPIO controller to use.
	 * @param mymask is the bit mask for the GPIO bit to use.
	 */
	Morse(volatile void * mybase = GPIO::base(LED), uint8_t mymask = GPIO::mask(LED))
	: base(mybase)
	, mask(mymask)
	{}

	/**
	 * Destructor.
	 */
	~Morse() {}

	/**
	 * Blink an LED according to a caller-specified pattern.
	 * @param code points to a string indicating the blink pattern.
	 */
	void morse(const char * code) const;

private:

	volatile void * base;
	uint8_t mask;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_MORSE_H_ */
