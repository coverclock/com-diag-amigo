/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include "com/diag/amigo/target/Console.h"

namespace com {
namespace diag {
namespace amigo {

Console::Console()
: ubrrl(0)
, ubrrh(0)
, ucsra(0)
, ucsrb(0)
, ucsrc(0)
{}

Console::~Console() {
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
	ubrrl = UBRR0L;
	ubrrh = UBRR0H;
	ucsra = UCSR0A;
	ucsrb = UCSR0B;
	ucsrc = UCSR0C;

	static const uint16_t COUNTER = (((F_CPU) + (4UL * 115200UL)) / (8UL * 115200UL)) - 1UL;

	UBRR0L = COUNTER & 0xff;
	UBRR0H = (COUNTER >> 8) & 0xff;

	UCSR0A = _BV(U2X0);

	UCSR0B = _BV(RXEN0) | _BV(TXEN0);

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

	return *this;
}

Console & Console::stop() {
	UBRR0L = ubrrl;
	UBRR0H = ubrrh;
	UCSR0A = ucsra;
	UCSR0B = ucsrb;
	UCSR0C = ucsrc;

	return *this;
}

Console & Console::write(uint8_t ch) {
	emit(ch);
	return *this;
}

Console & Console::write(const char * string) {
	while (*string != '\0') {
		emit(*(string++));
	}
	return *this;
}

Console & Console::write(const void * data, size_t size) {
	static const char HEX[] = "0123456789ABCDEF";
	const uint8_t * here = static_cast<const uint8_t *>(data);
	uint8_t datum;
	while ((size--) > 0) {
		datum = *(here++);
		emit(HEX[datum >> 4]);
		emit(HEX[datum & 0xf]);
	}
	return *this;
}

Console & Console::flush() {
	wait();
	return *this;
}

}
}
}
