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

	Queue(Count count, Size size = 1, const signed char * name = NULL);

	~Queue();

	operator bool() const {
		return (handle != 0);
	}

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

private:

	xQueueHandle handle;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_QUEUE_H_ */
