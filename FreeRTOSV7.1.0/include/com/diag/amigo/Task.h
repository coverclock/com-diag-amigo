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
 * Task encapsulates the FreeRTOS task facility. Unlike other FreeRTOS
 * encapsulations in Amigo, the FreeRTOS task is not allocated and initialized
 * in the constructor, but in the start() method. Not all functions in the
 * FreeRTOS task API are directly supported.
 */
class Task
{

	/***************************************************************************
	 * TASK CREATING AND DELETING
	 **************************************************************************/

public:

	/**
	 * Constructor. The task is not started once construction is complete.
	 * @param myname points to the name of the task. This is passed to
	 * FreeRTOS when the task is started.
	 * @param myname points to a C-string naming this task; this object keeps
	 * a pointer to this C-string.
	 */
	explicit Task(const char * myname = "?");

	/**
	 * Constructor. This form of the constructor can be used to construct a
	 * Task object for an existing FreeRTOS task. This constructs a proxy
	 * Task object which may be deleted while the task is running. Proxy task
	 * objects can be used to manipulate running tasks without having the
	 * pointer to the original Task object (if it exists).
	 * @param myhandle is the task handle for an existing FreeRTOS task.
	 */
	explicit Task(xTaskHandle myhandle);

	/**
	 * Constructor. This form of the constructor can be used to construct a
	 * Task object for the current FreeRTOS task. This constructs a proxy
	 * Task object which may be deleted while the task is running. Proxy task
	 * objects can be used to manipulate running tasks without having the
	 * pointer to the original Task object (if it exists).
	 */
	explicit Task();

	/**
	 * Destructor. Deleting the Task object of a running non-proxy task is a
	 * fatal error.
	 */
	virtual ~Task();

	/***************************************************************************
	 * TASK STARTTING AND STOPPING
	 **************************************************************************/

public:

	/**
	 * This is the default stack depth for a Task. As stated in the FreeRTOS
	 * documentation, this is in units of the fundamental stack cell type,
	 * which can differ from architecture to architecture. On the megaAVR a
	 * stack cell is one byte.
	 */
	static const unsigned int DEPTH = 512;

	/**
	 * This is the default priority for a Task. Tasks can have priorities from
	 * 0 (lowest) to configMAX_PRIORITIES-1 (highest).
	 */
	static const unsigned int PRIORITY = 0;

	/**
	 * Return true if the task is running, false otherwise. When the task
	 * instance method returns, the FreeRTOS task automatically deleted and
	 * its task handle in this object is zeroed out.
	 * Return true if the task is running, false otherwise.
	 */
	operator bool() const { return (handle != 0); }

	/**
	 * Start the task whose body is the task instance method.
	 * @param mydepth is the stack depth for this task.
	 * @param mypriority is the priority for this task.
	 */
	void start(Count mydepth = DEPTH, Count mypriority = PRIORITY);

	/**
	 * Ask the task to stop. This is purely advisory and stopping is strictly
	 * voluntary on the part of the task. It is under no obligation to do so.
	 * Tasks should probably not stop when they hold resources like semaphores.
	 */
	void stop() { stopping = true; }

	/**
	 * Return true if this task has been asked to stop, false otherwise. Use the
	 * boolean cast operator to determine if the task has actually stopped.
	 * Return true if this task has been asked to stop, false otherwise.
	 */
	bool stopped() const { return stopping; }

	/***************************************************************************
	 * TASK IMPLEMENTATION
	 **************************************************************************/

protected:

	/**
	 * This is the instance method you override in your derived class to
	 * implement your task. This method is allowed to return. If it does so, the
	 * FreeRTOS task that represents it is automatically deleted and the Task
	 * is marked as stopped by zeroing its handle. Your derived class can have
	 * its own class and instance variables and methods in order to implement
	 * whatever it needs to do. Since the Task object is created at the time
	 * of construction but doesn't run until start() and begin(), you can
	 * manipulate the Task object as you see fit with your task implementation
	 * being none the wiser.
	 */
	virtual void task();

