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

public:

	typedef void * (Function)(void *);

	static const unsigned short DEPTH = 512;

	static const unsigned char PRIORITY = 0;

	static void begin();

	static void yield();

protected:

	static COM_DIAG_AMIGO_TASK(trampoline);

public:

	explicit Task(const char * myname = 0);

	virtual ~Task();

	operator bool() const { return (handle != 0); }

	void start(void * (*myfunction)(void *) = 0, void * myparameter = 0, unsigned short depth = DEPTH, unsigned char priority = PRIORITY);

	void stop() { stopping = true; }

	bool stopped() const { return stopping; }

	const char * getName() const { return name; }

	xTaskHandle getHandle() const { return handle; }

	Function * getFunction() const { return function; }

	void * getParameter() const { return parameter; }

	void * getResult() const { return result; }

protected:

	const char * name;
	xTaskHandle handle;
	void * (*function)(void *);
	void * parameter;
	void * result;
	bool stopping;

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

inline void Task::begin() {
	vTaskStartScheduler();
}

inline void Task::yield() {
	taskYIELD();
}

}
}
}

#endif /* _COM_DIAG_AMIGO_TASK_H_ */
