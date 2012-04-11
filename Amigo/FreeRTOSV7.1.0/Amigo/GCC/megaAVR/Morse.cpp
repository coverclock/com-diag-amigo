/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <util/delay.h>
#include "com/diag/amigo/target/Morse.h"

namespace com {
namespace diag {
namespace amigo {

void Morse::morse(const char * code) const {
	GPIO gpio(base);
	gpio.output(mask, 0);
	if (*code != '\0') {
		_delay_ms(LETTER);
		while (*code != '\0') {
			_delay_ms(PAUSE);
			switch (*(code++)) {
			case '.':
				gpio.set(mask);
				_delay_ms(DOT);
				gpio.clear(mask);
				break;
			case '-':
				gpio.set(mask);
				_delay_ms(DASH);
				gpio.clear(mask);
				break;
			case ',':
				gpio.set(mask);
				_delay_ms(LETTER);
				gpio.clear(mask);
				break;
			default:
			case ' ':
				_delay_ms(WORD);
				break;
			}
		}
		_delay_ms(LETTER);
		_delay_ms(PAUSE);
	}
}

}
}
}
