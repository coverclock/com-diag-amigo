#ifndef _COM_DIAG_AMIGO_CRITICALSECTION_H_
#define _COM_DIAG_AMIGO_CRITICALSECTION_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * These symbols are in the global name space!\n
 */

#include "com/diag/amigo/MutexSemaphore.h"

namespace com {
namespace diag {
namespace amigo {

class CriticalSection {

public:

	CriticalSection(MutexSemaphore & semaphore)
	: mutex(semaphore)
	{
		success = mutex.take();
	}

	~CriticalSection() {
		if (success) {
			mutex.give();
		}
	}

	bool operator bool() {
		return success;
	}

private:

	MutexSemaphore & mutex;
	bool success;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_CRITICALSECTION_H_ */
