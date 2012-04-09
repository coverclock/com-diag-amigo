/**
 * @file
 * AMIGO UNIT TEST SUITE
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/fatal.h"
#include "com/diag/amigo/littleendian.h"
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
#include "com/diag/amigo/Dump.h"
#include "com/diag/amigo/BinarySemaphore.h"
#include "com/diag/amigo/CountingSemaphore.h"
#include "com/diag/amigo/CriticalSection.h"
#include "com/diag/amigo/MutexSemaphore.h"
#include "com/diag/amigo/Timer.h"

/*******************************************************************************
 * UNIT TEST FRAMEWORK (SUCH AS IT IS)
 ******************************************************************************/

static const char UNITTEST_FAILED_AT_LINE[] PROGMEM = "FAILED at line %d!\n";
static const char UNITTEST_PASSED[] PROGMEM = "PASSED.\n";
static const char CUNITTEST_FAILED_AT_LINE[] PROGMEM = "FAILED at line 0x";
static const char CUNITTEST_FAILED_EOL[] PROGMEM = "!\r\n";
static const char CUNITTEST_PASSED[] PROGMEM = "PASSED.\r\n";

#define UNITTEST(_NAME_) printf(PSTR("Unit Test " _NAME_ " "))
#define UNITTESTLN(_NAME_) printf(PSTR("Unit Test " _NAME_ "\n"))
#define FAILED(_LINE_) printf(UNITTEST_FAILED_AT_LINE, _LINE_)
#define PASSED() printf(UNITTEST_PASSED)

#define CUNITTEST(_NAME_) com::diag::amigo::Console::instance().start().write_P(PSTR("Unit Test " _NAME_ " ")).flush().stop()
#define CUNITTESTLN(_NAME_) com::diag::amigo::Console::instance().start().write_P(PSTR("Unit Test " _NAME_ "\r\n")).flush().stop()
#define CFAILED(_LINE_) do { int line = com::diag::amigo::littleendian() ? (((_LINE_) >> 8) & 0xff) | (((_LINE_) & 0xff) << 8) : (_LINE_); com::diag::amigo::Console::instance().start().write_P(CUNITTEST_FAILED_AT_LINE).dump(&line, sizeof(line)).write_P(CUNITTEST_FAILED_EOL).flush().stop(); } while (0)
#define CPASSED() com::diag::amigo::Console::instance().start().write_P(CUNITTEST_PASSED).flush().stop()

/*******************************************************************************
 * Timers
 ******************************************************************************/

class OneShotTimer : public com::diag::amigo::OneShotTimer {
public:
	explicit OneShotTimer(com::diag::amigo::Ticks duration) : com::diag::amigo::OneShotTimer(duration), counter(0) {}
	virtual void timer();
	int counter;
};

void OneShotTimer::timer() {
	++counter;
}

class PeriodicTimer : public com::diag::amigo::PeriodicTimer {
public:
	explicit PeriodicTimer(com::diag::amigo::Ticks duration) : com::diag::amigo::PeriodicTimer(duration), counter(0) {}
	virtual void timer();
	int counter;
};

void PeriodicTimer::timer() {
	++counter;
}

/*******************************************************************************
 * TAKER TASK (FOR TESTING BINARYSEMAPHORE)
 ******************************************************************************/

static com::diag::amigo::BinarySemaphore * binarysemaphorep = 0;

class TakerTask : public com::diag::amigo::Task {
public:
	explicit TakerTask(const char * name) : com::diag::amigo::Task(name), errors(0) {}
	virtual void task();
	int errors;
} static takertask("Taker");

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

/*******************************************************************************
 * WIZNET W5100 SPI FUNCTIONS (FOR TESTING SPI)
 ******************************************************************************/

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

/*******************************************************************************
 * UNIT TEST TASK
 ******************************************************************************/

class UnitTestTask : public com::diag::amigo::Task {
public:
	explicit UnitTestTask(const char * name) : com::diag::amigo::Task(name), errors(0) {}
	virtual void task();
	int errors;
} static unittesttask("UnitTest");

