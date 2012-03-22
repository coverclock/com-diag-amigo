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

size_t Sink::operator() (const void * dat, size_t size, bool progmem) {
	const unsigned char * here = static_cast<const unsigned char *>(dat);
	while ((size--) > 0) {
		if ((*this)(progmem ? pgm_read_byte(*here) : *here) != 1) { break; }
		++here;
	}
	return (here - static_cast<const unsigned char *>(dat));
}

}
}
}
