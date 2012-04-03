#ifndef _COM_DIAG_AMIGO_MORSE_H_
#define _COM_DIAG_AMIGO_MORSE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is specific to the Freetronics EtherMega 2560 board.
 */

#include "com/diag/amigo/types.h"

namespace com {
namespace diag {
namespace amigo {

class Morse {

protected:

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
	Morse(volatile uint8_t * mybase /* e.g. &DDRB */, uint8_t mymask /* e.g. _BV(7) */)
	: base(mybase)
	, mask(mymask)
	{}

	/**
	 * Destructor.
	 */
	~Morse() {}

	/**
	 * This method blinks an LED according to a caller-specified pattern. It
	 * uses busy waiting, does not depend on FreeRTOS, and talks directly to
	 * the memory-mapped GPIO registers. This is useful for debugging when
	 * absolutely nothing else is working.
	 * The basic timing follows the traditional Morse rules.
	 * A dot ('.') is a short blink of about 125ms.
	 * A dash ('-') is a long blink of three dots.
	 * A pause between dots and dashes is one dot.
	 * A pause ('_') between letters is three dots.
	 * A pause (' ') between words is seven dots.
	 * @param code points to a string indicating the blink pattern.
	 */
	void morse(const char * code);

private:

	volatile uint8_t * base;
	uint8_t mask;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_MORSE_H_ */
