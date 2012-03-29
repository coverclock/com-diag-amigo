/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include "com/diag/amigo/Console.h"

namespace com {
namespace diag {
namespace amigo {

Console::Console()
{}

Console::~Console() {
}

inline uint8_t Console::begin() const {
	uint8_t ucsrb = UCSR0B;
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	return ucsrb;
}

inline void Console::end(uint8_t ucsrb) const {
	UCSR0B = ucsrb;
}

inline void Console::wait() const {
	while ((UCSR0A & _BV(UDRE0)) == 0) {
		// Do nothing.
	}
}

inline void Console::emit(uint8_t ch) const {
	wait();
	UDR0 = ch;
}

Console & Console::start() {
	static const uint16_t COUNTER = (((F_CPU) + (4UL * 115200UL)) / (8UL * 115200UL)) - 1UL;

	UBRR0L = COUNTER & 0xff;
	UBRR0H = (COUNTER >> 8) & 0xff;

	UCSR0A = _BV(U2X0);

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

	return *this;
}

Console & Console::stop() {
	return *this;
}

Console & Console::write(uint8_t ch) {
	uint8_t ucsrb = begin();
	emit(ch);
	end(ucsrb);
	return *this;
}

Console & Console::write(const char * string) {
	uint8_t ucsrb = begin();
	while (*string != '\0') {
		emit(*(string++));
	}
	end(ucsrb);
	return *this;
}

Console & Console::write(const void * data, size_t size) {
	uint8_t ucsrb = begin();
	static const char HEX[] = "0123456789ABCDEF";
	const uint8_t * here = static_cast<const uint8_t *>(data);
	uint8_t datum;
	while ((size--) > 0) {
		datum = *(here++);
		emit(HEX[datum >> 4]);
		emit(HEX[datum & 0xf]);
	}
	end(ucsrb);
	return *this;
}

Console & Console::flush() {
	wait();
	return *this;
}

}
}
}
