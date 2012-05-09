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
		bytes[0] = a;
		bytes[1] = b;
		bytes[2] = c;
		bytes[3] = d;
		return true;
	} while (false);
	return false;
}

const char * IPV4Address::ntoa(char * buffer, size_t length) {
	snprintf(buffer, length, "%u.%u.%u.%u", bytes[0], bytes[1], bytes[2], bytes[3]);
	buffer[length - 1] = '\0';
	return buffer;
}

}
}
}
