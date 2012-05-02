/**
 * @file
 * AMIGO UNIT TEST SUITE
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <string.h>
#include <avr/io.h>
#include "com/diag/amigo/configuration.h"
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/fatal.h"
#include "com/diag/amigo/littleendian.h"
#include "com/diag/amigo/byteorder.h"
#include "com/diag/amigo/heap.h"
#include "com/diag/amigo/target/harvard.h"
#include "com/diag/amigo/target/Morse.h"
#include "com/diag/amigo/target/Serial.h"
#include "com/diag/amigo/target/SPI.h"
#include "com/diag/amigo/target/GPIO.h"
#include "com/diag/amigo/target/PWM.h"
#include "com/diag/amigo/target/A2D.h"
#include "com/diag/amigo/target/Uninterruptible.h"
#include "com/diag/amigo/target/Console.h"
#include "com/diag/amigo/Task.h"
#include "com/diag/amigo/SerialSink.h"
#include "com/diag/amigo/SerialSource.h"
#include "com/diag/amigo/Source.h"
#include "com/diag/amigo/Sink.h"
#include "com/diag/amigo/Print.h"
#include "com/diag/amigo/Dump.h"
#include "com/diag/amigo/BinarySemaphore.h"
#include "com/diag/amigo/CountingSemaphore.h"
#include "com/diag/amigo/CriticalSection.h"
#include "com/diag/amigo/MutexSemaphore.h"
#include "com/diag/amigo/Timer.h"
#include "com/diag/amigo/SPISlaveSelect.h"
#include "com/diag/amigo/W5100/W5100.h"
#include "com/diag/amigo/W5100/Socket.h"

extern "C" void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char * pcTaskName);

// Note that this is global and is initialized in main(). Hence it can be
// referenced in other translation units for debugging by having them create
// their own local SerialSink and Print objects.
com::diag::amigo::Serial * serialp = 0;

static int errors = 0;

/*******************************************************************************
 * UNIT TEST FRAMEWORK (SUCH AS IT IS)
 ******************************************************************************/

static const char UNITTEST_FAILED_AT_LINE[] PROGMEM = "FAILED at line %d!\n";
static const char UNITTEST_PASSED[] PROGMEM = "PASSED.\n";
static const char UNITTEST_TRACE[] PROGMEM = "TRACE at line %d; ";
static const char CUNITTEST_FAILED_AT_LINE[] PROGMEM = "FAILED at line 0x";
static const char CUNITTEST_FAILED_EOL[] PROGMEM = "!\r\n";
static const char CUNITTEST_PASSED[] PROGMEM = "PASSED.\r\n";

#define UNITTEST(_NAME_) do { printf(PSTR("Unit Test " _NAME_ " ")); serialp->flush(); } while (false)
#define UNITTESTLN(_NAME_) do { printf(PSTR("Unit Test " _NAME_ "\n")); serialp->flush(); } while (false)
#define FAILED(_LINE_) do { printf(UNITTEST_FAILED_AT_LINE, _LINE_); serialp->flush(); ++errors; } while (false)
#define PASSED() do { printf(UNITTEST_PASSED); serialp->flush(); } while (false)
#define TRACE(_LINE_) do { printf(UNITTEST_TRACE, _LINE_); serialp->flush(); } while (false)

/*******************************************************************************
 * CONSOLE TEST FIXTURE
 ******************************************************************************/

static const char VINTAGE[] PROGMEM = "VINTAGE=" COM_DIAG_AMIGO_VINTAGE;

																// e.g. (from UnitTest.map):
extern void * __data_start				__attribute__ ((weak)); // 00800200 D __data_start
extern void * __data_end				__attribute__ ((weak)); // 00800536 D __data_end
extern void * __bss_start				__attribute__ ((weak)); // 00800536 B __bss_start
extern void * __bss_end					__attribute__ ((weak)); // 00801636 B __bss_end
extern void * __trampolines_start		__attribute__ ((weak)); // 00001060 T __trampolines_start
extern void * __trampolines_end			__attribute__ ((weak)); // 00001060 T __trampolines_end
extern void * __ctors_start				__attribute__ ((weak)); // 00001144 T __ctors_start
extern void * __ctors_end				__attribute__ ((weak)); // 0000114a T __ctors_end
extern void * __dtors_start				__attribute__ ((weak)); // 0000114a T __dtors_start
extern void * __dtors_end				__attribute__ ((weak)); // 00001150 T __dtors_end
extern void * __data_load_start			__attribute__ ((weak)); // 0000cb5a A __data_load_start
extern void * __data_load_end			__attribute__ ((weak)); // 0000ce90 A __data_load_end

inline void * htonvp(void * vp) {
	return reinterpret_cast<void*>(com::diag::amigo::htons(reinterpret_cast<uintptr_t>(vp)));
}

// Deliberately not in PROGMEM in order to test Console::dump().
// Nonexistent weak external references will print as zero (NULL).
// Put in network byte order just to make dump more readable.
static const void * const MAP[] = {
	htonvp(__data_start),
	htonvp(__data_end),
	htonvp(__bss_start),
	htonvp(__bss_end),
	htonvp(__trampolines_start),
	htonvp(__trampolines_end),
	htonvp(__ctors_start),
	htonvp(__ctors_end),
	htonvp(__dtors_start),
	htonvp(__dtors_end),
	htonvp(__data_load_start),
	htonvp(__data_load_end),
};

class Scope {
public:
	Scope() {
		com::diag::amigo::Console::instance()
			.start()
			.write_P(PSTR("Unit Test Console\r\n"))
			.write_P(VINTAGE)
			.write("\r\n")
			.dump_P(&VINTAGE[sizeof("VINTAGE=") - 1], sizeof(VINTAGE) - sizeof("VINTAGE="))
			.write('\r')
			.write('\n')
			.dump(&MAP, sizeof(MAP))
			.write_P(PSTR("\r\nPASSED\r\n"))
			.flush()
			.stop();
	}
	~Scope() {
		com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
	}
};

/*******************************************************************************
 * TIMER TEST FIXTURES
 ******************************************************************************/

class OneShotTimer : public com::diag::amigo::OneShotTimer {
public:
	explicit OneShotTimer(com::diag::amigo::ticks_t duration) : com::diag::amigo::OneShotTimer(duration), now(0) {}
	virtual void timer();
	com::diag::amigo::ticks_t now;
};

void OneShotTimer::timer() {
	now = com::diag::amigo::Task::ticks2milliseconds(com::diag::amigo::Task::elapsed());
}

class PeriodicTimer : public com::diag::amigo::PeriodicTimer {
public:
	explicit PeriodicTimer(com::diag::amigo::ticks_t duration) : com::diag::amigo::PeriodicTimer(duration), counter(0) {}
	virtual void timer();
	unsigned int counter;
};

void PeriodicTimer::timer() {
	++counter;
}

/*******************************************************************************
 * WIZNET W5100 TEST FIXTURE (FOR TESTING SPI AND ALSO GPIO)
 ******************************************************************************/

static com::diag::amigo::MutexSemaphore * mutexsemaphorep = 0;

class W5100 {
public:
	explicit W5100(com::diag::amigo::MutexSemaphore & mymutex, com::diag::amigo::GPIO::Pin myss, com::diag::amigo::SPI & myspi)
	: mutex(&mymutex)
	, spi(&myspi)
	, gpio(com::diag::amigo::GPIO::gpio2base(myss))
	, mask(com::diag::amigo::GPIO::gpio2mask(myss))
	{
		gpio.output(mask, mask);
	}
	int read(uint16_t address) {
		com::diag::amigo::CriticalSection cs(*mutex);
		com::diag::amigo::SPISlaveSelect ss(gpio, mask);
		spi->master(0x0f);
		spi->master(address >> 8);
		spi->master(address & 0xff);
		int datum = spi->master();
		return datum;
	}
private:
	com::diag::amigo::MutexSemaphore * mutex;
	com::diag::amigo::SPI * spi;
	com::diag::amigo::GPIO gpio;
	uint8_t mask;
};

