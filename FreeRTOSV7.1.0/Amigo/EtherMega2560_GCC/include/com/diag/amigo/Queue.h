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
#include "com/diag/amigo/values.h"
#include "com/diag/amigo/unused.h"

namespace com {
namespace diag {
namespace amigo {

class Queue {

public:

	explicit Queue(Count count, Size size = 1, const signed char * name = NULL);

	virtual ~Queue();

	operator bool() const { return (handle != 0); }

	bool isEmptyFromISR() const;

	bool isFullFromISR() const;

	Count available() const;

	Count availableFromISR() const;

	bool peek(void * buf, Ticks timeout = IMMEDIATELY);

	bool receive(void * buf, Ticks timeout = NEVER);

	bool receiveFromISR(void * buf, bool & woken = unused.b);

	bool send(const void * dat, Ticks timeout = NEVER);

	bool sendFromISR(const void * dat, bool & woken = unused.b);

	bool express(const void * dat, Ticks timeout = NEVER);

	bool expressFromISR(const void * dat, bool & woken = unused.b);

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

inline bool Queue::peek(void * buf, Ticks timeout) {
	return (xQueuePeek(handle, buf, timeout) == pdPASS);
}

inline bool Queue::receive(void * buf, Ticks timeout) {
	return (xQueueReceive(handle, buf, timeout) == pdPASS);
}

inline bool Queue::send(const void * dat, Ticks timeout) {
	return (xQueueSendToBack(handle, dat, timeout) == pdPASS);
}

inline bool Queue::express(const void * dat, Ticks timeout) {
	return (xQueueSendToFront(handle, dat, timeout) == pdPASS);
}

inline bool Queue::receiveFromISR(void * buf, bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xQueueReceiveFromISR(handle, buf, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

inline bool Queue::sendFromISR(const void * dat, bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xQueueSendToBackFromISR(handle, dat, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

inline bool Queue::expressFromISR(const void * dat, bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xQueueSendToFrontFromISR(handle, dat, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

}
}
}

#endif /* _COM_DIAG_AMIGO_QUEUE_H_ */
