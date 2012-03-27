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
#include "com/diag/amigo/values.h"
#include "com/diag/amigo/unused.h"

namespace com {
namespace diag {
namespace amigo {

class CountingSemaphore {

public:

	explicit CountingSemaphore(Count maximum, Count initial = 0);

	virtual ~CountingSemaphore();

	operator bool() const { return (handle != 0); }

	bool take(Ticks timeout = NEVER);

	bool give();

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

inline bool CountingSemaphore::take(Ticks timeout) {
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
