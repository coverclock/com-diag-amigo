/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/delay.h>
#include "FreeRTOS.h"
#include "task.h"
#include "com/diag/amigo/arch/Serial.h"
#include "com/diag/amigo/arch/Uninterruptable.h"
#include "com/diag/amigo/DebugSink.h"
#include "com/diag/amigo/SerialSink.h"
#include "com/diag/amigo/Print.h"
#include "com/diag/amigo/Console.h"

static void morse(const char * code) {
	static const double DOT = 125.0;
	static const double DASH = 3 * DOT;
	static const double SPACE = DOT;
	static const double WORD = 7 * DOT;
	DDRB |= _BV(7);
	PORTB &= ~_BV(7);
	while (*code != '\0') {
		if (*code == '.') {
			_delay_ms(SPACE);
			PORTB |= _BV(7);
			_delay_ms(DOT);
			PORTB &= ~_BV(7);
		} else if (*code == '-') {
			_delay_ms(SPACE);
			PORTB |= _BV(7);
			_delay_ms(DASH);
			PORTB &= ~_BV(7);
		} else {
			_delay_ms(WORD);
		}
		++code;
	}
}

int main(void)
{
	morse(" ... ");

	com::diag::amigo::Console console;
	console.start().write("starting\r\n");

	morse(" ..- ");

	sei();

	com::diag::amigo::Serial serial;

	morse(" .-. ");

	serial.start();

	morse(" .-- ");

	serial.emit('n');
	serial.emit('o');
	serial.emit('w');
	serial.emit('\n');
	serial.emit('\r');

	morse(" -.. ");

	com::diag::amigo::DebugSink debugsink(serial);
	com::diag::amigo::SerialSink serialsink(serial);
	com::diag::amigo::Print debugf(debugsink);
	com::diag::amigo::Print printf(serialsink);

	morse(" -.- ");

	debugf("running\n");

	morse(" --. ");

	printf("ready\n");

	morse(" --- ");

	for (;;) {
		while (serial.available() > 0) {
			serial.write(serial.read());
		}
	}

    return(0);
}
