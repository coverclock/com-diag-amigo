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
	 * @param semaphore refers to the MutexSemaphore to give and take.
	 */
	CriticalSection(MutexSemaphore & mutex)
	: mutexp(&mutex)
	{
		success = mutexp->take();
	}

	/**
	 * Destructor. The MutexSemaphore is given, unblocking any waiting tasks.
	 */
	~CriticalSection() {
		if (success) {
			mutexp->give();
		}
	}

	/**
	 * Return true if construction was successful and the MutexSemaphore was
	 * taken.
	 * @return true if construction was successful, false otherwise.
	 */
	operator bool() { return success; }

protected:

	MutexSemaphore * mutexp;
	bool success;

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
