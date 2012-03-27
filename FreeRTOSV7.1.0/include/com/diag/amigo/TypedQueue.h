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
: public Queue {

public:

	explicit TypedQueue(Count count, const signed char * name = NULL)
	: Queue(count, sizeof(_TYPE_), name)
	{}

	virtual ~TypedQueue() {}

	bool peek(_TYPE_ * buf, Ticks timeout = IMMEDIATELY) { return Queue::peek(buf, timeout); }

	bool receive(_TYPE_ * buf, Ticks timeout = NEVER) { return Queue::receive(buf, timeout); }

	bool receiveFromISR(_TYPE_ * buf, bool & woken = unused.b) { return Queue::receiveFromISR(buf, woken); }

	bool send(const _TYPE_ * dat, Ticks timeout = NEVER) { return Queue::send(dat, timeout); }

	bool sendFromISR(const _TYPE_ * dat, bool & woken = unused.b) { return Queue::sendFromISR(dat, woken); }

	bool express(const _TYPE_ * dat, Ticks timeout = NEVER) { return Queue::express(dat, timeout); }

	bool expressFromISR(const _TYPE_ * dat, bool & woken = unused.b) { return Queue::expressFromISR(dat, woken); }

};

}
}
}

#endif /* _COM_DIAG_AMIGO_TYPEDQUEUE_H_ */
