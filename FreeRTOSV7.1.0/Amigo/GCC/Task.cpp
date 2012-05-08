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
// I also note that when I enabled -mrelax on the linker the call to this
// class method was reduced to a jump from the trampoline. It's kind of a cheap
// form of tail recursion optimization. So: not so much overhead.
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
	// whether it is or not, we delay indefinitely.
	while (!0) { delay(EPOCH); }
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
: handle(0)
, name(myname)
, stopping(false)
, proxy(false)
{}

Task::Task(xTaskHandle myhandle)
: handle(myhandle)
, name(reinterpret_cast<const char *>(pcTaskGetTaskName(handle))) // reinterpret_cast? Srsly?
, stopping(false)
, proxy(true)
{}

Task::Task()
: handle(xTaskGetCurrentTaskHandle())
, name(reinterpret_cast<const char *>(pcTaskGetTaskName(handle))) // reinterpret_cast? Srsly?
, stopping(false)
, proxy(true)
{}

Task::~Task() {
	if (proxy) {
		// Do nothing.
	} else if (handle != 0) {
		// It is a fatal error to delete the Task object of a running task.
		com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
	} else {
		// Do nothing.
	}
}

void Task::start(size_t mydepth, priority_t mypriority) {
	// We don't check to see if the task is already started. Might be a good
	// idea, but then we'd have to return a boolean since the handle value
	// would not reflect the correct state.
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
