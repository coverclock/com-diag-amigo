/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Sink.h"
#include <avr/pgmspace.h>
#include <string.h>

namespace com {
namespace diag {
namespace amigo {

size_t Sink::write(const void * datum, size_t size) {
	const uint8_t * here = static_cast<const uint8_t *>(datum);
	while ((size--) > 0) {
		if (write(*here) != 1) { break; }
		++here;
	}
	return (here - static_cast<const unsigned char *>(datum));
}

}
}
}
