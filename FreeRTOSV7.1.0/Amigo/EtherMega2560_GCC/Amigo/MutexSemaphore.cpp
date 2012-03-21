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
	handle = 0;
}

}
}
}
