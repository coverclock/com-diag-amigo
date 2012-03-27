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
#include "com/diag/amigo/DebugSink.h"
#include "com/diag/amigo/SerialSink.h"
#include "com/diag/amigo/Print.h"

int main(void)
{
	sei();

	com::diag::amigo::Serial console;
	com::diag::amigo::DebugSink debugsink(console);
	com::diag::amigo::SerialSink serialsink(console);
	com::diag::amigo::Print debugf(debugsink);
	com::diag::amigo::Print printf(serialsink);

	console.start();

	debugf("running\n");

	DDRB |= _BV(7);
	for (int ii = 0; ii < 10; ++ii) {
		PORTB &= ~_BV(7);
		_delay_ms(500);
		PORTB |= _BV(7);
		_delay_ms(500);
	}

	printf("ready\n");

	for (;;) {
		while (console.available() > 0) {
			console.write(console.read());
		}
	}

    return(0);
}
