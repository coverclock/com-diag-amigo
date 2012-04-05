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
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/fatal.h"
#include "com/diag/amigo/Task.h"
#include "com/diag/amigo/target/Morse.h"
#include "com/diag/amigo/target/Serial.h"
#include "com/diag/amigo/target/SPI.h"
#include "com/diag/amigo/target/Uninterruptable.h"
#include "com/diag/amigo/SerialSink.h"
#include "com/diag/amigo/Print.h"
#include "com/diag/amigo/target/Console.h"
#include "com/diag/amigo/target/harvard.h"
#include "com/diag/amigo/BinarySemaphore.h"
#include "com/diag/amigo/CountingSemaphore.h"
#include "com/diag/amigo/CriticalSection.h"
#include "com/diag/amigo/MutexSemaphore.h"

static com::diag::amigo::Task takertask("Taker");
static com::diag::amigo::Task unittesttask("UnitTest");

static void * taker(void * parameter) {
	com::diag::amigo::BinarySemaphore * binarysemaphorep = static_cast<com::diag::amigo::BinarySemaphore *>(parameter);
	int result = 1;

#if 1
	if (!(*binarysemaphorep)) {
		result = -1;
	} else if (!binarysemaphorep->take()) {
		result = -2;
	} else {
		// Do nothing.
	}
#endif

	return reinterpret_cast<void *>(result);
}

inline void w5100init() {
	DDRB |= _BV(4);
}

inline void w5100set() {
	PORTB &= ~_BV(4);
}

inline int w5100read(com::diag::amigo::SPI & spi, uint16_t address) {
	spi.master(0x0f);
	spi.master(address >> 8);
	spi.master(address & 0xff);
	return spi.master();
}

inline void w5100reset() {
	PORTB |=  _BV(4);
}

static void * unittest(void * parameter) {
	com::diag::amigo::BinarySemaphore * binarysemaphorep = static_cast<com::diag::amigo::BinarySemaphore *>(parameter);
	int errors = 0;

	com::diag::amigo::Serial serial;
	com::diag::amigo::SerialSink serialsink(serial);
	com::diag::amigo::Print printf(serialsink);
	serial.start();

#if 1
	printf("Uninterruptable Unit Test...\n");
	{
		com::diag::amigo::Uninterruptable uninterruptable1;
		if (static_cast<int8_t>(uninterruptable1) > 0) {
			printf("uninterruptable1 FAILED!\n");
			++errors;
		}
		{
			com::diag::amigo::Uninterruptable uninterruptable2;
			if (static_cast<int8_t>(uninterruptable2) < 0) {
				printf("uninterruptable2 FAILED!\n");
				++errors;
			} else {
				printf("Uninterruptable Unit Test PASSED.\n");
			}
		}
	}
#endif

#if 1
	printf("BinarySemaphore Unit Test...\n");
	com::diag::amigo::Task::yield();
	if (!(*binarysemaphorep)) {
		printf("BinarySemaphore() FAILED!\n");
		++errors;
	} else if (!binarysemaphorep->give()) {
		printf("BinarySemaphore::give() FAILED!\n");
		++errors;
	} else {
		com::diag::amigo::Task::yield();
		int result = reinterpret_cast<int>(takertask.getResult());
		if (result != 1) {
			printf("takertask FAILED!\n");
			++errors;
		} else {
			printf("BinarySemaphore Unit Test PASSED.\n");
		}
	}
#endif

#if 1
	printf("CountingSemaphore Unit Test...\n");
	{
		com::diag::amigo::CountingSemaphore countingsemaphore(2, 2);
		if (!countingsemaphore) {
			printf("CountingSemaphore() FAILED!\n");
			++errors;
		} else if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			printf("CountingSemaphore::take() 1 FAILED!\n");
			++errors;
		} else if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			printf("CountingSemaphore::take() 2 FAILED!\n");
			++errors;
		} else if (countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			printf("CountingSemaphore::take() 3 FAILED!\n");
			++errors;
		} else if (!countingsemaphore.give()) {
			printf("CountingSemaphore::give() 1 FAILED!\n");
			++errors;
		} else if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			printf("CountingSemaphore::take() 4 FAILED!\n");
			++errors;
		} else if (!countingsemaphore.give()) {
			printf("CountingSemaphore::give() 2 FAILED!\n");
			++errors;
		} else if (!countingsemaphore.give()) {
			printf("CountingSemaphore::give() 3 FAILED!\n");
			++errors;
		} else if (countingsemaphore.give()) {
			printf("CountingSemaphore::give() 4 FAILED!\n");
			++errors;
		} else {
			printf("CountingSemaphore Unit Test PASSED.\n");
		}
	}
#endif

#if 1
	printf("MutexSemaphore Unit Test...\n");
	{
		com::diag::amigo::MutexSemaphore mutexsemaphore;
		if (!mutexsemaphore) {
			printf("MutexSemaphore() FAILED!\n");
			++errors;
		} else if (!mutexsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			printf("MutexSemaphore::take() FAILED!\n");
			++errors;
		} else if (!mutexsemaphore.give()) {
			printf("MutexSemaphore::give() FAILED!\n");
			++errors;
		} else {
			printf("MutexSemaphore Unit Test PASSED.\n");
			printf("CriticalSection Unit Test...\n");
			{
				com::diag::amigo::CriticalSection criticalsection1(mutexsemaphore);
				if (!criticalsection1) {
					printf("criticalsection1 FAILED!\n");
					++errors;
				} else {
					com::diag::amigo::CriticalSection criticalsection2(mutexsemaphore);
					if (!criticalsection2) {
						printf("criticalsection2 FAILED!\n");
						++errors;
					} else {
						printf("CriticalSection Unit Test PASSED.\n");
					}
				}
			}
		}
	}
