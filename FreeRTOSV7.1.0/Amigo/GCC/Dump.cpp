/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Dump.h"
#include "com/diag/amigo/target/harvard.h"

namespace com {
namespace diag {
namespace amigo {

static const char HEX[] PROGMEM = "0123456789ABCDEF";

size_t Dump::operator() (const void * data, size_t size) {
	size_t result = 0;
	uint8_t datum;
	if (progmem) {
		PGM_P here = static_cast<PGM_P>(data);
		while ((size--) > 0) {
			datum = pgm_read_byte(here++);
			result += sink->write(pgm_read_byte(&HEX[datum >> 4]));
			result += sink->write(pgm_read_byte(&HEX[datum & 0xf]));
		}
	} else {
		const uint8_t * here = static_cast<const uint8_t *>(data);
		while ((size--) > 0) {
			datum = *(here++);
			result += sink->write(pgm_read_byte(&HEX[datum >> 4]));
			result += sink->write(pgm_read_byte(&HEX[datum & 0xf]));
		}
	}
	return result;
}

}
}
}
