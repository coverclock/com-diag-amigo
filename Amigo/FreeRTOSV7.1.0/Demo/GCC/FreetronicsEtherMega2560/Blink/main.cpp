/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "FreeRTOS.h"
#include "task.h"
#include "com/diag/amigo/arch/Serial.h"

int main(void)
{
	sei();

	com::diag::amigo::Serial console;

	console.start();

	console.emit('r');
	console.emit('u');
	console.emit('n');
	console.emit('n');
	console.emit('i');
	console.emit('n');
	console.emit('g');
	console.emit('\r');
	console.emit('\n');

	DDRB |= _BV(7);
	for (int ii = 0; ii < 10; ++ii) {
		PORTB &= ~_BV(7);
		_delay_ms(500);
		PORTB |= _BV(7);
		_delay_ms(500);
	}

	console.write('r');
	console.write('e');
	console.write('a');
	console.write('d');
	console.write('y');
	console.write('\r');
	console.write('\n');

	for (;;) {
		while (console.available() > 0) {
			console.write(console.read());
		}
	}

    return(0);
}