/*******************************************************************************
 * TAKER TEST FIXTURE (FOR TESTING BINARYSEMAPHORE)
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
 * SOURCE TO SINK COPY (FOR TESTING SOURCE AND SINK)
 ******************************************************************************/

static bool source2sink(com::diag::amigo::Source & source, com::diag::amigo::Sink & sink) {
	static const int CONTROL_D = 0x04;
	char buffer[20];
	unsigned int have;
	size_t in;
	size_t out;
	char * controld;
	bool eof = false;
	while (!eof) {
		com::diag::amigo::Task::yield();
		while ((have = source.available()) > 0) {
			if (have > (sizeof(buffer) - 1)) {
				have = sizeof(buffer) - 1;
			}
			in = source.read(buffer, have);
			if (in != have) {
				return false;
			}
			buffer[in] = '\0';
			if ((controld = strchr(buffer, CONTROL_D)) != 0) {
				*controld = '\0';
				in = controld - buffer;
				eof = true;
				if (in == 0) {
					break;
				}
			}
			out = sink.write(buffer, in);
			if (out != in) {
				return false;
			}
		}
	}
	sink.flush();
	return true;
}

/*******************************************************************************
 * BRIGHTNESS CONTROL (FOR TESTING PWM)
 ******************************************************************************/

static bool brightnesscontrol(com::diag::amigo::PWM::Pin pin, com::diag::amigo::ticks_t ticks) {
	com::diag::amigo::PWM pwm(pin);
	if (!pwm) {
		return false;
	}
	if (pwm.prestart() == com::diag::amigo::PWM::NONE) {
		return false;
	}
	for (uint16_t ii = 0; ii <= 250; ii += 25) {
		com::diag::amigo::Task::delay(ticks);
		pwm.start(ii);
	}
	com::diag::amigo::Task::delay(ticks);
	pwm.start(255);
	for (uint16_t ii = 250; ii > 0; ii -= 25) {
		com::diag::amigo::Task::delay(ticks);
		pwm.start(ii);
	}
	com::diag::amigo::Task::delay(ticks);
	pwm.start(0);
	com::diag::amigo::Task::delay(ticks);
	pwm.stop();
	return true;
}

/*******************************************************************************
 * UNIT TEST TASK
 ******************************************************************************/

namespace com { namespace diag { namespace amigo {
extern int Serial_port;
extern int Serial_countof;
extern volatile void * Serial_base;
extern int Serial_line;
extern bool Serial_ge;
extern bool Serial_lt;
extern bool Serial_not;
extern int Serial_port2;
} } }

class UnitTestTask : public com::diag::amigo::Task {
public:
	explicit UnitTestTask(const char * name) : com::diag::amigo::Task(name) {}
	virtual void task();
} static unittesttask("UnitTest");

