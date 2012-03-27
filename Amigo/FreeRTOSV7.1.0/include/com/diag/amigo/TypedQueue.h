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

template <typename _TYPE_>
class TypedQueue
: public Queue
{

public:

	explicit TypedQueue(Count count, const signed char * name = NULL)
	: Queue(count, sizeof(_TYPE_), name)
	{}

	virtual ~TypedQueue() {}

	bool peek(_TYPE_ * buffer, Ticks timeout = IMMEDIATELY) { return Queue::peek(buffer, timeout); }

	bool receive(_TYPE_ * buffer, Ticks timeout = NEVER) { return Queue::receive(buffer, timeout); }

	bool receiveFromISR(_TYPE_ * buffer, bool & woken = unused.b) { return Queue::receiveFromISR(buffer, woken); }

	bool send(const _TYPE_ * datum, Ticks timeout = NEVER) { return Queue::send(datum, timeout); }

	bool sendFromISR(const _TYPE_ * datum, bool & woken = unused.b) { return Queue::sendFromISR(datum, woken); }

	bool express(const _TYPE_ * datum, Ticks timeout = NEVER) { return Queue::express(datum, timeout); }

	bool expressFromISR(const _TYPE_ * datum, bool & woken = unused.b) { return Queue::expressFromISR(datum, woken); }

};

}
}
}

#endif /* _COM_DIAG_AMIGO_TYPEDQUEUE_H_ */
