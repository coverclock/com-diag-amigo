#ifndef _COM_DIAG_AMIGO_UNUSED_H_
#define _COM_DIAG_AMIGO_UNUSED_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"

namespace com {
namespace diag {
namespace amigo {

union Unused {
	bool b;
	char c;
	double d;
	float f;
	int i;
	long l;
	short s;
	long long x;
	size_t z;
	int8_t s8;
	int16_t s16;
	int32_t s32;
	int64_t s64;
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;
	uint64_t u64;
};

extern Unused unused;

}
}
}

#endif /* _COM_DIAG_AMIGO_UNUSED_H_ */