	/***************************************************************************
	 * SCHEDULER MANAGEMENT
	 **************************************************************************/

public:

	/**
	 * Start the FreeRTOS scheduler. All FreeRTOS tasks that have been started
	 * and are not suspended will begin to run. There is always at least one
	 * FreeRTOS task, the idle task. You must call this once and only once from
	 * main. Unless the FreeRTOS scheduler fails to start, this class method
	 * never returns.
	 */
	static void begin();

	/***************************************************************************
	 * TASK TIMING
	 **************************************************************************/

public:

	/**
	 * This is the number of milliseconds per system tick. A system tick is
	 * the basic timing and scheduling interval in FreeRTOS.
	 */
	static const Ticks PERIOD = portTICK_RATE_MS;

	/**
	 * Defines the maximum delay value in Ticks. The actual delay depends on
	 * the duration of a system tick, the width of the Ticks data type, and
	 * any underlying scheduling latency. Forever may be as short as about 131
	 * seconds given a two millisecond tick duration and a sixteen-bit Ticks
	 * data type. If you really want to delay forever, you must put your
	 * delay() in a loop.
	 */
	static const Ticks FOREVER = portMAX_DELAY; // Nominally ~0.

	/**
	 * Convert milliseconds to ticks. Fractional results are truncated. The
	 * application can control rounding by adding some portion (like half) or
	 * all of the value MILLISECONDS_PER_TICK to the argument value.
	 * @param milliseconds is a value in milliseconds.
	 * @return a value in ticks.
	 */
	static Ticks milliseconds2ticks(Ticks milliseconds) { return (milliseconds / PERIOD); }

	/**
	 * Convert ticks to milliseconds.
	 * @param ticks is a value in ticks.
	 * @return a value in milliseconds.
	 */
	static Ticks ticks2milliseconds(Ticks ticks) { return (ticks * PERIOD); }

	/**
	 * Return the number of FreeRTOS ticks that have elapsed since the FreeRTOS
	 * scheduler was started. The value returned by this class method will wrap
	 * around, perhaps quite quickly, depending on the frequency of the system
	 * clock and the width of the data type used by FreeRTOS for ticks. Quite
	 * quickly may be as little as about 131 seconds given a two millisecond
	 * tick duration and a sixteen-bit Ticks data type.
	 * @return the number of elapsed FreeRTOS ticks.
	 */
	static Ticks elapsed();

	/**
	 * Return the number of FreeRTOS ticks that have elapsed since the FreeRTOS
	 * scheduler was started. This method can be called from a interrupt
	 * service routine.
	 * @return the number of elapsed FreeRTOS ticks.
	 */
	static Ticks elapsedFromISR();

	/**
	 * Block the calling task for the specified absolute number of ticks. Note
	 * that this delay is approximate depending on scheduling latency.
	 * Specifying zero is equivalent to yield().
	 * @param absolute is the absolute delay value in ticks.
	 */
	static void delay(Ticks absolute);

	/**
	 * Block the calling task for the specified number of ticks relative to
	 * a prior time in ticks. The prior time variable is updated by this action
	 * so that it can be subsequently reused without modification to implement
	 * a periodic action. Note that this delay is approximate depending on
	 * scheduling latency.
	 * @param since is the prior time value-result variable that is modified.
	 * @param relative is the relative delay value in ticks.
	 */
	static void delay(Ticks & since, Ticks relative);

	/***************************************************************************
	 * TASK SCHEDULING
	 **************************************************************************/

public:

	/**
	 * Yield the processor, placing the calling FreeRTOS task at the end of
	 * the list of ready tasks and resuming the highest priority ready task.
	 * This class method will return to the caller when that task once again
	 * becomes the current running task.
	 */
	static void yield();

	/**
	 * Return the priority of this task.
	 * @return the priorty of this task.
	 */
	unsigned int priority() const;

