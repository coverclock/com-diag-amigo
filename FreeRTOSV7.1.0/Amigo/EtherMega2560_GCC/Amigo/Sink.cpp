/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Sink.h"
#include <string.h>

namespace com {
namespace diag {
namespace amigo {

Sink::~Sink() {
}

ssize_t Sink::operator() (const void * buf, size_t size, bool progmem) {
	const unsigned char * here = static_cast<const unsigned char *>(buf);
	while ((size--) > 0) {
		if ((*this)(progmem ? pgm_read_byte(*here) : *here) != 1) { break; }
		++here;
	}
	return (here - static_cast<const unsigned char *>(buf));
}

ssize_t Sink::operator() (const char * str, bool progmem) {
	return (*this)(str, progmem ? strlen_P(str) : strlen(str), progmem);
}

}
}
}
