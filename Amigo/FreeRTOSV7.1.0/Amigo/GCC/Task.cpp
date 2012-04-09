/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Task.h"
#include "com/diag/amigo/fatal.h"

namespace com {
namespace diag {
namespace amigo {

// We declare this to be inline. But in the disassembly I noticed that the
// compiler outlined it, and did not issue a warning that it had done so. I
// can rationalize this by saying this conforms the definition of a class
// (static) method. But this is not the behavior I have seen in GCC elsewhere.
inline void Task::task(Task * that) {
	that->task();
#if (INCLUDE_vTaskDelete == 1)
	void * handle = that->handle;
	that->handle = 0;
	vTaskDelete(handle);
#else
	that->handle = 0;
#endif
	// If task deletion is enabled in FreeRTOS we should never get here. But
	// whether it is or not, we delay literally forever.
	while (!0) { delay(FOREVER); }
}

#if defined(__AVR_3_BYTE_PC__)
extern "C" void amigo_task_trampoline(Task * that) __attribute__((section(".lowtext")));
#else
extern "C" void amigo_task_trampoline(Task * that);
#endif

extern "C" void amigo_task_trampoline(Task * that) {
	Task::task(that);
}

Task::Task(const char * myname)
: name(myname)
, handle(0)
, stopping(false)
{}

Task::~Task() {
	if (handle != 0) {
		com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
	}
}

void Task::start(unsigned int mydepth, unsigned int mypriority) {
	// We cast the function pointer type here instead of casting the parameter
	// pointer type in the trampoline function.
	if (xTaskCreate(reinterpret_cast<void(*)(void*)>(amigo_task_trampoline), (const signed char *)name, mydepth, this, mypriority, &handle) != pdPASS) {
		handle = 0;
	}
}

void Task::task() {
}

}
}
}
