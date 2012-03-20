#ifndef _COM_DIAG_AMIGO_MUTEXSEMAPHORE_H_
#define _COM_DIAG_AMIGO_MUTEXSEMAPHORE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "FreeRTOS.h"
#include "semphr.h"
#include "com/diag/amigo/values.h"

namespace com {
namespace diag {
namespace amigo {

class MutexSemaphore {

public:

	MutexSemaphore();

	~MutexSemaphore();

	operator bool() const {
		return (handle != 0);
	}

	bool take(Ticks timeout = NEVER);

	bool give();

private:

	xSemaphoreHandle handle;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_MUTEXSEMAPHORE_H_ */