void UnitTestTask::task() {
	com::diag::amigo::SerialSink serialsink(*serialp);
	com::diag::amigo::SerialSource serialsource(*serialp);
	com::diag::amigo::Print printf(serialsink, true);

	serialp->start();

#if 1
	UNITTEST("Serial");
	do {
		if (!(*serialp)) {
			FAILED(__LINE__);
			break;
		}
		{
			com::diag::amigo::Serial bogus(com::diag::amigo::Serial::FAIL);
printf(PSTR("P=0x%x C=0x%x B=0x%x L=%d F=%d\n"), com::diag::amigo::Serial_port, com::diag::amigo::Serial_countof, com::diag::amigo::Serial_base, com::diag::amigo::Serial_line, static_cast<bool>(bogus));
printf(PSTR("G=%d L=%d N=%d P=%d\n"), com::diag::amigo::Serial_ge, com::diag::amigo::Serial_lt, com::diag::amigo::Serial_not, com::diag::amigo::Serial_port2);
		if (bogus) {
				FAILED(__LINE__);
				break;
			}
		}
		PASSED();
	} while (false);
#endif

#if 0
	UNITTEST("fatal");
	com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
	FAILED(__LINE__);
#endif

#if 0
	UNITTESTLN("Overflow");
	{
		vApplicationStackOverflowHook(0, (signed char *)"OVERFLOW");
		PASSED();
	}
#endif

#if 1
	UNITTEST("Morse");
	do {
		com::diag::amigo::Morse telegraph;
		telegraph.morse(" .. .- -. -- ");
		PASSED();
	} while (false);
#endif

#if 1
	UNITTESTLN("Sink");
	do {
		size_t written;
		written = serialsink.write("Now is the time ");
		if (written != (sizeof("Now is the time ") - 1)) {
			FAILED(__LINE__);
			break;
		}
		written = serialsink.write("for all good men ", sizeof("for all good men ") - 1);
		if (written != (sizeof("for all good men ") - 1)) {
			FAILED(__LINE__);
			break;
		}
		written = serialsink.write_P(PSTR("to come to the aid "));
		if (written != (sizeof("to come to the aid ") - 1)) {
			FAILED(__LINE__);
			break;
		}
		written = serialsink.write_P(PSTR("of their country."), sizeof("of their country.") - 1);
		if (written != (sizeof("of their country ") - 1)) {
			FAILED(__LINE__);
			break;
		}
		written = serialsink.write('\r');
		if (written != 1) {
			FAILED(__LINE__);
			break;
		}
		written = serialsink.write('\n');
		if (written != 1) {
			FAILED(__LINE__);
			break;
		}
		serialsink.flush();
		PASSED();
	} while (false);
#endif

#if 1
	UNITTESTLN("sizeof");
	do {
		// The need to do this cast smells like a compiler bug to me, but I'd be
		// glad to be proven wrong. Not doing the cast results in BIGNUMs being
		// printed, regardless of the printf format, %u, %d, %lu, etc. Note
		// that this test itself tells us that sizeof(size_t) is two bytes, and
		// size_t is the type that sizeof() is supposed to return.
#		define SIZEOF(_TYPE_) printf(PSTR("sizeof(" # _TYPE_ ")=%u\n"), static_cast<size_t>(sizeof(_TYPE_)));
		// I actually worked on a project for a client in which in their code
		// base the unsigned wasn't the same size as the signed for one of
		// their locally-defined integer types. WTF? Man, that smelled bad.
		SIZEOF(char);
		SIZEOF(signed char);
		SIZEOF(unsigned char);
		SIZEOF(short);
		SIZEOF(signed short);
		SIZEOF(unsigned short);
		SIZEOF(int);
		SIZEOF(signed int);
		SIZEOF(unsigned int);
		SIZEOF(long);
		SIZEOF(signed long);
		SIZEOF(unsigned long);
		SIZEOF(long long);
		SIZEOF(signed long long);
		SIZEOF(unsigned long long);
		SIZEOF(bool);
		SIZEOF(int8_t);
		SIZEOF(uint8_t);
		SIZEOF(int16_t);
		SIZEOF(uint16_t);
		SIZEOF(int32_t);
		SIZEOF(uint32_t);
		SIZEOF(int64_t);
		SIZEOF(uint64_t);
		SIZEOF(ssize_t);
		SIZEOF(size_t);
		if (sizeof(size_t) != sizeof(ssize_t)) {
			// Fix <com/diag/amigo/types.h>!
			FAILED(__LINE__);
			break;
		}
		SIZEOF(float);
		SIZEOF(double);
		// Note how small many of these are. For some, it's just the two-byte
		// virtual pointer overhead for the virtual destructor.
		SIZEOF(com::diag::amigo::A2D);
		SIZEOF(com::diag::amigo::A2D::Pin);
		SIZEOF(com::diag::amigo::BinarySemaphore);
		SIZEOF(com::diag::amigo::Console);
		SIZEOF(com::diag::amigo::CountingSemaphore);
		SIZEOF(com::diag::amigo::CriticalSection);
		SIZEOF(com::diag::amigo::Dump);
		SIZEOF(com::diag::amigo::GPIO);
		SIZEOF(com::diag::amigo::GPIO::Pin);
		SIZEOF(com::diag::amigo::Morse);
		SIZEOF(com::diag::amigo::MutexSemaphore);
		SIZEOF(com::diag::amigo::PeriodicTimer);
		SIZEOF(com::diag::amigo::OneShotTimer);
		SIZEOF(com::diag::amigo::Print);
		SIZEOF(com::diag::amigo::PWM);
		SIZEOF(com::diag::amigo::PWM::Pin);
		SIZEOF(com::diag::amigo::Queue);
		SIZEOF(com::diag::amigo::Serial);
		SIZEOF(com::diag::amigo::SerialSink);
		SIZEOF(com::diag::amigo::SerialSource);
		SIZEOF(com::diag::amigo::Sink);
		SIZEOF(com::diag::amigo::Socket);
		SIZEOF(com::diag::amigo::Source);
		SIZEOF(com::diag::amigo::SPI);
		SIZEOF(com::diag::amigo::Task);
		SIZEOF(com::diag::amigo::Task::priority_t);
		SIZEOF(com::diag::amigo::ticks_t);
		SIZEOF(com::diag::amigo::Timer);
		SIZEOF(com::diag::amigo::TypedQueue<uint16_t>);
		SIZEOF(com::diag::amigo::TypedQueue<uint8_t>);
		SIZEOF(com::diag::amigo::Uninterruptible);
		SIZEOF(com::diag::amigo::W5100::W5100);
		SIZEOF(com::diag::amigo::W5100::Socket);
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("Task");
	do {
		Task proxyidletask(idle());
		if (!proxyidletask) {
			FAILED(__LINE__);
			break;
		}
		Task proxytimertask(com::diag::amigo::Timer::daemon());
		if (!proxytimertask) {
			FAILED(__LINE__);
			break;
		}
		Task proxyselftask(self());
		if (!proxyselftask) {
			FAILED(__LINE__);
			break;
		}
		if (proxyselftask.getHandle() != self()) {
			FAILED(__LINE__);
			break;
		}
		Task proxycurrentask;
		if (!proxycurrentask) {
			FAILED(__LINE__);
			break;
		}
		if (proxycurrentask.getHandle() != self()) {
			FAILED(__LINE__);
			break;
		}
		size_t proxyidlestack = proxyidletask.stack();
		size_t proxytimerstack = proxytimertask.stack();
		size_t takerstack = takertask.stack();
		size_t unitteststack = stack();
		size_t selfstack = stackSelf();
		if (unitteststack != selfstack) {
			FAILED(__LINE__);
			break;
		}
		uintptr_t stackpointerbefore = SPH;
		stackpointerbefore = (stackpointerbefore << 8) | SPL;
		yield();
		uintptr_t stackpointerafter = SPH;
		stackpointerafter = (stackpointerafter << 8) | SPL;
		if (stackpointerbefore != stackpointerafter) {
			FAILED(__LINE__);
			break;
		}
		PASSED();
		printf(PSTR("idlestack=%u\n"), proxyidlestack);
		printf(PSTR("timerstack=%u\n"), proxytimerstack);
		printf(PSTR("takerstack=%u\n"), takerstack);
		printf(PSTR("unitteststack=%u\n"), selfstack);
	} while (false);
#endif

#if 1
	UNITTEST("heap");
	do {
		size_t freeheapbefore = heap();
		void * thing = ::malloc(20);
		if (thing == 0) {
			FAILED(__LINE__);
			break;
		}
		::free(thing);
		thing = ::calloc(3, 10);
		if (thing == 0) {
			FAILED(__LINE__);
			break;
		}
		::free(thing);
		static bool ctor = false;
		static bool dtor = false;
		struct That { That() { ctor = true; } ~That() { dtor = true; } };
		That * that = new That;
		if (that == 0) {
			FAILED(__LINE__);
			break;
		}
		if ((!ctor) || dtor) {
			FAILED(__LINE__);
			break;
		}
		ctor = false;
		That * that2 = new(that) That;
		if (that2 != that) {
			FAILED(__LINE__);
			break;
		}
		if ((!ctor) || dtor) {
			FAILED(__LINE__);
			break;
		}
		that->~That();
		if (!dtor) {
			FAILED(__LINE__);
			break;
		}
		dtor = false;
		delete that;
		if (!dtor) {
			FAILED(__LINE__);
			break;
		}
		size_t freeheapafter = heap();
		if (freeheapbefore != freeheapafter) {
			FAILED(__LINE__);
			break;
		}
		PASSED();
		printf(PSTR("freeheap=%u\n"), freeheapafter);
	} while (false);
#endif

#if 1
	UNITTEST("littleendian and byteorder");
	// megaAVR is little-endian.
	do {
		if (!com::diag::amigo::littleendian()) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::byteswap16(0x1234) != 0x3412) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::htons(0x1234) != 0x3412) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::ntohs(0x1234) != 0x3412) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::byteswap32(0x12345678) != 0x78563412UL) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::htonl(0x12345678) != 0x78563412UL) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::ntohl(0x12345678) != 0x78563412UL) {
			FAILED(__LINE__);
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("Low Precision delay");
	do {
		static const com::diag::amigo::ticks_t W1 = 200;
		static const com::diag::amigo::ticks_t W2 = 500;
		// Typical test results show that 20% is not enough of a margin. I'm
		// guessing this is due to scheduling latency as I added tasks (most
		// recently the FreeRTOS timer task). Example: t1=102, t3=704,
		// milliseconds=602, but (W2+(20%*W2))=(W2*1.2)=600. So the test fails,
		// barely. This is good to know. Juggling task priorities might solve
		// this, but at risk of making timers less accurate. I would choose to
		// have more accurate timers, and have task delays be less accurate,
		// just based on my experience implementing telecommunications protocol
		// stacks. The stacks based real-time actions on timers not on task
		// delays. This is, after all, the "low precision" delay.
		static const com::diag::amigo::ticks_t PERCENT = 33;
		com::diag::amigo::ticks_t t1 = elapsed();
		delay(milliseconds2ticks(W1));
		com::diag::amigo::ticks_t t2 = elapsed();
		com::diag::amigo::ticks_t ms = ticks2milliseconds(t2 - t1);
		if (!((W1 <= ms) && (ms <= (W1 + (W1 / (100 / PERCENT)))))) {
			FAILED(__LINE__);
			break;
		}
		com::diag::amigo::ticks_t t3 = t1;
		delay(t3, milliseconds2ticks(W2));
		com::diag::amigo::ticks_t t4 = elapsed();
		ms = ticks2milliseconds(t4 - t1);
		if (!((W2 <= ms) && (ms <= (W2 + (W2 / (100 / PERCENT)))))) {
			FAILED(__LINE__);
			//printf(PSTR("t1=%u t2=%u t3=%u t4=%u ms=%u W2=%u W2+=%u\n"), t1, t2, t3, t4, ms, W2, (W2 + (W2 / (100 / PERCENT))));
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("High Precision busy wait");
	// There is no way to test the high precision busywait in a software-only
	// manner. If we leave interrupts enabled, tick processing by FreeRTOS adds
	// enormously to our delay (nearly doubles it), since for a 10ms delay we
	// are taking interrupt latency for 5 ticks. If we disable interrupts,
	// we have no way to measure time. So here we're really just measuring the
	// average high precision delay and assuming that the individual delays are
	// more or less correct. This is where I wish the AVR had a hardware time
	// base register like I've used on the PowerPC which is crazy precise.
	// OTOH, we could use this test to toggle a GPIO pin and measure the
	// square wave with our handy logic analyzer; that would be totally cool.
	do {
		static const com::diag::amigo::ticks_t W3 = 10;
		static const com::diag::amigo::ticks_t PERCENT = 100;
		com::diag::amigo::ticks_t t5 = ticks2milliseconds(elapsed());
		double microseconds = (W3 * 1000) / 100;
		for (uint8_t ii = 100; ii > 0; --ii) {
			busywait(microseconds);
		}
		com::diag::amigo::ticks_t t6 = ticks2milliseconds(elapsed());
		com::diag::amigo::ticks_t milliseconds = t6 - t5;
		if (!((W3 <= milliseconds) && (milliseconds <= (W3 + (W3 / (100 / PERCENT)))))) {
			FAILED(__LINE__);
			break;
		}
		PASSED();
	} while (false);
#endif

#if 0
	UNITTEST("FOREVER");
	// This delays about two minutes and eleven seconds so I don't normally
	// run it. But it does verify that the FreeRTOS scheduler doesn't do
	// something unexpected with the maximum possible tick value.
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
		static const char ZEROX[] PROGMEM = "0x";
		static const char CRLF[] PROGMEM = "\r\n";
		printf(ZEROX); dump(datamemory, sizeof(datamemory)); printf(CRLF);
		printf(ZEROX); dump_P(programmemory, sizeof(programmemory)); printf(CRLF);
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("Uninterruptible");
	do {
		{
			if ((SREG & _BV(SREG_I)) == 0) {
				FAILED(__LINE__);
				break;
			}
			uint8_t sreg = com::diag::amigo::interrupts::disable();
			if ((SREG & _BV(SREG_I)) != 0) {
				FAILED(__LINE__);
				break;
			}
			com::diag::amigo::interrupts::restore(sreg);
			if ((SREG & _BV(SREG_I)) == 0) {
				FAILED(__LINE__);
				break;
			}
			com::diag::amigo::Uninterruptible uninterruptible1;
			if ((static_cast<int8_t>(uninterruptible1) & _BV(SREG_I)) == 0) {
				FAILED(__LINE__);
				break;
			}
			if ((SREG & _BV(SREG_I)) != 0) {
				FAILED(__LINE__);
				break;
			}
			{
				com::diag::amigo::Uninterruptible uninterruptible2;
				if ((static_cast<int8_t>(uninterruptible2) & _BV(SREG_I)) != 0) {
					FAILED(__LINE__);
					break;
				}
				if ((SREG & _BV(SREG_I)) != 0) {
					FAILED(__LINE__);
					break;
				}
			}
			if ((SREG & _BV(SREG_I)) != 0) {
				FAILED(__LINE__);
				break;
			}
		}
		if ((SREG & _BV(SREG_I)) == 0) {
			FAILED(__LINE__);
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("BinarySemaphore");
	do {
		// 0.5s should be enough for the takertask to initialize.
		static const com::diag::amigo::ticks_t DELAY = milliseconds2ticks(500);
		delay(DELAY);
		if (takertask != true) {
			FAILED(__LINE__);
			break;
		}
		if (self() != getHandle()) {
			FAILED(__LINE__);
			break;
		}
		if (idle() == 0) {
			FAILED(__LINE__);
			break;
		}
		if (tasks() != 4) {
			// UnitTestTask, TakerTask, Idle Task, Timer Task.
			FAILED(__LINE__);
			break;
		}
		if (*binarysemaphorep == false) {
			FAILED(__LINE__);
			break;
		}
		if (!binarysemaphorep->give()) {
			FAILED(__LINE__);
			break;
		}
		// 0.5s should be enough for the takertask to become ready.
		delay(DELAY);
		if (takertask != true) {
			FAILED(__LINE__);
			break;
		}
		if (takertask.stopped()) {
			FAILED(__LINE__);
			break;
		}
		takertask.stop();
		if (!takertask.stopped()) {
			FAILED(__LINE__);
			break;
		}
		// 0.5s should be enough for the takertask to terminate.
		delay(DELAY);
		if (takertask != false) {
			FAILED(__LINE__);
			break;
		}
		if (takertask.errors != 0) {
			FAILED(__LINE__);
			break;
		}
		if (tasks() != 3) {
			// UnitTestTask, Idle Task, Timer Task.
			FAILED(__LINE__);
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
			break;
		}
		if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			break;
		}
		if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			break;
		}
		if (countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			break;
		}
		if (!countingsemaphore.give()) {
			FAILED(__LINE__);
			break;
		}
		if (!countingsemaphore.take(com::diag::amigo::CountingSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			break;
		}
		if (!countingsemaphore.give()) {
			FAILED(__LINE__);
			break;
		}
		if (!countingsemaphore.give()) {
			FAILED(__LINE__);
			break;
		}
		if (countingsemaphore.give()) {
			FAILED(__LINE__);
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
			break;
		}
		if (!mutexsemaphore.take(com::diag::amigo::MutexSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			break;
		}
		if (!mutexsemaphore.give()) {
			FAILED(__LINE__);
			break;
		}
		if (!mutexsemaphore.take(com::diag::amigo::MutexSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			break;
		}
		if (!mutexsemaphore.take(com::diag::amigo::MutexSemaphore::IMMEDIATELY)) {
			FAILED(__LINE__);
			break;
		}
		if (!mutexsemaphore.give()) {
			FAILED(__LINE__);
			break;
		}
		if (!mutexsemaphore.give()) {
			FAILED(__LINE__);
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("CriticalSection");
	do {
		com::diag::amigo::MutexSemaphore mutexsemaphore;
		{
			com::diag::amigo::CriticalSection criticalsection1(mutexsemaphore);
			if (!criticalsection1) {
				FAILED(__LINE__);
				break;
			}
			{
				com::diag::amigo::CriticalSection criticalsection2(mutexsemaphore);
				if (!criticalsection2) {
					FAILED(__LINE__);
					break;
				}
			}
			{
				com::diag::amigo::CriticalSection criticalsection3(0);
				if (criticalsection3) {
					FAILED(__LINE__);
					break;
				}
			}
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("PeriodicTimer");
	{
		static const com::diag::amigo::ticks_t T1 = 100;
		static const com::diag::amigo::ticks_t W1 = 500;
		PeriodicTimer periodictimer(milliseconds2ticks(T1));
		do {
			if (periodictimer != true) {
				FAILED(__LINE__);
				break;
			}
			delay(milliseconds2ticks(W1));
			if (periodictimer.counter != 0) {
				FAILED(__LINE__);
				break;
			}
			if (!periodictimer.start()) {
				FAILED(__LINE__);
				break;
			}
			delay(milliseconds2ticks(W1));
			if (!periodictimer.stop()) {
				FAILED(__LINE__);
				break;
			}
			if (!(((W1 / T1) <= periodictimer.counter) && (periodictimer.counter <= ((W1 / T1) + 1)))) {
				FAILED(__LINE__);
				break;
			}
			delay(milliseconds2ticks(W1));
			if (!(((W1 / T1) <= periodictimer.counter) && (periodictimer.counter <= ((W1 / T1) + 1)))) {
				FAILED(__LINE__);
				break;
			}
			PASSED();
		} while (false);
		// Try to avoid taking a fatal() in the destructor because the timer
		// task hasn't stopped the timer yet.
		periodictimer.stop();
		delay(milliseconds2ticks(W1));
	}
#endif

#if 1
	UNITTEST("OneShotTimer");
	{
		static const com::diag::amigo::ticks_t T2 = 200;
		static const com::diag::amigo::ticks_t T3 = 300;
		static const com::diag::amigo::ticks_t W2 = 500;
		static const com::diag::amigo::ticks_t PERCENT = 20;
		OneShotTimer oneshottimer(milliseconds2ticks(T2));
		do {
			if (oneshottimer != true) {
				FAILED(__LINE__);
				break;
			}
			delay(milliseconds2ticks(W2));
			if (oneshottimer.now != 0) {
				FAILED(__LINE__);
				break;
			}
			com::diag::amigo::ticks_t t2 = ticks2milliseconds(elapsed());
			if (!oneshottimer.start()) {
				FAILED(__LINE__);
				break;
			}
			delay(milliseconds2ticks(W2));
			if (oneshottimer.now == 0) {
				FAILED(__LINE__);
				break;
			}
			com::diag::amigo::ticks_t milliseconds = oneshottimer.now - t2;
			if (!((T2 <= milliseconds) && (milliseconds <= (T2 + (T2 / (100 / PERCENT)))))) {
				FAILED(__LINE__);
				break;
			}
			oneshottimer.now = 0;
			if (!oneshottimer.start()) {
				FAILED(__LINE__);
				break;
			}
			delay(milliseconds2ticks(T2 / 2));
			com::diag::amigo::ticks_t t3 = ticks2milliseconds(elapsed());
			if (!oneshottimer.reset()) {
				FAILED(__LINE__);
				break;
			}
			delay(milliseconds2ticks(W2));
			if (oneshottimer.now == 0) {
				FAILED(__LINE__);
				break;
			}
			milliseconds = oneshottimer.now - t3;
			if (!((T2 <= milliseconds) && (milliseconds <= (T2 + (T2 / (100 / PERCENT)))))) {
				FAILED(__LINE__);
				break;
			}
			oneshottimer.now = 0;
			if (!oneshottimer.reset()) {
				FAILED(__LINE__);
				break;
			}
			delay(milliseconds2ticks(T2 / 2));
			com::diag::amigo::ticks_t t4 = ticks2milliseconds(elapsed());
			if (!oneshottimer.reschedule(milliseconds2ticks(T3))) {
				FAILED(__LINE__);
				break;
			}
			delay(milliseconds2ticks(W2));
			if (oneshottimer.now == 0) {
				FAILED(__LINE__);
				break;
			}
			milliseconds = oneshottimer.now - t4;
			if (!((T3 <= milliseconds) && (milliseconds <= (T3 + (T3 / (100 / PERCENT)))))) {
				FAILED(__LINE__);
				break;
			}
			com::diag::amigo::ticks_t t5 = ticks2milliseconds(elapsed());
			if (!oneshottimer.reschedule(milliseconds2ticks(T3))) {
				FAILED(__LINE__);
				break;
			}
			delay(milliseconds2ticks(W2));
			if (oneshottimer.now == 0) {
				FAILED(__LINE__);
				break;
			}
			milliseconds = oneshottimer.now - t5;
			if (!((T3 <= milliseconds) && (milliseconds <= (T3 + (T3 / (100 / PERCENT)))))) {
				FAILED(__LINE__);
				break;
			}
			PASSED();
		} while (false);
		// Try to avoid taking a fatal() because the timer task hasn't stopped
		// the timer yet.
		oneshottimer.stop();
		delay(milliseconds2ticks(W2));
	}
#endif

#if 1
	UNITTEST("GPIO");
	// This is not a very good unit test. But I'm surprised about how much
	// variance there is among megaAVRs in GPIO port nomenclature. For example,
	// the ATmega328P (used on the Arduino Uno) starts with GPIO B. I'm sure
	// there's a rhyme or reason for it, but I clueless as to what it is. The
	// pins I check for below are the ones used by the Freetronics EtherMega
	// 2560 (ATmega2560) and the Arduino Uno (ATmega328p) respectively for
	// their diagnostic LEDs, and hence would be the pins likely used by Morse.
	do {
		if (com::diag::amigo::GPIO::gpio2base(com::diag::amigo::GPIO::PIN_B7) != &PINB) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::GPIO::gpio2offset(com::diag::amigo::GPIO::PIN_B7) != 7) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::GPIO::gpio2mask(com::diag::amigo::GPIO::PIN_B7) != _BV(7)) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::GPIO::gpio2base(com::diag::amigo::GPIO::PIN_B4) != &PINB) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::GPIO::gpio2offset(com::diag::amigo::GPIO::PIN_B4) != 4) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::GPIO::gpio2mask(com::diag::amigo::GPIO::PIN_B4) != _BV(4)) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::GPIO::gpio2base(com::diag::amigo::GPIO::INVALID) != 0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::GPIO::gpio2offset(com::diag::amigo::GPIO::INVALID) != static_cast<uint8_t>(~0)) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::GPIO::gpio2mask(com::diag::amigo::GPIO::INVALID) != 0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::GPIO::arduino2gpio(~0) != com::diag::amigo::GPIO::INVALID) {
			FAILED(__LINE__);
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("Digital I/O (requires text fixture on EtherMega)");
	// This is a separate test because it requires a simple text fixture to be
	// wired up on the board. It is specific to the EtherMega 2560 board.
	// PE4 (Arduino Mega pin 2) wired to ground.
	// PE5 (Arduino Mega pin 3) wired to Vcc.
	// PG5 (Arduino Mega pin 4) is otherwise in use.
	// PE3 (Arduino Mega pin 5) connected to PH3 (Arduino Mega pin 6).
	// PH4 (Arduino Mega pin 7) not connected.
	do {
		typedef com::diag::amigo::GPIO GPIO;
		GPIO::Pin pin2 = GPIO::arduino2gpio(2);
		if (pin2 != GPIO::PIN_E4) {
			FAILED(__LINE__);
			break;
		}
		GPIO::Pin pin3 = GPIO::arduino2gpio(3);
		if (pin3 != GPIO::PIN_E5) {
			FAILED(__LINE__);
			break;
		}
		GPIO::Pin pin5 = GPIO::arduino2gpio(5);
		if (pin5 != GPIO::PIN_E3) {
			FAILED(__LINE__);
			break;
		}
		GPIO::Pin pin6 = GPIO::arduino2gpio(6);
		if (pin6 != GPIO::PIN_H3) {
			FAILED(__LINE__);
			break;
		}
		GPIO::Pin pin7 = GPIO::arduino2gpio(7);
		if (pin7 != GPIO::PIN_H4) {
			FAILED(__LINE__);
			break;
		}
		GPIO::input(pin2);
		GPIO::input(pin3);
		GPIO::output(pin5, false);
		GPIO::input(pin6);
		GPIO::pulledup(pin7);
		if (GPIO::get(pin2)) {
			FAILED(__LINE__);
			break;
		}
		if (!GPIO::get(pin3)) {
			FAILED(__LINE__);
			break;
		}
		if (GPIO::get(pin6)) {
			FAILED(__LINE__);
			break;
		}
		GPIO::set(pin5);
		if (!GPIO::get(pin6)) {
			FAILED(__LINE__);
			break;
		}
		GPIO::clear(pin5);
		if (GPIO::get(pin6)) {
			FAILED(__LINE__);
			break;
		}
		if (!GPIO::get(pin7)) {
			FAILED(__LINE__);
			break;
		}
		// This is just an example of how the instance methods might be used
		// using instance method chaining. They are also useful when you want
		// to manipulate several bits in the same port atomically. It is not
		// unusual to implement hardware protocols like Two Wire Interface
		// (TWI a.k.a. I2C), Serial Peripheral Interface (SPI), or even a
		// serial port by "bit banging" the GPIO pins. This interface is
		// intended to facilitate this, hence the built-in delay method that
		// can be incorporated in a method chain.
		volatile void * base = GPIO::gpio2base(GPIO::arduino2gpio(13));
		uint8_t mask = GPIO::gpio2mask(GPIO::arduino2gpio(13));
		com::diag::amigo::ticks_t ticks = com::diag::amigo::Task::milliseconds2ticks(500);
		GPIO gpio(base);
		gpio.output(mask).set(mask).delay(ticks).clear(mask).delay(ticks).set(mask).delay(ticks).clear(mask).delay(ticks).set(mask).delay(ticks).clear(mask).delay(ticks).set(mask).delay(ticks).clear(mask).delay(ticks).set(mask).delay(ticks).clear(mask);
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("PWM");
	// This should work on either the 2560 (Arduino Mega and compatibles) or
	// the 328p (Arduino Uno and compatibles) since both implement PWM on
	// timers 0, 1, and 2. Depending on what timer FreeRTOS is configured to
	// use as the system tick timer, some of these tests have to be suppressed
	// because attempts to use a PWM pin that depends on the FreeRTOS tick timer
	// will cause the underlying code will return some deliberately invalid
	// value like NULL or 0 or ~0. You can't actually test this because the
	// PWM class will not even define a pin enumerated value for PWM pins
	// on the FreeRTOS tick timer.
	do {
		if (com::diag::amigo::PWM::LOW != 0x00U) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::HIGH != 0xffU) {
			FAILED(__LINE__);
			break;
		}
#if defined(TCCR0B) && !defined(portUSE_TIMER0)
		if (com::diag::amigo::PWM::pwm2control(com::diag::amigo::PWM::PIN_0B) != &TCCR0A) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2outputcompare8(com::diag::amigo::PWM::PIN_0B) != &OCR0B) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2outputcompare16(com::diag::amigo::PWM::PIN_0B) != 0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2offset(com::diag::amigo::PWM::PIN_0B) != COM0B1) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2mask(com::diag::amigo::PWM::PIN_0B) != _BV(COM0B1)) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2timer(com::diag::amigo::PWM::PIN_0B) != com::diag::amigo::PWM::TIMER_0) {
			FAILED(__LINE__);
			break;
		}
#endif
#if defined(TCCR1A) && !defined(portUSE_TIMER1)
		if (com::diag::amigo::PWM::pwm2control(com::diag::amigo::PWM::PIN_1A) != &TCCR1A) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2outputcompare8(com::diag::amigo::PWM::PIN_1A) != 0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2outputcompare16(com::diag::amigo::PWM::PIN_1A) != &OCR1A) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2offset(com::diag::amigo::PWM::PIN_1A) != COM1A1) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2mask(com::diag::amigo::PWM::PIN_1A) != _BV(COM1A1)) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2timer(com::diag::amigo::PWM::PIN_1A) != com::diag::amigo::PWM::TIMER_1) {
			FAILED(__LINE__);
			break;
		}
