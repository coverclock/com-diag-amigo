/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Source.h"

namespace com {
namespace diag {
namespace amigo {

Source::~Source() {}

size_t Source::read(void * buffer, size_t size) {
	uint8_t * here = static_cast<uint8_t *>(buffer);
	int ch;
	while ((size--) > 0) {
		if ((ch = read()) < 0) { break; }
		*(here++) = static_cast<uint8_t>(ch);
	}
	return (here - static_cast<uint8_t *>(buffer));
}

}
}
}
