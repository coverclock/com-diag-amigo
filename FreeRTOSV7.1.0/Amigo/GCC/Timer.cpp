/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Timer.h"
#include "com/diag/amigo/fatal.h"

namespace com {
namespace diag {
namespace amigo {

int Timer::errors = 0;

// We declare this to be inline. I haven't checked, but in similar code in
// Task.cpp the compiler actually outlined it.
inline void Timer::timer(Timer * that) {
	that->timer();
}

#if defined(__AVR_3_BYTE_PC__)
extern "C" void amigo_timer_trampoline(xTimerHandle handle) __attribute__((section(".lowtext")));
#else
extern "C" void amigo_timer_trampoline(xTimerHandle handle);
#endif

extern "C" void amigo_timer_trampoline(xTimerHandle handle) {
	Timer::timer(static_cast<Timer*>(pvTimerGetTimerID(handle)));
}

Timer::Timer(ticks_t duration, bool periodic, const char * myname)
: handle(0)
, name(myname)
{
	handle = xTimerCreate((const signed char *)name, duration, periodic ? pdTRUE : pdFALSE, this, reinterpret_cast<void(*)(void*)>(amigo_timer_trampoline));
}

Timer::~Timer() {
	// If this fails, it's not fatal, but it may be a resource leak depending on
	// the reason for the failure.
	if (xTimerDelete(handle, DESTRUCTION) != pdPASS) {
		++errors;
	}
	// It is a fatal error to delete a Timer object associated with an active
	// timer. This implies that the timer delete above failed. It is possible
	// for the delete to fail but the timer not be active, which is a non-fatal
	// resource leak.
	if (xTimerIsTimerActive(handle) != pdFALSE) {
		com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
	}
}

void Timer::timer() {
}

PeriodicTimer::~PeriodicTimer() {
}

OneShotTimer::~OneShotTimer() {
}

}
}
}
