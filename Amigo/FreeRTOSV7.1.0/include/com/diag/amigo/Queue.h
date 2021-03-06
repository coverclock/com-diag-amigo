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
#include "com/diag/amigo/constants.h"
#include "com/diag/amigo/unused.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * Queue encapsulates a FreeRTOS queue, which implements a ring buffer. Queues
 * are used as data channels between concurrent tasks, or between a task and an
 * interrupt service routine. Elements sent to or received from a particular
 * Queue are of a fixed size, and each Queue contains a fixed number of such
 * elements.
 */
class Queue
{

public:

	/**
	 * Constructor.
	 * @param count is the maximum number of elements in the Queue.
	 * @param size is the size of each element in the Queue in bytes.
	 * @param name is the optional name of the Queue.
	 */
	explicit Queue(size_t count, size_t size = 1, const signed char * name = 0);

	/**
	 * Destructor.
	 */
	virtual ~Queue();

	/**
	 * Returns true if the construction of the Queue was successful.
	 * @return true if successful, false otherwise.
	 */
	operator bool() const { return (handle != 0); }

	/**
	 * Return true if the Queue is empty. Can only be called by an interrupt
	 * service routine.
	 * @return true if empty, false otherwise.
	 */
	bool isEmptyFromISR() const;

	/**
	 * Return true if the Queue is full. Can only be called by an interrupt
	 * service routine.
	 * @return true if full, false otherwise.
	 */
	bool isFullFromISR() const;

	/**
	 * Return the number of elements in the Queue.
	 * @return the number of elements in the Queue.
	 */
	size_t available() const;

	/**
	 * Return the number of elements in the Queue. Can only be called by an
	 * interrupt service routine.
	 * @return the number of elements in the Queue.
	 */
	size_t availableFromISR() const;

	/**
	 * Return the first element in the Queue without removing it from the
	 * Queue. This is vastly useful when implementing certain
	 * classes of parsers. I have always suspected it is the reason why UNIX
	 * I/O streams implemented the ungetch() function.
	 * @param buffer points to the data memory into which the element is copied.
	 * @param timeout is the duration in ticks to wait if the Queue is empty.
	 * @return true if an element was copied into the buffer, false otherwise.
	 */
	bool peek(void * buffer, ticks_t timeout = IMMEDIATELY);

	/**
	 * Return the first element in the Queue and remove it from the Queue.
	 * @param buffer points to the data memory into which the element is copied.
	 * @param timeout is the duration in ticks to wait if the Queue is empty.
	 * @return true if an element was copied into the buffer, false otherwise.
	 */
	bool receive(void * buffer, ticks_t timeout = NEVER);

	/**
	 * Return the first element in the Queue and remove it from the Queue. This
	 * is guaranteed to be non-blocking and should only be called from an
	 * interrupt service routine.
	 * @param buffer points to the data memory into which the element is copied.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if an element was copied into the buffer, false otherwise.
	 */
	bool receiveFromISR(void * buffer, bool & woken = unused.b);

	/**
	 * Append an element to the end of the Queue.
	 * @param datum points to the data memory from which the element is copied.
	 * @param timeout is the duration in ticks to wait if the Queue is full.
	 * @return true if an element was copied from the datum, false otherwise.
	 */
	bool send(const void * datum, ticks_t timeout = NEVER);

	/**
	 * Append an element to the end of the Queue. THis is guaranteed to be
	 * non-blocking and should only be called from an interrupt service routine.
	 * @param datum points to the data memory from which the element is copied.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if an element was copied from the datum, false otherwise.
	 */
	bool sendFromISR(const void * datum, bool & woken = unused.b);

	/**
	 * Prepend an element to the beginning of the Queue. This can be used to
	 * send a datum out of band.
	 * @param datum points to the data memory from which the element is copied.
	 * @param timeout is the duration in ticks to wait if the Queue is full.
	 * @return true if an element was copied from the datum, false otherwise.
	 */
	bool express(const void * datum, ticks_t timeout = NEVER);

	/**
	 * Prepend an element to the beginning of the Queue. This can be used to
	 * send a datum out of band. This is guaranteed to be non-blocking and
	 * should only be called from an interrupt service routine.
	 * @param datum points to the data memory from which the element is copied.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if an element was copied from the datum, false otherwise.
	 */
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

inline size_t Queue::available() const {
	return uxQueueMessagesWaiting(handle);
}

inline size_t Queue::availableFromISR() const {
	return uxQueueMessagesWaitingFromISR(handle);
}

inline bool Queue::peek(void * buffer, ticks_t timeout) {
	return (xQueuePeek(handle, buffer, timeout) == pdPASS);
}

inline bool Queue::receive(void * buffer, ticks_t timeout) {
	return (xQueueReceive(handle, buffer, timeout) == pdPASS);
}

inline bool Queue::send(const void * datum, ticks_t timeout) {
	return (xQueueSendToBack(handle, datum, timeout) == pdPASS);
}

inline bool Queue::express(const void * datum, ticks_t timeout) {
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