	/**
	 * Modify the priority of this task.
	 * @param mypriority is the new priority for this task.
	 */
	void priority(unsigned int mypriority);

	/**
	 * Suspend the calling task. This is the same as calling suspend() on the
	 * Task object of the calling task, but relieves the calling task from
	 * having to have a pointer to its own Task object.
	 */
	static void suspendSelf();

	/**
	 * Suspend this task. It is okay for the current task to suspend itself and
	 * if it does so it is exactly the same as if it called suspendSelf(). Tasks
	 * can be suspended only after start(). If they are suspended before
	 * begin() they are initially suspended when task scheduling begins. Even
	 * blocked tasks can be suspended.
	 */
	void suspend();

	/**
	 * Return true if this task is suspended, false otherwise.
	 * @return true if this task is suspended, false otherwise.
	 */
	bool suspended() const;

	/**
	 * Resume this task. Although the FreeRTOS documentation doesn't seem to
	 * say this, the FreeRTOS code will context switch into the resumed task
	 * if it has a higher priority than that of the calling task. Tasks can be
	 * resumed only after begin().
	 */
	void resume();

	/**
	 * Resume this task. This method can be called from an interrupt service
	 * routine.
	 * @return true if the resumed task has a higher priority than that of the
	 * interrupted task.
	 */
	bool resumeFromISR();

	/***************************************************************************
	 * ANCILLARY STUFF
	 **************************************************************************/

public:

	/**
	 * Return the task handle for the current task.
	 * @return the task handle for the current task.
	 */
	static xTaskHandle current();

	/**
	 * Return the task handle for the idle task.
	 * @return the task handle for the idle task.
	 */
	static xTaskHandle idle();

	/**
	 * Return the number of tasks.
	 * @return the number of tasks.
	 */
	static Count tasks();

	/**
	 * Call the instance task method when invoked by the Amigo trampoline
	 * function. It must be public so that it can be called by the trampoline
	 * function which has C-linkage. It is not part of the public API and you
	 * should never call it.
	 * @param that points to the Task object for the starting task.
	 */
	static void task(Task * that);

	/**
	 * Get the name of this task. This can be done even if the task is not
	 * running or has never run.
	 * @return a pointer to a C-string naming this task.
	 */
	const char * getName() const { return name; }

	/**
	 * Get the FreeRTOS handle for this task. This value will be zero (null)
	 * if the task has never been started or has returned and been deleted.
	 * @return the FreeRTOS handle for this task.
	 */
	xTaskHandle getHandle() const { return handle; }

protected:

	xTaskHandle handle;
	const char * name;
	bool stopping;
	bool proxy;

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

inline Ticks Task::elapsedFromISR() {
	return xTaskGetTickCountFromISR();
}

inline void Task::delay(Ticks absolute) {
	vTaskDelay(absolute);
}

inline void Task::delay(Ticks & since, Ticks relative) {
	vTaskDelayUntil(&since, relative);
}

inline xTaskHandle Task::current() {
	return xTaskGetCurrentTaskHandle();
}

inline xTaskHandle Task::idle() {
	return xTaskGetIdleTaskHandle();
}

inline Count Task::tasks() {
	return uxTaskGetNumberOfTasks();
}

inline bool Task::suspended() const {
	return (xTaskIsTaskSuspended(handle) == pdTRUE);
}

inline unsigned int Task::priority() const {
	return uxTaskPriorityGet(handle);
}

inline void Task::priority(unsigned int mypriority) {
	vTaskPrioritySet(handle, mypriority);
}

inline void Task::suspend() {
	vTaskSuspend(handle);
}

inline void Task::suspendSelf() {
	vTaskSuspend(0);
}

inline void Task::resume() {
	vTaskResume(handle);
}

inline bool Task::resumeFromISR() {
	return (xTaskResumeFromISR(handle) == pdTRUE);
}

}
}
}

#endif /* _COM_DIAG_AMIGO_TASK_H_ */
