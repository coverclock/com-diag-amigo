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
		bytes[0] = 0;
		bytes[1] = 0;
		bytes[2] = a;
		bytes[3] = b;
		bytes[4] = c;
		bytes[5] = d;
		bytes[6] = e;
		bytes[7] = f;
		return true;
	} while (false);
	return false;
}

const char * MACAddress::ntoa(char * buffer, size_t length) {
	snprintf(buffer, length, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7]);
	buffer[length - 1] = '\0';
	return buffer;
}

}
}
}
