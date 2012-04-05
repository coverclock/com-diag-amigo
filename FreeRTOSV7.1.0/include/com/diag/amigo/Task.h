#ifndef _COM_DIAG_AMIGO_TASK_H_
#define _COM_DIAG_AMIGO_TASK_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "FreeRTOS.h"
#include "task.h"
#include "com/diag/amigo/types.h"

#if defined(__AVR_3_BYTE_PC__)
#	define COM_DIAG_AMIGO_TASK(_NAME_) void _NAME_(void *) __attribute__((section(".lowtext")))
#else
#	define COM_DIAG_AMIGO_TASK(_NAME_) void _NAME_(void *)
#endif

namespace com {
namespace diag {
namespace amigo {

/**
 *
 */
class Task
{

protected:

	static COM_DIAG_AMIGO_TASK(task);

public:

	explicit Task(const char * name = 0);

	virtual ~Task();

	void start(void * (*myfunction)(void *) = 0, void * myparameter = 0);

protected:

	xTaskHandle handle;
	void * (*function)(void *);
	void * parameter;
	void * result;

	virtual void * task();

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Task(const Task& that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Task& operator=(const Task& that);

};

}
}
}

#endif /* _COM_DIAG_AMIGO_TASK_H_ */
