/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is specific to the Freetronics EtherMega 2560 board.
 */

#include <avr/delay.h>
#include "com/diag/amigo/target/Morse.h"
#include "com/diag/amigo/io.h"

namespace com {
namespace diag {
namespace amigo {

#define PIN			AMIGO_MMIO_8(base, 0)
#define DDR			AMIGO_MMIO_8(base, 1)
#define PORT		AMIGO_MMIO_8(base, 2)

// A dot ('.') is a short blink of about 125ms.
// A dash ('-') is a long blink of three dots.
// A pause between dots and dashes is one dot.
// A pause ('_') between letters is three dots.
// A pause (' ') between words is seven dots.

void Morse::morse(const char * code) {
	int count = 0;
	DDR |= mask; // Output.
	PORT &= ~mask; // Low.
	if (*code != '\0') {
		_delay_ms(LETTER);
		while (*code != '\0') {
			_delay_ms(PAUSE);
			switch (*(code++)) {
			case '.':
				PORT |= mask;
				_delay_ms(DOT);
				PORT &= ~mask;
				break;
			case '-':
				PORT |= mask;
				_delay_ms(DASH);
				PORT &= ~mask;
				break;
			case '_':
				PORT |= mask;
				_delay_ms(LETTER);
				PORT &= ~mask;
				break;
			case ' ':
			default:
				_delay_ms(WORD);
				break;
			}
			++count;
		}
		_delay_ms(LETTER);
		_delay_ms(PAUSE);
	}
}

}
}
}
