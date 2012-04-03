/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Sink.h"

namespace com {
namespace diag {
namespace amigo {

size_t Sink::write(const void * datum, size_t size) {
	const uint8_t * here = static_cast<const uint8_t *>(datum);
	while ((size--) > 0) {
		if (write(*here) != 1) { break; }
		++here;
	}
	return (here - static_cast<const uint8_t *>(datum));
}

size_t Sink::write_P(PGM_VOID_P datum, size_t size) {
	PGM_P here = static_cast<PGM_P>(datum);
	while ((size--) > 0) {
		if (write(pgm_read_byte(here)) != 1) { break; }
		++here;
	}
	return (here - static_cast<PGM_P>(datum));
}

}
}
}
