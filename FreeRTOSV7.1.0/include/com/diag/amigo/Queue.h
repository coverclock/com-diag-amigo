#ifndef _COM_DIAG_AMIGO_QUEUE_H_
#define _COM_DIAG_AMIGO_QUEUE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/unused.h"

namespace com {
namespace diag {
namespace amigo {

class Queue
{

public:

	static const Ticks IMMEDIATELY = 0;

	static const Ticks NEVER = portMAX_DELAY; // Nominally ~0.

	explicit Queue(Count count, Size size = 1, const signed char * name = NULL);

	virtual ~Queue();

	operator bool() const { return (handle != 0); }

	bool isEmptyFromISR() const;

	bool isFullFromISR() const;

	Count available() const;

	Count availableFromISR() const;

	bool peek(void * buffer, Ticks timeout = IMMEDIATELY);

	bool receive(void * buffer, Ticks timeout = NEVER);

	bool receiveFromISR(void * buffer, bool & woken = unused.b);

	bool send(const void * datum, Ticks timeout = NEVER);

	bool sendFromISR(const void * datum, bool & woken = unused.b);

	bool express(const void * datum, Ticks timeout = NEVER);

	bool expressFromISR(const void * datum, bool & woken = unused.b);

protected:

	xQueueHandle handle;

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Queue(const Queue& that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Queue& operator=(const Queue& that);

};

inline bool Queue::isEmptyFromISR() const {
	return (xQueueIsQueueEmptyFromISR(handle) == pdFALSE);
}

inline bool Queue::isFullFromISR() const {
	return (xQueueIsQueueFullFromISR(handle) != pdFALSE);
}

inline Count Queue::available() const {
	return uxQueueMessagesWaiting(handle);
}

inline Count Queue::availableFromISR() const {
	return uxQueueMessagesWaitingFromISR(handle);
}

inline bool Queue::peek(void * buffer, Ticks timeout) {
	return (xQueuePeek(handle, buffer, timeout) == pdPASS);
}

inline bool Queue::receive(void * buffer, Ticks timeout) {
	return (xQueueReceive(handle, buffer, timeout) == pdPASS);
}

inline bool Queue::send(const void * datum, Ticks timeout) {
	return (xQueueSendToBack(handle, datum, timeout) == pdPASS);
}

inline bool Queue::express(const void * datum, Ticks timeout) {
	return (xQueueSendToFront(handle, datum, timeout) == pdPASS);
}

inline bool Queue::receiveFromISR(void * buffer, bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xQueueReceiveFromISR(handle, buffer, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

inline bool Queue::sendFromISR(const void * datum, bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xQueueSendToBackFromISR(handle, datum, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

inline bool Queue::expressFromISR(const void * datum, bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xQueueSendToFrontFromISR(handle, datum, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

}
}
}

#endif /* _COM_DIAG_AMIGO_QUEUE_H_ */
