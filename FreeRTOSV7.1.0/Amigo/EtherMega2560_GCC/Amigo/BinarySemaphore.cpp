/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/BinarySemaphore.h"

namespace com {
namespace diag {
namespace amigo {

BinarySemaphore::BinarySemaphore() {
	vSemaphoreCreateBinary(handle);
}

BinarySemaphore::~BinarySemaphore() {
	vSemaphoreDelete(handle);
	handle = static_cast<xSemaphoreHandle>(0);
}

bool BinarySemaphore::take(Ticks timeout) {
	return (xSemaphoreTake(handle, timeout) == pdPASS);
}

bool BinarySemaphore::give() {
	return (xSemaphoreGive(handle) == pdPASS);
}

bool BinarySemaphore::giveFromISR(bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xSemaphoreGiveFromISR(handle, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

}
}
}
