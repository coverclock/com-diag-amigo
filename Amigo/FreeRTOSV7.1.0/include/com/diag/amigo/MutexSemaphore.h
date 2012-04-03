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

/**
 * MutexSemaphore encapsulates a FreeRTOS recursive mutex semaphore.
 * MutexSemaphores are used to implement critical sections in which one and
 * only one task may execute at a time.
 */
class MutexSemaphore
{

public:

	/**
	 * Defines the timeout value in Ticks that causes the application to never
	 * block waiting for the semaphore to become available, but instead be
	 * returned an error.
	 */
	static const Ticks IMMEDIATELY = Queue::IMMEDIATELY;

	/**
	 * Defines the timeout value in Ticks that causes the application to block
	 * indefinitely waiting for the semaphore to become available.
	 */
	static const Ticks NEVER = Queue::NEVER;

	/**
	 * Constructor.
	 */
	explicit MutexSemaphore();

	/**
	 * Destructor.
	 */
	virtual ~MutexSemaphore();

	/**
	 * Returns true if the construction of the Semaphore was successful.
	 * @return true if successful, false otherwise.
	 */
	operator bool() const { return (handle != 0); }

	/**
	 * Attempt to acquire the semaphore if it can be done within the timeout
	 * period. On other systems this would be equivalent to an operation that
	 * might be called lock, enqueue, or P.
	 * @param timeout is the duration in ticks to wait for acquisiton.
	 * @return true if the semaphore was acquired successfully, false otherwise.
	 */
	bool take(Ticks timeout = NEVER);

	/**
	 * Relinquish the semaphore. On other systems this would be equivalent to
	 * an operation that might be called unlock, dequeue, or V.
	 * @return true if the semaphore was relinquished successfully, false
	 * otherwise.
	 */
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
