/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Queue.h"

namespace com {
namespace diag {
namespace amigo {

Queue::Queue(Count count, Size size, const signed char * name) {
	handle = xQueueCreate(count, size);
	if ((handle != 0) && (name != 0)) {
		vQueueAddToRegistry(handle, name);
	}
}

Queue::~Queue() {
	vQueueDelete(handle);
	handle = 0;
}

bool Queue::isEmptyFromISR() const {
	return (xQueueIsQueueEmptyFromISR(handle) == pdFALSE);
}

bool Queue::isFullFromISR() const {
	return (xQueueIsQueueFullFromISR(handle) != pdFALSE);
}

Count Queue::available() const {
	return uxQueueMessagesWaiting(handle);
}

Count Queue::availableFromISR() const {
	return uxQueueMessagesWaitingFromISR(handle);
}

bool Queue::peek(void * buf, Ticks timeout) {
	return (xQueuePeek(handle, buf, timeout) == pdPASS);
}

bool Queue::receive(void * buf, Ticks timeout) {
	return (xQueueReceive(handle, buf, timeout) == pdPASS);
}

bool Queue::receiveFromISR(void * buf, bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xQueueReceiveFromISR(handle, buf, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

bool Queue::send(const void * dat, Ticks timeout) {
	return (xQueueSendToBack(handle, dat, timeout) == pdPASS);
}

bool Queue::sendFromISR(const void * dat, bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xQueueSendToBackFromISR(handle, dat, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

bool Queue::express(const void * dat, Ticks timeout) {
	return (xQueueSendToFront(handle, dat, timeout) == pdPASS);
}

bool Queue::expressFromISR(const void * dat, bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xQueueSendToFrontFromISR(handle, dat, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

}
}
}
