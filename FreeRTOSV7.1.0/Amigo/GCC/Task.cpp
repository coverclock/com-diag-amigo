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

inline void Task::task(Task * that) {
	that->task();
#if (INCLUDE_vTaskDelete == 1)
	void * handle = that->handle;
	that->handle = 0;
	vTaskDelete(handle);
#else
	that->handle = 0;
#endif
}

#if defined(__AVR_3_BYTE_PC__)
extern "C" void amigo_task_trampoline(Task * that) __attribute__((section(".lowtext")));
#else
extern "C" void amigo_task_trampoline(Task * that);
#endif

extern "C" void amigo_task_trampoline(Task * that) {
	Task::task(that);
	while (!0) { taskYIELD(); }
}

Task::Task(const char * myname)
: name(myname)
, handle(0)
, stopping(false)
{}

Task::~Task() {
	if (handle != 0) {
		com::diag::amigo::fatal(__FILE__, __LINE__);
	}
}

void Task::start(unsigned short depth, unsigned char priority) {
	if (xTaskCreate(reinterpret_cast<void(*)(void*)>(amigo_task_trampoline), (const signed char *)name, depth, this, priority, &handle) != pdPASS) {
		handle = 0;
	}
}

void Task::task() {
}

}
}
}
