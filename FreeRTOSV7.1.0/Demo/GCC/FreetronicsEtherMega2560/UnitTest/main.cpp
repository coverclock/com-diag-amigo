/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/fatal.h"
#include "com/diag/amigo/target/harvard.h"
#include "com/diag/amigo/target/Morse.h"
#include "com/diag/amigo/target/Serial.h"
#include "com/diag/amigo/target/SPI.h"
#include "com/diag/amigo/target/Uninterruptable.h"
#include "com/diag/amigo/target/Console.h"
#include "com/diag/amigo/Task.h"
#include "com/diag/amigo/SerialSink.h"
#include "com/diag/amigo/Source.h"
#include "com/diag/amigo/Sink.h"
#include "com/diag/amigo/Print.h"
#include "com/diag/amigo/BinarySemaphore.h"
#include "com/diag/amigo/CountingSemaphore.h"
#include "com/diag/amigo/CriticalSection.h"
#include "com/diag/amigo/MutexSemaphore.h"

static com::diag::amigo::BinarySemaphore * binarysemaphorep = 0;

class TakerTask : public com::diag::amigo::Task {
public:
	explicit TakerTask(const char * name) : Task(name), errors(0) {}
	virtual void task();
	int errors;
} static takertask("Taker");

class UnitTestTask : public com::diag::amigo::Task {
public:
	explicit UnitTestTask(const char * name) : Task(name), errors(0) {}
	virtual void task();
	int errors;
} static unittesttask("UnitTest");

