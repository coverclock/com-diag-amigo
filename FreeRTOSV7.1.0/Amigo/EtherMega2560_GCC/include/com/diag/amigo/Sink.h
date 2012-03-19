#ifndef _COM_DIAG_SINK_H_
#define _COM_DIAG_SINK_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include <avr/pgmspace.h>

namespace com {
namespace diag {
namespace amigo {

class Sink {

	virtual ~Sink();

	virtual ssize_t operator() () = 0; /* flush */

	virtual ssize_t operator() (unsigned char ch) = 0;

	ssize_t operator() (const void * buf, size_t size, bool progmem = false);

	ssize_t operator() (const char * str, bool progmem = false);

};

}
}
}

#endif /* _COM_DIAG_SINK_H_ */
