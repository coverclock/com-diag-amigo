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
#include <stddef.h>
#include "FreeRTOS.h"
#include "task.h"
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/target/Serial.h"
#include "com/diag/amigo/target/Uninterruptable.h"
#include "com/diag/amigo/DebugSink.h"
#include "com/diag/amigo/SerialSink.h"
#include "com/diag/amigo/Print.h"
#include "com/diag/amigo/target/Console.h"

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

static com::diag::amigo::Serial * serialp;

static void unittest(void * parm) __attribute__((section(".lowtext")));
static void unittest(void * parm) {
	com::diag::amigo::SerialSink serialsink(*serialp);
	com::diag::amigo::Print printf(serialsink);

	printf("ready\n");

	for (;;) {
		while (serialp->available() > 0) {
			serialp->write(serialp->read());
		}
	}

}

int main() __attribute__((OS_main));
int main() {
	com::diag::amigo::Console console;
	console.start().write("starting\r\n").flush().stop();

	sei();

	com::diag::amigo::Serial serial;
	serialp = &serial;
	serialp->start();

	com::diag::amigo::DebugSink debugsink(*serialp);
	com::diag::amigo::Print debugf(debugsink);
	debugf("running\n");

    xTaskCreate(unittest, (const signed char *)"UnitTest", 512, 0, 3, 0);

	vTaskStartScheduler();

	console.start().write("exiting\r\n").flush().stop();

	for (;;);
}