#endif

#if 1
	{
		printf("SPI Unit Test (specific to W5100 Ethernet controller)...\n");
		com::diag::amigo::SPI spi;
		spi.start();
		{
			com::diag::amigo::CriticalSection criticalsection(spi);
			w5100init();
			static const uint16_t RTR0 = 0x0017;
			static const uint16_t RTR1 = 0x0018;
			static const uint16_t RCR = 0x0019;
			w5100set();
			uint8_t rtr0 = w5100read(spi, RTR0);
			w5100reset();
			w5100set();
			uint8_t rtr1 = w5100read(spi, RTR1);
			w5100reset();
			w5100set();
			uint8_t rcr = w5100read(spi, RCR);
			w5100reset();
			// Reference: WIZnet, W5100 Datasheet, Version 1.2.4, 2011
			if (rtr0 != 0x07) {
				printf("rtr0 FAILED!\n");
				++errors;
			} else if (rtr1 != 0xd0) {
				printf("rtr1 FAILED!\n");
				++errors;
			} else if (rcr != 0x08) {
				printf("rcr FAILED!\n");
				++errors;
			} else {
				printf("SPI Unit Test PASSED.\n");
			}
		}
		spi.stop();
	}
#endif

#if 1
	printf("Serial Unit Test (type control-D to exit)...\n");
	static const int CONTROL_D = 0x04;
	int ch = ~CONTROL_D;
	for (;;) {
		while (serial.available() > 0) {
			ch = serial.read();
			if (ch == CONTROL_D) { break; }
			serial.write(ch);
		}
		if (ch == CONTROL_D) { break; }
		com::diag::amigo::Task::yield();
	}
	printf("\nSerial Unit Test PASSED.\n");
#endif

	printf("errors=%d\n", errors);

	serial.flush();

	return reinterpret_cast<void *>(errors);
}

int main() __attribute__((OS_main));
int main() {

	com::diag::amigo::Console::instance().start().write("starting\r\n").flush().stop();

	// We test the busy waiting stuff in main before we create our tasks and
	// start the scheduler.

#if 0
	com::diag::amigo::Console::instance().start().write("Fatal Unit Test...\r\n").flush().stop();
	com::diag::amigo::fatal(__FILE__, __LINE__);
	com::diag::amigo::Console::instance().start().write("Fatal Unit Test FAILED!\r\n").flush().stop();
#endif

#if 1
	com::diag::amigo::Console::instance().start().write("Console Unit Test...\r\n").flush().stop();
	static const char STARTING[] PROGMEM = "STARTING=0x";
	com::diag::amigo::Console::instance().start().write_P(STARTING).dump_P(&STARTING, sizeof(STARTING)).write('\r').write('\n').flush().stop();
	com::diag::amigo::Console::instance().start().write_P(STARTING, strlen_P(STARTING)).dump_P(&STARTING, sizeof(STARTING)).write('\r').write('\n').flush().stop();
	void * (*task)(void *) = unittest;
	static const char TASK[] = "TASK=0x";
	com::diag::amigo::Console::instance().start().write(TASK).dump(&task, sizeof(task)).write('\r').write('\n').flush().stop();
	com::diag::amigo::Console::instance().start().write(TASK, strlen(TASK)).dump(&task, sizeof(task)).write('\r').write('\n').flush().stop();
	com::diag::amigo::Console::instance().start().write("Console Unit Test PASSED\r\n").flush().stop();
#endif

#if 1
	{
		com::diag::amigo::Console::instance().start().write("Morse Unit Test...\r\n").flush().stop();
		com::diag::amigo::Morse telegraph;
		telegraph.morse(" .. .- -. -- ");
		com::diag::amigo::Console::instance().start().write("Morse Unit Test PASSED\r\n").flush().stop();
	}
#endif

	sei();

	// Variables allocated on the stack in this function will never go out of
	// scope, since main never exits. So it is quite possible to allocate
	// what are essentially permanent objects on main's stack that persist
	// until the board is rebooted. That's useful.

	com::diag::amigo::Console::instance().start().write("Task Unit Test...\r\n").flush().stop();
	com::diag::amigo::BinarySemaphore binarysemaphore;
	takertask.start(taker, &binarysemaphore);
	unittesttask.start(unittest, &binarysemaphore);
	if (takertask != true) {
		com::diag::amigo::Console::instance().start().write("takertask.start() FAILED!\r\n").flush().stop();
	} else if (unittesttask != true) {
		com::diag::amigo::Console::instance().start().write("unittesttask.start() FAILED!\r\n").flush().stop();
	} else {
		com::diag::amigo::Console::instance().start().write("Task Unit Test PASSED.\r\n").flush().stop();
		com::diag::amigo::Task::begin();
	}

	com::diag::amigo::Console::instance().start().write("exiting\r\n").flush().stop();

	while (!0);
}
