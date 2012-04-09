#ifndef _COM_DIAG_AMIGO_TYPEDQUEUE_H_
#define _COM_DIAG_AMIGO_TYPEDQUEUE_H_

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

/**
 * A TypedQueue is a type-safe Queue. The size of each individual element on
 * the underlying Queue is the sizeof() of the specified _TYPE_. The TypedQueue
 * methods that handle elements are type-safe.
 */
template <typename _TYPE_>
class TypedQueue
: public Queue
{

public:

	/**
	 * Constructor. The size of each element on the TypedQueue is the sizeof()
	 * the _TYPE_ type.
	 * @param count is the maximum number of elements in the Queue.
	 * @param name is the optional name of the Queue.
	 */
	explicit TypedQueue(Count count, const signed char * name = 0)
	: Queue(count, sizeof(_TYPE_), name)
	{}

	/**
	 * Destructor.
	 */
	virtual ~TypedQueue() {}

	/**
	 * Return the first element in the Queue without removing it from the
	 * Queue. This is vastly useful when implementing certain
	 * classes of parsers. I have always suspected it is the reason why UNIX
	 * I/O streams implemented the ungetch() function.
	 * @param buffer points to the data memory into which the element is copied.
	 * @param timeout is the duration in ticks to wait if the Queue is empty.
	 * @return true if an element was copied into the buffer, false otherwise.
	 */
	bool peek(_TYPE_ * buffer, Ticks timeout = IMMEDIATELY) { return Queue::peek(buffer, timeout); }

	/**
	 * Return the first element in the Queue and remove it from the Queue.
	 * @param buffer points to the data memory into which the element is copied.
	 * @param timeout is the duration in ticks to wait if the Queue is empty.
	 * @return true if an element was copied into the buffer, false otherwise.
	 */
	bool receive(_TYPE_ * buffer, Ticks timeout = NEVER) { return Queue::receive(buffer, timeout); }

	/**
	 * Return the first element in the Queue and remove it from the Queue. This
	 * is guaranteed to be non-blocking and should only be called from an
	 * interrupt service routine.
	 * @param buffer points to the data memory into which the element is copied.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if an element was copied into the buffer, false otherwise.
	 */
	bool receiveFromISR(_TYPE_ * buffer, bool & woken = unused.b) { return Queue::receiveFromISR(buffer, woken); }

	/**
	 * Append an element to the end of the Queue.
	 * @param datum points to the data memory from which the element is copied.
	 * @param timeout is the duration in ticks to wait if the Queue is full.
	 * @return true if an element was copied from the datum, false otherwise.
	 */
	bool send(const _TYPE_ * datum, Ticks timeout = NEVER) { return Queue::send(datum, timeout); }

	/**
	 * Append an element to the end of the Queue. THis is guaranteed to be
	 * non-blocking and should only be called from an interrupt service routine.
	 * @param datum points to the data memory from which the element is copied.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if an element was copied from the datum, false otherwise.
	 */
	bool sendFromISR(const _TYPE_ * datum, bool & woken = unused.b) { return Queue::sendFromISR(datum, woken); }

	/**
	 * Prepend an element to the beginning of the Queue. This can be used to
	 * send a datum out of band.
	 * @param datum points to the data memory from which the element is copied.
	 * @param timeout is the duration in ticks to wait if the Queue is full.
	 * @return true if an element was copied from the datum, false otherwise.
	 */
	bool express(const _TYPE_ * datum, Ticks timeout = NEVER) { return Queue::express(datum, timeout); }

	/**
	 * Prepend an element to the beginning of the Queue. This can be used to
	 * send a datum out of band. This is guaranteed to be non-blocking and
	 * should only be called from an interrupt service routine.
	 * @param datum points to the data memory from which the element is copied.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if an element was copied from the datum, false otherwise.
	 */
	bool expressFromISR(const _TYPE_ * datum, bool & woken = unused.b) { return Queue::expressFromISR(datum, woken); }

};

}
}
}

#endif /* _COM_DIAG_AMIGO_TYPEDQUEUE_H_ */
