/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "com/diag/amigo/Print.h"
#include "com/diag/amigo/arch/Serial.h"

namespace com {
namespace diag {
namespace amigo {

size_t Print::operator() (const char * format, ...) {
	char buffer[Serial::TRANSMITS + 1]; // + 1 to accommodate trailing NUL.
	va_list ap;

	va_start(ap, format);
	progmem ? vsnprintf_P(buffer, sizeof(buffer), format, ap) : vsnprintf(buffer, sizeof(buffer), format, ap);
	va_end(ap);
	buffer[sizeof(buffer) - 1] = '\0';

	size_t length = strlen(buffer);
	if ((buffer[length - 1] == '\n') && (length < (sizeof(buffer) - 1))) {
		buffer[length++] = '\r';
		buffer[length++] = '\0';
	}

	return (*sink)(buffer, length);
}

}
}
}
