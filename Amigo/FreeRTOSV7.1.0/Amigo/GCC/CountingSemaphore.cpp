/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <FreeRTOS.h>
#include "com/diag/amigo/CountingSemaphore.h"

namespace com {
namespace diag {
namespace amigo {

CountingSemaphore::CountingSemaphore(size_t maximum, size_t initial) {
	handle = xSemaphoreCreateCounting(maximum, initial);
}

CountingSemaphore::~CountingSemaphore() {
	vSemaphoreDelete(handle);
	handle = 0;
}

}
}
}
