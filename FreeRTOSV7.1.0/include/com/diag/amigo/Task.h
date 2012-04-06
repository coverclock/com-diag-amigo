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

namespace com {
namespace diag {
namespace amigo {

/**
 *
 */
class Task
{

public:

	static const unsigned short DEPTH = 512;

	static const unsigned char PRIORITY = 0;

	static const Ticks MILLISECONDS_PER_TICK = portTICK_RATE_MS;

	static void task(Task * that);

	static void begin();

	static void yield();

	static Ticks elapsed();

	static void delay(Ticks absolute);

	static void delay(Ticks & since, Ticks relative);

	static Ticks ticks(Ticks milliseconds) { return (milliseconds / MILLISECONDS_PER_TICK); }

	static Ticks milliseconds(Ticks ticks) { return (ticks * MILLISECONDS_PER_TICK); }

	explicit Task(const char * myname = 0);

	virtual ~Task();

	operator bool() const { return (handle != 0); }

	void start(unsigned short depth = DEPTH, unsigned char priority = PRIORITY);

	void stop() { stopping = true; }

	bool stopped() const { return stopping; }

	const char * getName() const { return name; }

	xTaskHandle getHandle() const { return handle; }

protected:

	const char * name;
	xTaskHandle handle;
	bool stopping;

	virtual void task();

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

inline Ticks Task::elapsed() {
	return xTaskGetTickCount();
}

inline void Task::delay(Ticks absolute) {
	vTaskDelay(absolute);
}

inline void Task::delay(Ticks & since, Ticks relative) {
	vTaskDelayUntil(&since, relative);
}

}
}
}

#endif /* _COM_DIAG_AMIGO_TASK_H_ */
