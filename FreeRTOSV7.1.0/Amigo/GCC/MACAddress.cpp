/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <stdio.h>
#include <stdlib.h>
#include "com/diag/amigo/MACAddress.h"

namespace com {
namespace diag {
namespace amigo {

bool MACAddress::aton(const char * string) {
	do {
		char * end;
		long a = strtol(string, &end, 16);
		if ((a > 255) || (*end != ':')) { break; }
		string = end + 1;
		long b = strtol(string, &end, 16);
		if ((b > 255) || (*end != ':')) { break; }
		string = end + 1;
		long c = strtol(string, &end, 16);
		if ((c > 255) || (*end != ':')) { break; }
		string = end + 1;
		long d = strtol(string, &end, 16);
		if ((d > 255) || (*end != ':')) { break; }
		string = end + 1;
		long e = strtol(string, &end, 16);
		if ((e > 255) || (*end != ':')) { break; }
		string = end + 1;
		long f = strtol(string, &end, 16);
		if ((f > 255) || (*end != '\0')) { break; }
		payload.bytes[0] = 0;
		payload.bytes[1] = 0;
		payload.bytes[2] = a;
		payload.bytes[3] = b;
		payload.bytes[4] = c;
		payload.bytes[5] = d;
		payload.bytes[6] = e;
		payload.bytes[7] = f;
		return true;
	} while (false);
	return false;
}

bool MACAddress::aton_P(PGM_P pstring) {
	char string[sizeof("XX:XX:XX:XX:XX:XX")];
	strncpy_P(string, pstring, sizeof(string));
	string[sizeof(string) - 1] = '\0';
	return aton(string);
}

const char * MACAddress::ntoa(char * buffer, size_t length) {
	snprintf(buffer, length, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", payload.bytes[2], payload.bytes[3], payload.bytes[4], payload.bytes[5], payload.bytes[6], payload.bytes[7]);
	buffer[length - 1] = '\0';
	return buffer;
}

}
}
}
