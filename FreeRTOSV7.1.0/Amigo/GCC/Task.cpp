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

void Task::trampoline(void * instance) {
	Task * that = static_cast<Task *>(instance);
	that->result = that->task();
	void * temporary = that->handle;
	that->handle = 0;
#if (INCLUDE_vTaskDelete == 1)
	vTaskDelete(temporary);
#else
	(void)temporary;
#endif
	while (!0) { taskYIELD(); }
}

Task::Task(const char * myname)
: name(myname)
, handle(0)
, function(0)
, parameter(0)
, result(0)
{}

Task::~Task() {
	if (handle != 0) {
		com::diag::amigo::fatal(__FILE__, __LINE__);
	}
}

void Task::start(void * (*myfunction)(void *), void * myparameter, unsigned short depth, unsigned char priority) {
	function = myfunction;
	parameter = myparameter;
	if (xTaskCreate(trampoline, (const signed char *)name, depth, this, priority, &handle) != pdPASS) {
		handle = 0;
	}
}

void * Task::task() {
	return ((function != 0) ? (*function)(parameter) : 0);
}

}
}
}
