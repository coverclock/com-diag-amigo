/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Socket.h"
#include "com/diag/amigo/CriticalSection.h"

namespace com {
namespace diag {
namespace amigo {

MutexSemaphore Socket::mutex;

Socket::port_t Socket::localport = LOCAL;

Socket::port_t Socket::allocate() {
	CriticalSection cs(mutex);
	port_t port = localport++;
	if (localport == 0) {
		localport = LOCAL;
	}
	return port;
}

Socket::~Socket() {}

}
}
}
