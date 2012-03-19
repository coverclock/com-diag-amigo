#ifndef _COM_DIAG_SOURCE_H_
#define _COM_DIAG_SOURCE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include <string.h>
#include <avr/pgmspace.h>

namespace com {
namespace diag {
namespace amigo {

class Source {

	virtual ~Source();

	virtual int operator() (bool peek = false) = 0;

	ssize_t operator() (void * buf, size_t size);

	ssize_t operator() (char * str, size_t size, const char * end, bool progmem = false);

};

}
}
}

#endif /* _COM_DIAG_SOURCE_H_ */
