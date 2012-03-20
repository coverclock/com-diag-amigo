/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/MutexSemaphore.h"

namespace com {
namespace diag {
namespace amigo {

MutexSemaphore::MutexSemaphore() {
	handle = xSemaphoreCreateRecursiveMutex();
}

MutexSemaphore::~MutexSemaphore() {
	vSemaphoreDelete(handle);
	handle = static_cast<xSemaphoreHandle>(0);
}

bool MutexSemaphore::take(Ticks timeout) {
	return (xSemaphoreTakeRecursive(handle, timeout) == pdPASS);
}

bool MutexSemaphore::give() {
	return (xSemaphoreGiveRecursive(handle) == pdPASS);
}

}
}
}
