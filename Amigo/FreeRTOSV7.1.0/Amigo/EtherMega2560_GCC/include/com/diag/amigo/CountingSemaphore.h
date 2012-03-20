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

	CountingSemaphore(Count maximum, Count initial);

	~CountingSemaphore();

	operator bool() const {
		return (handle != 0);
	}

	bool take(Ticks timeout = NEVER);

	bool give();

	bool giveFromISR(bool & woken = unused.b);

private:

	xSemaphoreHandle handle;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_COUNTINGSEMAPHORE_H_ */
