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

Queue::Queue(Count count, Size size, const signed char * name)
{
	handle = xQueueCreate(count, size);
	if ((handle != 0) && (name != 0)) {
		vQueueAddToRegistry(handle, name);
	}
}

Queue::~Queue() {
	vQueueDelete(handle);
	handle = 0;
}

}
}
}
