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

/**
 * CriticalSection implements a scoped critical section protected by the
 * specified MutexSemaphore which provides mutual exclusion among concurrent
 * tasks within the scope. CriticalSection takes the MutexSemaphore in its
 * constructor, and gives the MutexSemaphore in its destructor.This allows
 * scoped critical sections of code to be written, exploiting the
 * "Resource Acquisition is Initialization" idiom.
 */
class CriticalSection
{

public:

	/**
	 * Constructor. This object saves a pointer to the MutexSemaphore in an
	 * instance variable. The MutexSemaphore is taken, blocking the allocating
	 * task.
	 * @param mymutex refers to the MutexSemaphore to give and take.
	 */
	CriticalSection(MutexSemaphore & mymutex)
	: mutex(&mymutex)
	{
		if (!mutex->take()) {
			mutex = 0;
		}
	}

	/**
	 * Constructor. This object saves a pointer to the MutexSemaphore in an
	 * instance variable. If the pointer is not NULL, MutexSemaphore is taken,
	 * blocking the allocating task. This constructor is useful when the use
	 * of the semaphore is optional in an application; passing a NULL pointer
	 * causes this constructor, and the common destructor, to do nothing.
	 * @param mymutexp points to the MutexSemaphore or NULL if none.
	 */
	CriticalSection(MutexSemaphore * mymutexp)
	: mutex(mymutexp)
	{
		if (mutex == 0) {
			// Do nothing: optional semaphore not provided.
		} else if (!mutex->take()) {
			mutex = 0;
		} else {
			// Do nothing: nominal case.
		}
	}

	/**
	 * Destructor. The MutexSemaphore is given, unblocking any waiting tasks.
	 */
	~CriticalSection() {
		if (mutex != 0) {
			mutex->give();
		}
	}

	/**
	 * Return true if construction was successful.
	 * @return true if construction was successful, false otherwise.
	 */
	operator bool() { return (mutex != 0); }

protected:

	MutexSemaphore * mutex;

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	CriticalSection(const CriticalSection& that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	CriticalSection& operator=(const CriticalSection& that);

};

}
}
}

#endif /* _COM_DIAG_AMIGO_CRITICALSECTION_H_ */
