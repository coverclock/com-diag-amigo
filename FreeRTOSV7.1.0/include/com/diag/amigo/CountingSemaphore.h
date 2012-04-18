#ifndef _COM_DIAG_AMIGO_COUNTINGSEMAPHORE_H_
#define _COM_DIAG_AMIGO_COUNTINGSEMAPHORE_H_

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
#include "com/diag/amigo/unused.h"
#include "com/diag/amigo/Queue.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * CountingSemaphore encapsulates a FreeRTOS counting semaphore.
 * CountingSemaphores are typically used to synchronize tasks sharing a pool of
 * resources.
 */
class CountingSemaphore
{

public:

	/**
	 * Defines the timeout value in ticks that causes the application to never
	 * block waiting for the semaphore to become available, but instead be
	 * returned an error.
	 */
	static const ticks_t IMMEDIATELY = Queue::IMMEDIATELY;

	/**
	 * Defines the timeout value in ticks that causes the application to block
	 * indefinitely waiting for the semaphore to become available.
	 */
	static const ticks_t NEVER = Queue::NEVER;

	/**
	 * Constructor.
	 */
	explicit CountingSemaphore(size_t maximum, size_t initial = 0);

	/**
	 * Destructor.
	 */
	virtual ~CountingSemaphore();

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
	bool take(ticks_t timeout = NEVER);

	/**
	 * Relinquish the semaphore. On other systems this would be equivalent to
	 * an operation that might be called unlock, dequeue, or V.
	 * @return true if the semaphore was relinquished successfully, false
	 * otherwise.
	 */
	bool give();

	/**
	 * Relinquish the semaphore. On other systems this would be equivalent to
	 * an operation that might be called unlock, dequeue, or V. This is
	 * guaranteed to be non-blocking and should only be called from an interrupt
	 * service routine.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if the semaphore was relinquished successfully, false
	 * otherwise.
	 */
	bool giveFromISR(bool & woken = unused.b);

protected:

	xSemaphoreHandle handle;

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	CountingSemaphore(const CountingSemaphore& that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	CountingSemaphore& operator=(const CountingSemaphore& that);

};

inline bool CountingSemaphore::take(ticks_t timeout) {
	return (xSemaphoreTake(handle, timeout) == pdPASS);
}

inline bool CountingSemaphore::give() {
	return (xSemaphoreGive(handle) == pdPASS);
}

inline bool CountingSemaphore::giveFromISR(bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xSemaphoreGiveFromISR(handle, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

}
}
}

#endif /* _COM_DIAG_AMIGO_COUNTINGSEMAPHORE_H_ */