#endif
#if defined(TCCR2A) && !defined(portUSE_TIMER2)
		if (com::diag::amigo::PWM::pwm2control(com::diag::amigo::PWM::PIN_2A) != &TCCR2A) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2outputcompare8(com::diag::amigo::PWM::PIN_2A) != &OCR2A) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2outputcompare16(com::diag::amigo::PWM::PIN_2A) != 0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2offset(com::diag::amigo::PWM::PIN_2A) != COM2A1) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2mask(com::diag::amigo::PWM::PIN_2A) != _BV(COM2A1)) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2timer(com::diag::amigo::PWM::PIN_2A) != com::diag::amigo::PWM::TIMER_2) {
			FAILED(__LINE__);
			break;
		}
#endif
		if (com::diag::amigo::PWM::pwm2control(com::diag::amigo::PWM::INVALID) != 0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2outputcompare8(com::diag::amigo::PWM::INVALID) != 0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2outputcompare16(com::diag::amigo::PWM::INVALID) != 0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2offset(com::diag::amigo::PWM::INVALID) != static_cast<uint8_t>(~0)) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2mask(com::diag::amigo::PWM::INVALID) != 0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::pwm2timer(com::diag::amigo::PWM::INVALID) != com::diag::amigo::PWM::NONE) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::PWM::arduino2pwm(~0) != com::diag::amigo::PWM::INVALID) {
			FAILED(__LINE__);
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	// This is a separate test because it requires a test fixture, an operator
	// to watch it, and is specific to the EtherMega 2560 board. It is designed
	// not to conflict with the test fixture for the Digital I/O (GPIO) test.
	// OC0A (Arduino Mega pin 13) pre-hard-wired to the red LED on EtherMega.
	// OC4C (Arduino Mega pin 8) wired to voltmeter or logic analyzer.
	// OC2B (Arduino Mega pin 9) wired to voltmeter or logic analyzer.
	{
		typedef com::diag::amigo::PWM PWM;
		typedef com::diag::amigo::GPIO GPIO;
		do {
			UNITTEST("Analog Output (uses red LED on EtherMega)");
			// LED turns off (if it was on), slowly brightens to its maximum
			// intensity, then slowly dims until it is off. This is the simplest
			// common application of Pulse Width Modulation. I'm betting it's
			// how your Mac laptop pulses its power LED when it's charging.
			// Yet I never fail to be amazed by it.
			if (PWM::arduino2pwm(13) != PWM::PIN_0A) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2gpio(PWM::PIN_0A) != GPIO::PIN_B7) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2control(PWM::PIN_0A) != &TCCR0A) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2outputcompare8(PWM::PIN_0A) != &OCR0A) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2outputcompare16(PWM::PIN_0A) != 0) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2offset(PWM::PIN_0A) != COM0A1) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2mask(PWM::PIN_0A) != _BV(COM0A1)) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2timer(PWM::PIN_0A) != PWM::TIMER_0) {
				FAILED(__LINE__);
				break;
			}
			if (!brightnesscontrol(PWM::arduino2pwm(13), milliseconds2ticks(500))) {
				FAILED(__LINE__);
				break;
			}
			PASSED();
		} while (false);
		do {
			UNITTEST("Analog Output Pin 9 (requires text fixture on EtherMega)");
			// Zero-volts for a few seconds, half-voltage for a few seconds,
			// full-voltage for a few seconds, back to half-voltage for a few
			// seconds, then back to zero-volts. For most Arduinos, like the
			// Mega and compatibles and the Uno, full-voltage is 5V, but there
			// are 3.3V Arduinos too.
			PWM::Pin pin = PWM::arduino2pwm(9);
			if (pin != PWM::PIN_2B) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2gpio(pin) != GPIO::PIN_H6) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2control(pin) != &TCCR2A) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2outputcompare8(pin) != &OCR2B) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2outputcompare16(pin) != 0) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2offset(pin) != COM2B1) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2mask(pin) != _BV(COM2B1)) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2timer(pin) != PWM::TIMER_2) {
				FAILED(__LINE__);
				break;
			}
			PWM pwm(pin);
			if (!pwm) {
				FAILED(__LINE__);
				break;
			}
			if (pwm.prestart() == PWM::NONE) {
				FAILED(__LINE__);
				break;
			}
			static const com::diag::amigo::ticks_t DELAY = milliseconds2ticks(5000);
			(*serialp).write('L');
			pwm.start(PWM::LOW);
			delay(DELAY);
			(*serialp).write('M');
			pwm.start(PWM::HIGH / 2);
			delay(DELAY);
			(*serialp).write('H');
			pwm.start(PWM::HIGH);
			delay(DELAY);
			(*serialp).write('M');
			pwm.start(PWM::HIGH / 2);
			delay(DELAY);
			(*serialp).write('L');
			pwm.start(PWM::LOW);
			delay(DELAY);
			(*serialp).write(' ');
			pwm.stop();
			PASSED();
		} while (false);
		do {
			UNITTEST("Analog Output Pin 8 (requires text fixture on EtherMega)");
			// Zero-volts for a few seconds, half-voltage for a few seconds,
			// full-voltage for a few seconds, back to half-voltage for a few
			// seconds, then back to zero-volts. For most Arduinos, like the
			// Mega and compatibles and the Uno, full-voltage is 5V, but there
			// are 3.3V Arduinos too.
			PWM::Pin pin = PWM::arduino2pwm(8);
			if (pin != PWM::PIN_4C) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2gpio(pin) != GPIO::PIN_H5) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2control(pin) != &TCCR4A) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2outputcompare8(pin) != 0) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2outputcompare16(pin) != &OCR4C) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2offset(pin) != COM4C1) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2mask(pin) != _BV(COM4C1)) {
				FAILED(__LINE__);
				break;
			}
			if (PWM::pwm2timer(pin) != PWM::TIMER_4) {
				FAILED(__LINE__);
				break;
			}
			PWM pwm(pin);
			if (!pwm) {
				FAILED(__LINE__);
				break;
			}
			if (pwm.prestart() == PWM::NONE) {
				FAILED(__LINE__);
				break;
			}
			static const com::diag::amigo::ticks_t DELAY = milliseconds2ticks(5000);
			(*serialp).write('L');
			pwm.start(PWM::LOW);
			delay(DELAY);
			(*serialp).write('M');
			pwm.start(PWM::HIGH / 2);
			delay(DELAY);
			(*serialp).write('H');
			pwm.start(PWM::HIGH);
			delay(DELAY);
			(*serialp).write('M');
			pwm.start(PWM::HIGH / 2);
			delay(DELAY);
			(*serialp).write('L');
			pwm.start(PWM::LOW);
			delay(DELAY);
			(*serialp).write(' ');
			pwm.stop(true);
			PASSED();
		} while (false);
	}
