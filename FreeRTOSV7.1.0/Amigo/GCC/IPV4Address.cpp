/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <stdio.h>
#include <stdlib.h>
#include "com/diag/amigo/IPV4Address.h"

namespace com {
namespace diag {
namespace amigo {

bool IPV4Address::aton(const char * string) {
	do {
		char * end;
		long a = strtol(string, &end, 0);
		if ((a > 255) || (*end != '.')) { break; }
		string = end + 1;
		long b = strtol(string, &end, 0);
		if ((b > 255) || (*end != '.')) { break; }
		string = end + 1;
		long c = strtol(string, &end, 0);
		if ((c > 255) || (*end != '.')) { break; }
		string = end + 1;
		long d = strtol(string, &end, 0);
		if ((d > 255) || (*end != '\0')) { break; }
		payload.bytes[0] = a;
		payload.bytes[1] = b;
		payload.bytes[2] = c;
		payload.bytes[3] = d;
		return true;
	} while (false);
	return false;
}

bool IPV4Address::aton_P(PGM_P pstring) {
	char string[sizeof("NNN.NNN.NNN.NNN")];
	strncpy_P(string, pstring, sizeof(string));
	string[sizeof(string) - 1] = '\0';
	return aton(string);
}

const char * IPV4Address::ntoa(char * buffer, size_t length) {
	snprintf(buffer, length, "%u.%u.%u.%u", payload.bytes[0], payload.bytes[1], payload.bytes[2], payload.bytes[3]);
	buffer[length - 1] = '\0';
	return buffer;
}

}
}
}
