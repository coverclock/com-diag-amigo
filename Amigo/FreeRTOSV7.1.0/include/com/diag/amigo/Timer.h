#ifndef _COM_DIAG_AMIGO_TIMER_H_
#define _COM_DIAG_AMIGO_TIMER_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "FreeRTOS.h"
#include "timers.h"
#include "com/diag/amigo/Queue.h"
#include "com/diag/amigo/unused.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * Timer encapsulates the FreeRTOS timers facility. All FreeRTOS timer
 * management is performed asynchronously by a background FreeRTOS timer task,
 * not by the FreeRTOS kernel itself. So using a Timer is the same as two
 * tasks using a Queue for intertask communication, with all of the same issues
 * of timeouts and failures. This also means that there is scheduling latency
 * involved in managing timers.
 */
class Timer
{

	/***************************************************************************
	 * TIMER CREATING AND DELETING
	 **************************************************************************/

public:

	/**
	 * Constructor.
	 * @param duration is the length of the timer in system ticks.
	 * @param periodic if true causes the timer to automatically reschedule
	 * itself for the same duration after it expires.
	 * @param myname is a C-string that names the timer.
	 */
	explicit Timer(ticks_t duration, bool periodic = false, const char * myname = "?");

	/**
	 * Destructor. It is a fatal error to delete a Timer object that has an
	 * active FreeRTOS timer.
	 */
	virtual ~Timer();

	/**
	 * Returns true if the construction of the Timer was successful.
	 * @return true if successful, false otherwise.
	 */
	operator bool() const { return (handle != 0); }

	/***************************************************************************
	 * TIMER STARTING AND STOPPING
	 **************************************************************************/

public:

	/**
	 * Defines the timeout value in ticks that causes the application to never
	 * block waiting for the Timer task to become available, but instead be
	 * returned an error.
	 */
	static const ticks_t IMMEDIATELY = Queue::IMMEDIATELY;

	/**
	 * Defines the timeout value in ticks that causes the application to block
	 * indefinitely waiting for the Timer task to become available.
	 */
	static const ticks_t NEVER = Queue::NEVER;

	/**
	 * Start the timer.
	 * @param timeout is the duration in ticks to wait for the timer task.
	 * @return true if the start message was sent to the timer task.
	 */
	bool start(ticks_t timeout = NEVER);

	/**
	 * Start the timer. This method can be called from an interrupt service
	 * routine.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if the start message was sent to the timer task.
	 */
	bool startFromISR(bool & woken = unused.b);

	/**
	 * Stop the timer.
	 * @param timeout is the duration in ticks to wait for the timer task.
	 * @return true if the stop message was sent to the timer task.
	 */
	bool stop(ticks_t timeout = NEVER);

	/**
	 * Stop the timer. This method can be called from an interrupt service
	 * routine.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if the stop message was sent to the timer task.
	 */
	bool stopFromISR(bool & woken = unused.b);

	/***************************************************************************
	 * TIMER IMPLEMENTATION
	 **************************************************************************/

protected:

	/**
	 * This is the instance method you override in your derived class to
	 * implement your timer.
	 */
	virtual void timer();

	/***************************************************************************
	 * TIMER MANAGEMENT
	 **************************************************************************/

public:

	/**
	 * Restart the timer. If the timer is not running, it is started with its
	 * existing duration. If it is running, it is rescheduled with its existing
	 * duration relative to now.
	 * @param timeout is the duration in ticks to wait for the timer task.
	 * @return true if the start message was sent to the timer task.
	 */
	bool reset(ticks_t timeout = NEVER);

	/**
	 * Restart the timer. If the timer is not running, it is started with its
	 * existing duration. If it is running, it is rescheduled with its existing
	 * duration relative to now. This method can be called from an interrupt
	 * service routine.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if the start message was sent to the timer task.
	 */
	bool resetFromISR(bool & woken = unused.b);

	/**
	 * Restart the timer. If the timer is not running, it is started with the
	 * new duration. If it is running, it is rescheduled with the new duration
	 * relative to now.
	 * @param duration is the length of the timer in system ticks.
	 * @param timeout is the duration in ticks to wait for the timer task.
	 * @return true if the restart message was sent to the timer task.
	 */
	bool reschedule(ticks_t duration, ticks_t timeout = NEVER);