#endif

#if 1
	UNITTEST("A2D (specific to Arduino Uno, Mega etc.)");
	do {
		com::diag::amigo::A2D a2d;
		if (!a2d) {
			FAILED(__LINE__);
			break;
		}
		{
			com::diag::amigo::A2D bogus(com::diag::amigo::A2D::FAIL);
			if (bogus) {
				FAILED(__LINE__);
				break;
			}
		}
#if defined(__AVR_ATmega2560__)
		if (com::diag::amigo::A2D::a2d2gpio(com::diag::amigo::A2D::PIN_0) != com::diag::amigo::GPIO::PIN_F0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::A2D::a2d2gpio(com::diag::amigo::A2D::PIN_7) != com::diag::amigo::GPIO::PIN_F7) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::A2D::a2d2gpio(com::diag::amigo::A2D::PIN_8) != com::diag::amigo::GPIO::PIN_K0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::A2D::a2d2gpio(com::diag::amigo::A2D::PIN_15) != com::diag::amigo::GPIO::PIN_K7) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::A2D::arduino2a2d(54) != com::diag::amigo::A2D::PIN_0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::A2D::arduino2a2d(69) != com::diag::amigo::A2D::PIN_15) {
			FAILED(__LINE__);
			break;
		}
#elif defined(__AVR_ATmega328P__)
		if (com::diag::amigo::A2D::a2d2gpio(com::diag::amigo::A2D::PIN_0) != com::diag::amigo::GPIO::PIN_C0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::A2D::a2d2gpio(com::diag::amigo::A2D::PIN_5) != com::diag::amigo::GPIO::PIN_C5) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::A2D::arduino2a2d(14) != com::diag::amigo::A2D::PIN_0) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::A2D::arduino2a2d(19) != com::diag::amigo::A2D::PIN_5) {
			FAILED(__LINE__);
			break;
		}