void UnitTestTask::task() {
	com::diag::amigo::Serial serial;
	com::diag::amigo::SerialSink serialsink(serial);
	com::diag::amigo::Print printf(serialsink, true);
	serial.start();

#if 1
	UNITTESTLN("sizeof");
	// The need to do this cast smells like a compiler bug to me, but I'd be
	// glad to be proven wrong. Not doing the cast results in BIGNUMs being
	// printed, regardless of the printf format, %u, %d, %lu, etc.
#	define SIZEOF(_TYPE_) printf(PSTR("sizeof(" # _TYPE_ ")=%lu\n"), static_cast<unsigned long>(sizeof(_TYPE_)));
	// Note how small many of these are. For some, it's just the two-byte
	// virtual pointer overhead for the virtual destructor (when one exists).
	SIZEOF(com::diag::amigo::BinarySemaphore);
	SIZEOF(com::diag::amigo::Console);
	SIZEOF(com::diag::amigo::CountingSemaphore);
	SIZEOF(com::diag::amigo::CriticalSection);
	SIZEOF(com::diag::amigo::Dump);
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
	PASSED();
#endif

#if 1
	UNITTEST("Task::delay");
	do {
		static const com::diag::amigo::Ticks W1 = 200;
		static const com::diag::amigo::Ticks W2 = 500;
		static const com::diag::amigo::Ticks PERCENT = 20;
		com::diag::amigo::Ticks elapsed;
		com::diag::amigo::Ticks t1 = milliseconds(com::diag::amigo::Task::elapsed());
		delay(ticks(W1));
		com::diag::amigo::Ticks t2 = milliseconds(com::diag::amigo::Task::elapsed());
		elapsed = t2 - t1;
		if (!((W1 <= elapsed) && (elapsed <= (W1 + (W1 / (100 / PERCENT)))))) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		com::diag::amigo::Ticks t4 = t1;
		delay(t4, ticks(W2));
		com::diag::amigo::Ticks t3 = milliseconds(com::diag::amigo::Task::elapsed());
		elapsed = t3 - t1;
		if (!((W2 <= elapsed) && (elapsed <= (W2 + (W2 / (100 / PERCENT)))))) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		PASSED();
	} while (false);
#endif

#if 0
	UNITTEST("Task::delay(FOREVER)");
	// This delays about two minutes and eleven seconds so I don't normally
	// run it. But it does verify that the FreeRTOS scheduler doesn't do
	// something unexpected with the maximum possible Ticks value.
	delay(FOREVER);
	PASSED();
#endif

#if 1
	UNITTESTLN("Dump");
	do {
		static const uint8_t datamemory[] = { 0xde, 0xad, 0xbe, 0xef };
		static const uint8_t programmemory[] PROGMEM = { 0xca, 0xfe, 0xba, 0xbe };
		com::diag::amigo::Dump dump(serialsink);
		com::diag::amigo::Dump dump_P(serialsink, true);
		dump(datamemory, sizeof(datamemory));
		printf(PSTR("\r\n"));
		dump_P(programmemory, sizeof(programmemory));
		printf(PSTR("\r\n"));
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("Uninterruptable");
	do {
		{
			if ((SREG & _BV(SREG_I)) == 0) {
				FAILED(__LINE__);
				++errors;
				break;
			}
			com::diag::amigo::Uninterruptable uninterruptable1;
			if (static_cast<int8_t>(uninterruptable1) >= 0) {
				FAILED(__LINE__);
				++errors;
				break;
			}
			if ((SREG & _BV(SREG_I)) != 0) {
				FAILED(__LINE__);
				++errors;
				break;
			}
			com::diag::amigo::Uninterruptable uninterruptable2;
			if (static_cast<int8_t>(uninterruptable2) < 0) {
				FAILED(__LINE__);
				++errors;
				break;
			}
			if ((SREG & _BV(SREG_I)) != 0) {
				FAILED(__LINE__);
				++errors;
				break;
			}
		}
		if ((SREG & _BV(SREG_I)) == 0) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("BinarySemaphore");
	do {
		// 0.5s should be enough for the takertask to initialize.
		delay(ticks(500));
		if (takertask != true) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (*binarysemaphorep == false) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (!binarysemaphorep->give()) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		// 0.5s should be enough for the takertask to become ready.
		delay(ticks(500));
		if (takertask != true) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (takertask.stopped()) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		takertask.stop();
		if (!takertask.stopped()) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		// 0.5s should be enough for the takertask to terminate.
		delay(ticks(500));
		if (takertask != false) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (takertask.errors != 0) {
			FAILED(__LINE__);
			errors += takertask.errors;
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("CountingSemaphore");
	do {
		com::diag::amigo::CountingSemaphore countingsemaphore(2, 2);
		if (!countingsemaphore) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (!countingsemaphore.give()) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (!countingsemaphore.give()) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (!countingsemaphore.give()) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (countingsemaphore.give()) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("MutexSemaphore");
	do {
		com::diag::amigo::MutexSemaphore mutexsemaphore;
		if (!mutexsemaphore) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (!mutexsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		if (!mutexsemaphore.give()) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		PASSED();
		UNITTEST("CriticalSection Unit Test");
		com::diag::amigo::CriticalSection criticalsection1(mutexsemaphore);
		if (!criticalsection1) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		com::diag::amigo::CriticalSection criticalsection2(mutexsemaphore);
		if (!criticalsection2) {
			FAILED(__LINE__);
			++errors;
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("Timer");
	do {

	} while (false);
#endif

#if 1
	UNITTEST("SPI (specific to W5100 Ethernet controller)");
	do {
		com::diag::amigo::SPI spi;
		spi.start();
		com::diag::amigo::MutexSemaphore mutex;
		do {
			com::diag::amigo::CriticalSection criticalsection(mutex);
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
				FAILED(__LINE__);
				++errors;
				break;
			}
			if (rtr1 != 0xd0) {
				FAILED(__LINE__);
				++errors;
				break;
			}
			if (rcr != 0x08) {
				FAILED(__LINE__);
				++errors;
				break;
			}
			PASSED();
		} while (false);
		spi.stop();
	} while (false);
#endif

#if 1
	UNITTESTLN("Serial (type control-D to exit)");
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
	PASSED();
#endif

	// Just to make sure the regular data memory printf() works.

	com::diag::amigo::Print errorf(serialsink);
	errorf("Unit Test errors=%d\n", errors);

	serial.flush();
}

/*******************************************************************************
 * MAIN PROGRAM
 ******************************************************************************/

class Scope {
public:
	Scope() { com::diag::amigo::Console::instance().start().write_P(PSTR("\r\nstarting\r\n")).flush().stop(); }
	~Scope() { com::diag::amigo::fatal(PSTR(__FILE__), __LINE__); }
};

int main() __attribute__((OS_main));
int main() {
	Scope scope;

	// We test the busy waiting stuff in main before we create our tasks and
	// start the scheduler.

#if 0
	CUNITTEST("fatal");
	com::diag::amigo::Console::instance().start().write_P(PSTR("Fatal Unit Test...\r\n")).flush().stop();
	com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
	CFAILED(__LINE__);
#endif

#if 1
	CUNITTESTLN("Console");
	static const char STARTING[] PROGMEM = "STARTING";
	static const char EQUALS[] PROGMEM = "=0x";
	com::diag::amigo::Console::instance().start().write_P(STARTING, strlen_P(STARTING)).write_P(EQUALS).dump_P(&STARTING, sizeof(STARTING)).write('\r').write('\n').flush().stop();
	static const char TASK[] = "unittesttask";
	com::diag::amigo::Console::instance().start().write(TASK, strlen(TASK)).write("=0x").dump(&unittesttask, sizeof(unittesttask)).write('\r').write('\n').flush().stop();
	CPASSED();
#endif

#if 1
	do {
		CUNITTEST("Morse");
		com::diag::amigo::Morse telegraph;
		telegraph.morse(" .. .- -. -- ");
		CPASSED();
	} while (false);
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

	CUNITTEST("Task");
	do {
		takertask.start();
		unittesttask.start();
		if (takertask != true) {
			CFAILED(__LINE__);
			break;
		}
		if (unittesttask != true) {
			CFAILED(__LINE__);
			break;
		}
		CPASSED();
		com::diag::amigo::Task::begin();
	} while (false);

	// Should never get here.
	com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
}