	/**
	 * Restart the timer. If the timer is not running, it is started with the
	 * new duration. If it is running, it is rescheduled with the new duration
	 * relative to now. This method can be called from an interrupt service
	 * routine.
	 * @param duration is the length of the timer in system ticks.
	 * @param woken is returned true if this woke a higher priority task.
	 * @return true if the restart message was sent to the timer task.
	 */
	bool rescheduleFromISR(ticks_t duration, bool & woken = unused.b);

	/***************************************************************************
	 * ANCILLARY STUFF
	 **************************************************************************/

public:

	/**
	 * This counter is incremented when a non-fatal but perhaps serious error
	 * has occurred in a place where it cannot be communicated to the
	 * application. Typically this means a resource leak may have occurred
	 * inside the destructor due to a failure reported by FreeRTOS.
	 */
	static int errors;

	/**
	 * This is how long this objects' destructor will wait to communicate with
	 * the FreeRTOS timer task to delete a FreeRTOS timer.
	 */
	static const ticks_t DESTRUCTION = portMAX_DELAY;

	/**
	 * Call the instance task method when invoked by the Amigo trampoline
	 * function. It must be public so that it can be called by the trampoline
	 * function which has C-linkage. It is not part of the public API and you
	 * should never call it.
	 * @param that points to the Timer object for the expiring timer.
	 */
	static void timer(Timer * that);

	/**
	 * Return the task handle for the FreeRTOS timer task.
	 * @return the task handle for the FreeRTOS timer task.
	 */
	static xTaskHandle daemon();

	/**
	 * Returns true if the timer is active, false otherwise.
	 * @return true if the timer is active, false otherwise.
	 */
	bool active() const;

	/**
	 * Get the name of this timer.
	 * @return a pointer to a C-string naming this task.
	 */
	const char * getName() const { return name; }

	/**
	 * Get the FreeRTOS handle for this timer.
	 * @return the FreeRTOS handle for this timer.
	 */
	xTimerHandle getHandle() const { return handle; }

protected:

	xTimerHandle handle;
	const char * name;

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Timer(const Timer& that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Timer& operator=(const Timer& that);

};

inline bool Timer::start(ticks_t timeout) {
	return (xTimerStart(handle, timeout) == pdPASS);
}

inline bool Timer::startFromISR(bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xTimerStartFromISR(handle, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

inline bool Timer::stop(ticks_t timeout) {
	return (xTimerStop(handle, timeout) == pdPASS);
}

inline bool Timer::stopFromISR(bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xTimerStopFromISR(handle, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;

}

inline bool Timer::reset(ticks_t timeout) {
	return (xTimerReset(handle, timeout) == pdPASS);
}

inline bool Timer::resetFromISR(bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xTimerResetFromISR(handle, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

inline bool Timer::reschedule(ticks_t duration, ticks_t timeout) {
	return (xTimerChangePeriod(handle, duration, timeout) == pdPASS);
}

inline bool Timer::rescheduleFromISR(ticks_t duration, bool & woken) {
	portBASE_TYPE temporary = pdFALSE;
	bool result = (xTimerChangePeriodFromISR(handle, duration, &temporary) == pdPASS);
	woken = (temporary == pdTRUE);
	return result;
}

inline xTaskHandle Timer::daemon() {
	return xTimerGetTimerDaemonTaskHandle();
}

inline bool Timer::active() const {
	return (xTimerIsTimerActive(handle) != pdFALSE);
}

/**
 * PeriodicTimer extends Timer for just supporting periodic timers.
 */
class PeriodicTimer
: public Timer
{

public:

	/**
	 * Constructor.
	 * @param duration is the period of the timer in system ticks.
	 * @param myname is a C-string that names the timer.
	 */
	explicit PeriodicTimer(ticks_t duration, const char * myname = "?")
	: Timer(duration, true, myname)
	{}

	/**
	 * Destructor.
	 */
	virtual ~PeriodicTimer();

};

/**
 * OneShotTimer extends Timer for just supporting one-shot timers.
 */
class OneShotTimer
: public Timer
{

public:

	/**
	 * Constructor.
	 * @param duration is the period of the timer in system ticks.
	 * @param myname is a C-string that names the timer.
	 */
	explicit OneShotTimer(ticks_t duration, const char * myname = "?")
	: Timer(duration, false, myname)
	{}

	/**
	 * Destructor.
	 */
	virtual ~OneShotTimer();

};


}
}
}

#endif /* _COM_DIAG_AMIGO_TIMER_H_ */
