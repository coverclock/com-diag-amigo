/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include <util/delay.h>
#include "com/diag/amigo/target/Console.h"

namespace com {
namespace diag {
namespace amigo {

static const char HEX[] PROGMEM = "0123456789ABCDEF";

static Console console;

Console & Console::instance() {
	return console;
}

Console::Console()
: ubrrl(0)
, ubrrh(0)
, ucsra(0)
, ucsrb(0)
, ucsrc(0)
{}

Console::~Console() {
}

inline void Console::emit(uint8_t ch) {
	while ((UCSR0A & _BV(UDRE0)) == 0) {
		// Do nothing.
	}
	UDR0 = ch;
}

Console & Console::start(uint32_t rate) {
	ubrrl = UBRR0L;
	ubrrh = UBRR0H;
	ucsra = UCSR0A;
	ucsrb = UCSR0B;
	ucsrc = UCSR0C;

	uint16_t counter = (((F_CPU) + (4UL * rate)) / (8UL * rate)) - 1UL;

	UBRR0L = counter & 0xff;
	UBRR0H = (counter >> 8) & 0xff;

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

Console & Console::write_P(PGM_P string) {
	uint8_t ch;
	while ((ch = pgm_read_byte(string++)) != '\0') {
		emit(ch);
	}
	return *this;
}

Console & Console::write(const void * data, size_t size) {
	const uint8_t * here = static_cast<const uint8_t *>(data);
	uint8_t datum;
	while ((size--) > 0) {
		datum = *(here++);
		emit(pgm_read_byte(&HEX[datum >> 4]));
		emit(pgm_read_byte(&HEX[datum & 0xf]));
	}
	return *this;
}

Console & Console::write_P(PGM_VOID_P data, size_t size) {
	PGM_P here = static_cast<PGM_P>(data);
	uint8_t datum;
	while ((size--) > 0) {
		datum = pgm_read_byte(here++);
		emit(pgm_read_byte(&HEX[datum >> 4]));
		emit(pgm_read_byte(&HEX[datum & 0xf]));
	}
	return *this;
}

// Merely checking the UDRE bit is not sufficient to insure that all data has
// been sent. Resetting the device prior to the TXC bit being set loses the
// hidden character in the two-level FIFO in the USART and it never gets
// transmitted. After some experience, even checking the TXC bit isn't
// sufficient, despite that the data sheet may say. This manifests as losing
// the last character, typically a newline. The delay below is equivalent to
// a little more than ten-bits at 115200 baud.
Console & Console::flush() {
	while ((UCSR0A & (_BV(UDRE0) | _BV(TXC0))) != (_BV(UDRE0) | _BV(TXC0))) {
		// Do nothing.
	}
	_delay_us(100.0);
	return *this;
}

CXXCAPI void amigo_console_start(uint32_t rate) {
	Console::instance().start(rate);
}

CXXCAPI void amigo_console_stop(void) {
	Console::instance().stop();
}

CXXCAPI void com_diag_amigo_console_write_char(uint8_t ch) {
	Console::instance().write(ch);
}

CXXCAPI void amigo_console_write_string(const char * string) {
	Console::instance().write(string);
}

CXXCAPI void amigo_console_write_string_P(PGM_P string) {
	Console::instance().write_P(string);
}

CXXCAPI void amigo_console_write_data(const void * data, size_t size) {
	Console::instance().write(data, size);
}

CXXCAPI void amigo_console_write_data_P(PGM_VOID_P data, size_t size) {
	Console::instance().write_P(data, size);

}

CXXCAPI void amigo_console_flush(void) {
	Console::instance().flush();
}

}
}
}