void TakerTask::task() {
	++errors;
	if (*binarysemaphorep == false) {
		++errors;
	} else if (!binarysemaphorep->take()) {
		++errors;
	} else {
		// Do nothing.
	}
	--errors;
	while (!stopped()) {
		yield();
	}
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

void UnitTestTask::task() {
	com::diag::amigo::Serial serial;
	com::diag::amigo::SerialSink serialsink(serial);
	com::diag::amigo::Print printf(serialsink);
	serial.start();

#if 0
	printf("sizeof Unit Test...\n");
	SIZEOF(com::diag::amigo::BinarySemaphore);
	SIZEOF(com::diag::amigo::Console);
	SIZEOF(com::diag::amigo::CountingSemaphore);
	SIZEOF(com::diag::amigo::CriticalSection);
	SIZEOF(com::diag::amigo::Morse);
	SIZEOF(com::diag::amigo::MutexSemaphore);
	SIZEOF(com::diag::amigo::Print);
	SIZEOF(com::diag::amigo::Queue);
	SIZEOF(com::diag::amigo::Serial);
	SIZEOF(com::diag::amigo::SerialSink);
	SIZEOF(com::diag::amigo::Sink);
	SIZEOF(com::diag::amigo::Source);
	SIZEOF(com::diag::amigo::SPI);
	SIZEOF(com::diag::amigo::Task);
	SIZEOF(com::diag::amigo::Uninterruptable);
	printf("sizeof Unit Test PASSED.\n");
#endif

#if 1
	printf("Delay Unit Test...\n");
	static const com::diag::amigo::Ticks W1 = 200;
	static const com::diag::amigo::Ticks W2 = 500;
	static const com::diag::amigo::Ticks PERCENT = 20;
	com::diag::amigo::Ticks elapsed;
	com::diag::amigo::Ticks t1 = milliseconds(com::diag::amigo::Task::elapsed());
	delay(ticks(W1));
	com::diag::amigo::Ticks t2 = milliseconds(com::diag::amigo::Task::elapsed());
	elapsed = t2 - t1;
	if (!((W1 <= elapsed) && (elapsed <= (W1 + (W1 / (100 / PERCENT)))))) {
		printf("(t2-t1)=%u?%u FAILED!\n", elapsed, W1);
		++errors;
	} else {
		com::diag::amigo::Ticks t4 = t1;
		delay(t4, ticks(W2));
		com::diag::amigo::Ticks t3 = milliseconds(com::diag::amigo::Task::elapsed());
		elapsed = t3 - t1;
		if (!((W2 <= elapsed) && (elapsed <= (W2 + (W2 / (100 / PERCENT)))))) {
			printf("(t3-t1)=%u?%u FAILED!\n", elapsed, W2);
			++errors;
		} else {
			printf("Delay Unit Test PASSED.\n");
		}
	}
#endif

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
	yield();
	if (takertask != true) {
		printf("takertask.start() FAILED!\n");
		++errors;
	} else if (*binarysemaphorep == false) {
		printf("BinarySemaphore() FAILED!\n");
		++errors;
	} else if (!binarysemaphorep->give()) {
		printf("BinarySemaphore::give() FAILED!\n");
		++errors;
	} else {
		yield();
		if (takertask != true) {
			printf("takertask yield FAILED!\n");
			++errors;
		} else if (takertask.stopped()) {
			printf("takertask.stopped() 1 FAILED!\n");
			++errors;
		} else {
			takertask.stop();
			if (!takertask.stopped()) {
				printf("takertask.stopped() 2 FAILED!\n");
				++errors;
			} else {
				for (int ii = 0; ii < 10; ++ii) {
					yield();
					if (false != takertask) { break; }
				}
				if (takertask != false) {
					printf("takertask.stop() FAILED!\n");
					++errors;
				} else if (takertask.errors != 0) {
					printf("takertask errors FAILED!\n");
					errors += takertask.errors;
				} else {
					printf("BinarySemaphore Unit Test PASSED.\n");
				}
			}
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
}

class Scope {
public:
	Scope() { com::diag::amigo::Console::instance().start().write("starting\r\n").flush().stop(); }
	~Scope() { com::diag::amigo::fatal(__FILE__, __LINE__); }
};

int main() __attribute__((OS_main));
int main() {
	Scope scope;

	// We test the busy waiting stuff in main before we create our tasks and
	// start the scheduler.

#if 0
	com::diag::amigo::Console::instance().start().write("Fatal Unit Test...\r\n").flush().stop();
	com::diag::amigo::fatal(__FILE__, __LINE__);
	com::diag::amigo::Console::instance().start().write("Fatal Unit Test FAILED!\r\n").flush().stop();
#endif

#if 1
	com::diag::amigo::Console::instance().start().write("Console Unit Test...\r\n").flush().stop();
	static const char STARTING[] PROGMEM = "STARTING";
	static const char EQUALS[] PROGMEM = "=0x";
	com::diag::amigo::Console::instance().start().write_P(STARTING, strlen_P(STARTING)).write_P(EQUALS).dump_P(&STARTING, sizeof(STARTING)).write('\r').write('\n').flush().stop();
	static const char TASK[] = "unittesttask";
	com::diag::amigo::Console::instance().start().write(TASK, strlen(TASK)).write("=0x").dump(&unittesttask, sizeof(unittesttask)).write('\r').write('\n').flush().stop();
	com::diag::amigo::Console::instance().start().write("Console Unit Test PASSED.\r\n").flush().stop();
#endif

#if 1
	{
		com::diag::amigo::Console::instance().start().write("Morse Unit Test...\r\n").flush().stop();
		com::diag::amigo::Morse telegraph;
		telegraph.morse(" .. .- -. -- ");
		com::diag::amigo::Console::instance().start().write("Morse Unit Test PASSED.\r\n").flush().stop();
	}
#endif

	sei();

	// The fact that main() never exits means local variables allocated on
	// its stack in its outermost braces never go out of scope. That means we
	// can take advantage of main to allocate objects on its stack that are
	// persist until the system is rebooted and pass pointers to them to tasks.
	// That's useful to know, especially in light of the fact that the memory
	// allocated for main's stack is never otherwise recovered or used.

	com::diag::amigo::BinarySemaphore binarysemaphore;
	binarysemaphorep = &binarysemaphore;

	com::diag::amigo::Console::instance().start().write("Task Unit Test...\r\n").flush().stop();
	takertask.start();
	unittesttask.start();
	if (takertask != true) {
		com::diag::amigo::Console::instance().start().write("main takertask.start() FAILED!\r\n").flush().stop();
	} else if (unittesttask != true) {
		com::diag::amigo::Console::instance().start().write("main unittesttask.start() FAILED!\r\n").flush().stop();
	} else {
		com::diag::amigo::Console::instance().start().write("Task Unit Test PASSED.\r\n").flush().stop();
		com::diag::amigo::Task::begin();
	}

	// Should never get here. Even if all our tasks here exit, the idle task
	// continues to run.

	com::diag::amigo::fatal(__FILE__, __LINE__);
}
