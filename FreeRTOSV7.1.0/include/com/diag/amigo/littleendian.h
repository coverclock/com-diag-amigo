#ifndef _COM_DIAG_AMIGO_LITTLEENDIAN_H_
#define _COM_DIAG_AMIGO_LITTLEENDIAN_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * Adapted from Desperado\n
 * http://www.diag.com/navigation/downloads/Desperado.html\n
 */

#include "com/diag/amigo/cxxcapi.h"
#include "com/diag/amigo/types.h"

/**
 *  Return non-zero if the target has little-endian byte order, zero if
 *  big-endian (a.k.a. "network byte order"). Very likely to be completely
 *  optimized away by the compiler since it can resolve the result at compile
 *  time. Can be called from either C or C++ translation units.
 *  @return non-zero if little-endian, zero if big-endian.
 */
CXXCINLINE int amigo_littleendian(void) {
    static union { uint16_t w; uint8_t b[sizeof(uint16_t)]; } d = { 1 };
    return d.b[0];
}

#if defined(__cplusplus)

namespace com {
namespace diag {
namespace amigo {

/**
 *  Return true if the target has little-endian byte order, false if
 *  big-endian.
 *  @return true if little-endian, false if big-endian.
 */
inline bool littleendian() {
	return (amigo_littleendian() != 0);
}

}
}
}

#endif

#endif /* _COM_DIAG_AMIGO_LITTLEENDIAN_H_ */
