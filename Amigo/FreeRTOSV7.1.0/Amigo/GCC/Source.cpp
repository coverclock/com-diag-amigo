/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Source.h"
#include <avr/pgmspace.h>
#include <string.h>

namespace com {
namespace diag {
namespace amigo {

size_t Source::operator() (void * buf, size_t size) {
	unsigned char * here = static_cast<unsigned char *>(buf);
	int datum;
	while ((size--) > 0) {
		if ((datum = (*this)()) < 0) { break; }
		*(here++) = datum;
	}
	return (here - static_cast<unsigned char *>(buf));
}

}
}
}