#else
#	main must be modified for this microcontroller!
#endif
		if (com::diag::amigo::A2D::a2d2gpio(com::diag::amigo::A2D::INVALID) != com::diag::amigo::GPIO::INVALID) {
			FAILED(__LINE__);
			break;
		}
		if (com::diag::amigo::A2D::arduino2a2d(~0) != com::diag::amigo::A2D::INVALID) {
			FAILED(__LINE__);
			break;
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("Analog Input (requires test fixture on EtherMega)");
	// This is a separate test because it requires a simple text fixture to be
	// wired up on the board. It is specific to the EtherMega 2560 board.
	// PF7 (Arduino Mega digital pin 61 or analog pin A7) wired to 3.3V.
	// PK7 (Arduino Mega digital pin 69 or analog pin A15) wired to ground.
	// A better test would be to combine PWM and A2D but I didn't want the
	// basic tests to depend upon one another.
	do {
		typedef com::diag::amigo::A2D A2D;
		typedef com::diag::amigo::GPIO GPIO;
		A2D::Pin a0 = A2D::arduino2a2d(54);
		if (a0 != A2D::PIN_0) {
			FAILED(__LINE__);
			break;
		}
		if (A2D::a2d2gpio(a0) != GPIO::PIN_F0) {
			FAILED(__LINE__);
			break;
		}
		A2D::Pin a15 = A2D::arduino2a2d(69);
		if (a15 != A2D::PIN_15) {
			FAILED(__LINE__);
			break;
		}
		if (A2D::a2d2gpio(a15) != GPIO::PIN_K7) {
			FAILED(__LINE__);
			break;
		}
		A2D a2d;
		if (!a2d) {
			FAILED(__LINE__);
			break;
		}
		a2d.start();
		int conversion = a2d.convert(a0);
		if (conversion < 0) {
			FAILED(__LINE__);
			break;
		}
		uint16_t sample0 = conversion;
		// Nominally: (3.3V * 1023) / 5.0V = 675 +/- 10% = 607 .. 742.
		// Note that the data sheet specifies 1024 instead of 1023. I can't
		// quite reconcile this in my head, unless it's impossible (and maybe
		// it is) to read 100% of the reference voltage.
		// Reference: ATmega2560 data sheet, 2549N-AVR-05/11, pp. 288-289
		if (!((607 <= sample0) && (sample0 <= 742))) {
			FAILED(__LINE__);
			break;
		}
		conversion = a2d.convert(a15);
		if (conversion < 0) {
			FAILED(__LINE__);
			break;
		}
		uint16_t sample15 = conversion;
		// Nominally: (0V * 1023) / 5.0V = 0.
		if (sample15 != 0) {
			FAILED(__LINE__);
			break;
		}
		// Why would you use this asynchronous interface instead of the
		// synchronous A2D::convert() method? Because if you only have one
		// ADC pin that you're using, you can have a timer periodically
		// request an ADC conversion on your schedule (instead of continuously
		// via ADC::FREE_RUNNING) and then just have your application pick the
		// next converted value off the queue.
		if (a2d.request(a0) != 1) {
			FAILED(__LINE__);
			break;
		}
		if (a2d.request(a15) != 1) {
			FAILED(__LINE__);
			break;
		}
		if (a2d.request(a0) != 1) {
			FAILED(__LINE__);
			break;
		}
		if (a2d.request(a15) != 1) {
			FAILED(__LINE__);
			break;
		}
		// Worst case for an ADC appears to be about half a millisecond:
		// 25 cycles @ 50KHz ~ 500 microseconds.
		// On the 16MHz ATmega2560 with a divisor of 128:
		// 25 cycles @ (16MHz / 128) ~ 200 microseconds.
		// Reference: ATmega2560 data sheet, 2549N-AVR-05/11, pp. 278-279
		delay(milliseconds2ticks(500));
		if (a2d.available() != 4) {
			FAILED(__LINE__);
			break;
		}
		conversion = a2d.conversion(a2d.IMMEDIATELY);
		if (!((607 <= conversion) && (conversion <= 742))) {
			FAILED(__LINE__);
			break;
		}
		if (a2d.available() != 3) {
			FAILED(__LINE__);
			break;
		}
		conversion = a2d.conversion(a2d.IMMEDIATELY);
		if (conversion != 0) {
			FAILED(__LINE__);
			break;
		}
		if (a2d.available() != 2) {
			FAILED(__LINE__);
			break;
		}
		conversion = a2d.conversion(a2d.IMMEDIATELY);
		if (!((607 <= conversion) && (conversion <= 742))) {
			FAILED(__LINE__);
			break;
		}
		if (a2d.available() != 1) {
			FAILED(__LINE__);
			break;
		}
		conversion = a2d.conversion(a2d.IMMEDIATELY);
		if (conversion != 0) {
			FAILED(__LINE__);
			break;
		}
		if (a2d.available() != 0) {
			FAILED(__LINE__);
			break;
		}
		if ((conversion = a2d.conversion(a2d.IMMEDIATELY)) >= 0) {
			FAILED(__LINE__);
			break;
		}
		a2d.stop();
		PASSED();
		printf(PSTR("A0=%u=%u.%uV\n"), sample0, (5 * sample0) / 1023, ((5 * sample0 * 10) / 1023) % 10);
		printf(PSTR("A15=%u=%u.%uV\n"), sample15, (5 * sample15) / 1023, ((5 * sample15 * 10) / 1023) % 10);
	} while (false);
#endif

#if 1
	UNITTEST("SPI (sanity)");
	do {
		com::diag::amigo::SPI spi;
		if (!spi) {
			FAILED(__LINE__);
			break;
		}
		{
			com::diag::amigo::SPI bogus(com::diag::amigo::SPI::FAIL);
			if (bogus) {
				FAILED(__LINE__);
				break;
			}
		}
		PASSED();
	} while (false);
#endif

#if 1
	UNITTEST("SPI (requires WIZnet W5100)");
	do {
		com::diag::amigo::SPI spi;
		spi.start();
		do {
			W5100 w5100(*mutexsemaphorep, com::diag::amigo::GPIO::arduino2gpio(10), spi);
			static const uint16_t RTR0 = 0x0017;
			static const uint16_t RTR1 = 0x0018;
			static const uint16_t RCR = 0x0019;
			uint8_t rtr0 = w5100.read(RTR0);
			uint8_t rtr1 = w5100.read(RTR1);
			uint8_t rcr = w5100.read(RCR);
			// Reference: WIZnet, W5100 Datasheet, Version 1.2.4, 2011
			if (rtr0 != 0x07) {
				FAILED(__LINE__);
				break;
			}
			if (rtr1 != 0xd0) {
				FAILED(__LINE__);
				break;
			}
			if (rcr != 0x08) {
				FAILED(__LINE__);
				break;
			}
			PASSED();
		} while (false);
		spi.stop();
	} while (false);
#endif

#if 1
	UNITTEST("W5100 (requires WIZnet W5100)");
	do {
		com::diag::amigo::SPI spi;
		com::diag::amigo::W5100::W5100 w5100(*mutexsemaphorep, com::diag::amigo::GPIO::arduino2gpio(10), spi);
		spi.start();
		w5100.start();
		do {
			// Reference: WIZnet, W5100 Datasheet, Version 1.2.4, 2011
			uint16_t rtr = w5100.getRetransmissionTime();
			if (rtr != 2000) {
				FAILED(__LINE__);
				break;
			}
			uint8_t rcr = w5100.getRetransmissionCount();
			if (rcr != 8) {
				FAILED(__LINE__);
				break;
			}
			PASSED();
		} while (false);
		w5100.stop();
		spi.stop();
	} while (false);
#endif

#if 1
	UNITTEST("Socket (requires internet connectivity)");
	// On typical larger system I would test this against 127.0.0.1 to
	// make sure I can talk to myself. But that implies a bunch more
	// infrastructure than we have on this tiny system at this point.
	// So instead I try to talk to q well known web server available
	// at a single IP address (so it is unlikely to be down or to
	// change). Your mileage may vary.
	do {
		com::diag::amigo::SPI spi;
		com::diag::amigo::W5100::W5100 w5100(*mutexsemaphorep, com::diag::amigo::GPIO::PIN_B4, spi);
		com::diag::amigo::W5100::Socket w5100socket(w5100);
		com::diag::amigo::Socket & socket = w5100socket;
		spi.start();
		w5100.start();
		// It's funny, thinking about how to store this stuff. Here, I place
		// them in data space. But they're actually copied from program space
		// to data space during run-time initialization. I could put them
		// explicitly in program space, but I'd still have to explicitly copy
		// them to local variables in data space. But then they'd be on the
		// stack, and stack space is precious. This architecture really forces
		// you to think about how stuff works under the hood.
		static const uint8_t GATEWAY[] = { 192, 168, 1, 1 };
		static const uint8_t SUBNET[] = { 255, 255, 255, 0 };
		static const uint8_t MACADDRESS[] = { 0x90, 0xa2, 0xda, 0x0d, 0x03, 0x4c };
		static const uint8_t IPADDRESS[] = { 192, 168, 1, 253 };
		static const uint8_t WEBSERVER[] = { 129, 42, 60, 216 }; // www.ibm.com
		static const com::diag::amigo::Socket::port_t HTTP = 80;
		com::diag::amigo::Socket::State state;
		do {
			w5100.setGatewayIp(GATEWAY);
			w5100.setSubnetMask(SUBNET);
			w5100.setMACAddress(MACADDRESS);
			w5100.setIPAddress(IPADDRESS);
			if (w5100socket) {
				FAILED(__LINE__);
				break;
			}
			for (com::diag::amigo::W5100::Socket::socket_t sock = 0; sock <= w5100.SOCKETS; ++sock) {
				w5100socket = sock;
				state = socket.state();
				if ((state == socket.STATE_CLOSED) || (state == socket.STATE_FIN_WAIT)) {
					break;
				}
			}
			if (!w5100socket) {
				FAILED(__LINE__);
				break;
			}
			com::diag::amigo::Socket::port_t port = socket.allocate();
			if (port == 0) {
				FAILED(__LINE__);
				break;
			}
			if (!socket.socket(socket.PROTOCOL_TCP, port)) {
				FAILED(__LINE__);
				break;
			}
			if (!socket.connect(WEBSERVER, HTTP)) {
				FAILED(__LINE__);
				break;
			}
			while (socket.state() != socket.STATE_ESTABLISHED) {
				delay(milliseconds2ticks(10));
				if (socket.state() == socket.STATE_CLOSED) {
					FAILED(__LINE__);
					break;
				}
			}
			state = socket.state();
			if ((state == socket.STATE_LISTEN) || (state == socket.STATE_CLOSED) || (state == socket.STATE_FIN_WAIT)) {
				FAILED(__LINE__);
				break;
			}
			// Reference: T. Berners-Lee et al., "Hypertext Transfer Protocol -- HTTP/1.0", RFC1945, May 1996
			static const char GET[] = "GET /expect404.html HTTP/1.0\r\nFrom: coverclock@diag.com\r\nUser-Agent: Amigo/1.0\r\n\r\n";
			ssize_t sent = socket.send(GET, sizeof(GET) - 1 /* Not including terminating NUL. */);
			if (sent != (sizeof(GET) - 1)) {
				FAILED(__LINE__);
				break;
			}
			for (uint8_t ii = 0; (ii < 100) && (socket.available() == 0); ++ii) {
				delay(milliseconds2ticks(100));
			}
			if (socket.available() == 0) {
				FAILED(__LINE__);
				break;
			}
			// If WEBSERVER ever updates their HTTP version to something other
			// than 1.1 this will have to change.
			static const char EXPECTED[] = "HTTP/1.1 404 Not Found";
			char buffer[sizeof(EXPECTED)];
			ssize_t received = socket.recv(buffer, sizeof(buffer) - 1 /* Not including terminating NUL. */);
			if (!((0 < received) && (received < sizeof(buffer)))) {
				FAILED(__LINE__);
				break;
			}
			buffer[received] = '\0';
			if (strcmp(buffer, EXPECTED) != 0) {
				FAILED(__LINE__);
				serialsink.write(buffer);
				serialsink.write("\r\n");
				break;
			}
			PASSED();
		} while (false);
		socket.disconnect();
		if (!w5100socket) {
			FAILED(__LINE__);
			break;
		}
		socket.close();
		if (w5100socket) {
			FAILED(__LINE__);
			break;
		}
		w5100.stop();
		spi.stop();
	} while (false);
#endif

	printf(PSTR("Unit Test errors=%d (so far)\n"), errors);

#if 1
	UNITTESTLN("Source (type <control d> to exit)");
	if (!source2sink(serialsource, serialsink)) {
		FAILED(__LINE__);
	} else {
		PASSED();
	}
#endif

	printf(PSTR("Unit Test errors=%d\n"), errors);

	// Just to make sure the regular data memory Print works.
	com::diag::amigo::Print testf(serialsink);
	testf("Type <control a><control \\>y to exit Mac screen utility.\n");

	serialp->flush();
}

