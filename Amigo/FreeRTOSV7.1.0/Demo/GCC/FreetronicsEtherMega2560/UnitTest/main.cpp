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
#include "com/diag/amigo/target/Morse.h"
#include "com/diag/amigo/target/Serial.h"
#include "com/diag/amigo/target/Uninterruptable.h"
#include "com/diag/amigo/SerialSink.h"
#include "com/diag/amigo/Print.h"
#include "com/diag/amigo/target/Console.h"
#include "com/diag/amigo/target/harvard.h"
#include "com/diag/amigo/BinarySemaphore.h"
#include "com/diag/amigo/CountingSemaphore.h"
#include "com/diag/amigo/CriticalSection.h"
#include "com/diag/amigo/MutexSemaphore.h"

static com::diag::amigo::Serial * serialp;

static volatile int takerstatus = 0;

static void takerbody(com::diag::amigo::BinarySemaphore * binarysemaphorep) {
	com::diag::amigo::SerialSink serialsink(*serialp);
	com::diag::amigo::Print printf(serialsink);

#if 1
	if (!(*binarysemaphorep)) {
		takerstatus = -1;
	} else if (!binarysemaphorep->take()) {
		takerstatus = -2;
	} else {
		takerstatus = 1;
	}
#endif

	while (!0) { taskYIELD(); }
}

static void taker(void * parm) __attribute__((section(".lowtext")));
static void taker(void * parm) {
	takerbody(static_cast<com::diag::amigo::BinarySemaphore *>(parm));
}

static void unittestbody(com::diag::amigo::BinarySemaphore * binarysemaphorep) {
	com::diag::amigo::SerialSink serialsink(*serialp);
	com::diag::amigo::Print printf(serialsink);

#if 1
	printf("Uninterruptable Unit Test...\n");
	{
		com::diag::amigo::Uninterruptable uninterruptable1;
		if (static_cast<int8_t>(uninterruptable1) > 0) {
			printf("uninterruptable1 FAILED!\n");
		}
		{
			com::diag::amigo::Uninterruptable uninterruptable2;
			if (static_cast<int8_t>(uninterruptable2) < 0) {
				printf("uninterruptable2 FAILED!\n");
			} else {
				printf("Uninterruptable Unit Test PASSED.\n");
			}
		}
	}
#endif

#if 1
	printf("BinarySemaphore Unit Test...\n");
	taskYIELD();
	if (!(*binarysemaphorep)) {
		printf("BinarySemaphore() FAILED!\n");
	} else if (!binarysemaphorep->give()) {
		printf("BinarySemaphore::give() FAILED!\n");
	} else {
		taskYIELD();
		if (takerstatus != 1) {
			printf("takerstatus FAILED!\n");
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
			printf("CountingSemaphore() failed!\n");
		} else if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			printf("CountingSemaphore::take() 1 FAILED!\n");
		} else if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			printf("CountingSemaphore::take() 2 FAILED!\n");
		} else if (countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			printf("CountingSemaphore::take() 3 FAILED!\n");
		} else if (!countingsemaphore.give()) {
			printf("CountingSemaphore::give() 1 FAILED!\n");
		} else if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			printf("CountingSemaphore::take() 4 FAILED!\n");
		} else if (!countingsemaphore.give()) {
			printf("CountingSemaphore::give() 2 FAILED!\n");
		} else if (!countingsemaphore.give()) {
			printf("CountingSemaphore::give() 3 FAILED!\n");
		} else if (countingsemaphore.give()) {
			printf("CountingSemaphore::give() 4 FAILED!\n");
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
		} else if (!mutexsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			printf("MutexSemaphore::take() FAILED!\n");
		} else if (!mutexsemaphore.give()) {
			printf("MutexSemaphore::give() FAILED!\n");
		} else {
			printf("MutexSemaphore Unit Test PASSED.\n");
			printf("CriticalSection Unit Test...\n");
			{
				com::diag::amigo::CriticalSection criticalsection1(mutexsemaphore);
				if (!criticalsection1) {
					printf("criticalsection1 FAILED!\n");
				}
				{
					com::diag::amigo::CriticalSection criticalsection2(mutexsemaphore);
					if (!criticalsection2) {
						printf("criticalsection2 FAILED!\n");
					} else {
						printf("CriticalSection Unit Test PASSED\n");
					}
				}
			}
		}
	}
#endif

#if 1
	printf("Serial Unit Test (type control-D to exit)...\n");
	static const int CONTROL_D = 0x04;
	int ch = ~CONTROL_D;
	for (;;) {
		while (serialp->available() > 0) {
			ch = serialp->read();
			if (ch == CONTROL_D) { break; }
			serialp->write(ch);
		}
		if (ch == CONTROL_D) { break; }
		taskYIELD();
	}
	printf("\nSerial Unit Test PASSED\n");
#endif

	while (!0) { taskYIELD(); }
}

static void unittest(void * parm) __attribute__((section(".lowtext")));
static void unittest(void * parm) {
	unittestbody(static_cast<com::diag::amigo::BinarySemaphore *>(parm));
}

int main() __attribute__((OS_main));
int main() {

	com::diag::amigo::Console::instance().start().write("starting\r\n").flush().stop();

#if 1
	com::diag::amigo::Console::instance().start().write("Console Unit Test...\r\n").flush().stop();
	static const char STARTING[] PROGMEM = "STARTING=0x";
	com::diag::amigo::Console::instance().start().write_P(STARTING).dump_P(&STARTING, sizeof(STARTING)).write('\r').write('\n').flush().stop();
	com::diag::amigo::Console::instance().start().write_P(STARTING, strlen_P(STARTING)).dump_P(&STARTING, sizeof(STARTING)).write('\r').write('\n').flush().stop();
	void (*task)(void *) = unittest;
	static const char TASK[] = "TASK=0x";
	com::diag::amigo::Console::instance().start().write(TASK).dump(&task, sizeof(task)).write('\r').write('\n').flush().stop();
	com::diag::amigo::Console::instance().start().write(TASK, strlen(TASK)).dump(&task, sizeof(task)).write('\r').write('\n').flush().stop();
	com::diag::amigo::Console::instance().start().write("Console Unit Test PASSED\r\n").flush().stop();
#endif

#if 1
	{
		com::diag::amigo::Console::instance().start().write("Morse Unit Test...\r\n").flush().stop();
		com::diag::amigo::Morse telegraph(&PINB, _BV(7));
		telegraph.morse(" .. .- -. -- ");
		com::diag::amigo::Console::instance().start().write("Morse Unit Test PASSED\r\n").flush().stop();
	}
#endif

	sei();

	// Variables allocated on the stack in this function will never go out of
	// scope, since main never exits. So it is quite possible to allocate
	// what are essentially permanent objects on main's stack that persist
	// until the board is rebooted.

	com::diag::amigo::Serial serial;
	serialp = &serial;
	serialp->start();

	com::diag::amigo::BinarySemaphore binarysemaphore;

    xTaskHandle takerhandle;
    xTaskCreate(taker, (const signed char *)"Taker", 512, &binarysemaphore, 3, &takerhandle);

	xTaskHandle unittesthandle;
    xTaskCreate(unittest, (const signed char *)"UnitTest", 512, &binarysemaphore, 3, &unittesthandle);

	vTaskStartScheduler();

	com::diag::amigo::Console::instance().start().write("exiting\r\n").flush().stop();

	while (!0);
}
