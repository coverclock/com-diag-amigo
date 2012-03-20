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

CountingSemaphore::CountingSemaphore(Count maximum, Count initial) {
	handle = xSemaphoreCreateCounting(maximum, initial);
}

CountingSemaphore::~CountingSemaphore() {
	vSemaphoreDelete(handle);
	handle = 0;
}

bool CountingSemaphore::take(Ticks timeout) {
	return (xSemaphoreTake(handle, timeout) == pdPASS);
}

bool CountingSemaphore::give() {
	return (xSemaphoreGive(handle) == pdPASS);
}

bool CountingSemaphore::giveFromISR(bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xSemaphoreGiveFromISR(handle, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

}
}
}