/*******************************************************************************
 * MAIN PROGRAM
 ******************************************************************************/

int main() __attribute__((OS_main));
int main() {
	// The constructor of Scope emits our start up message. You may see this
	// message more than once, or see some garbage printed before this message.
	// This is because on a stock Arduino or clone, connecting to the board via
	// the USB cable resets the AVR microcontroller by design. But there is some
	// latency in the AVR resetting, so it can actually start running its
	// software and emit some characters before this reset occurs, after which
	// it starts all over and remits them. There is also some lag between the
	// reset, which actually starts the bootloader in a stock Arduino, and the
	// timout after which the bootloader transfers control to your software.
	// During this the bootloader may emit some bytes to talk to avrdude, and
	// those bytes may be unprintable.
	Scope scope;

	// Enable interrupts system-wide.
	com::diag::amigo::interrupts::enable();

	// The fact that main() never exits means local variables allocated on
	// its stack in its outermost braces never go out of scope. We can take
	// advantage of main to allocate objects on its stack that persist until
	// the system is rebooted and pass pointers to them to tasks. That's useful
	// to know, especially in light of the fact that the memory allocated
	// for main's stack is never otherwise recovered or used.
	com::diag::amigo::Serial serial;
	serialp = &serial;

	com::diag::amigo::BinarySemaphore binarysemaphore;
	binarysemaphorep = &binarysemaphore;

	com::diag::amigo::MutexSemaphore mutexsemaphore;
	mutexsemaphorep = &mutexsemaphore;

	do {
		takertask.start();
		unittesttask.start();
		if (takertask != true) {
			break;
		}
		if (unittesttask != true) {
			break;
		}
		com::diag::amigo::Task::begin();
		// Should never get here unless the Task::start() or Task::begin()
		// methods fail, or the Task::begin() methods returns. All are
		// fatal errors.
	} while (false);

	// scope going out of lexical scope will FATAL in its destructor.
}
