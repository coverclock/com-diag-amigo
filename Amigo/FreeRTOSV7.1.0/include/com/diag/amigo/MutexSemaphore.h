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
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/Queue.h"

namespace com {
namespace diag {
namespace amigo {

class MutexSemaphore
{

public:

	static const Ticks IMMEDIATELY = Queue::IMMEDIATELY;

	static const Ticks NEVER = Queue::NEVER;

	explicit MutexSemaphore();

	virtual ~MutexSemaphore();

	operator bool() const { return (handle != 0); }

	bool take(Ticks timeout = NEVER);

	bool give();

protected:

	xSemaphoreHandle handle;

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	MutexSemaphore(const MutexSemaphore& that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	MutexSemaphore& operator=(const MutexSemaphore& that);

};

inline bool MutexSemaphore::take(Ticks timeout) {
	return (xSemaphoreTakeRecursive(handle, timeout) == pdPASS);
}

inline bool MutexSemaphore::give() {
	return (xSemaphoreGiveRecursive(handle) == pdPASS);
}

}
}
}

#endif /* _COM_DIAG_AMIGO_MUTEXSEMAPHORE_H_ */
