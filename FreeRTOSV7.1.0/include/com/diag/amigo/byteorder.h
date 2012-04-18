#ifndef _COM_DIAG_AMIGO_BYTEORDER_H_
#define _COM_DIAG_AMIGO_BYTEORDER_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/littleendian.h"

namespace com {
namespace diag {
namespace amigo {

inline uint16_t byteswap16(uint16_t datum) {
	return (((datum << 8) & 0xff00) | ((datum >> 8) & 0x00ff));
}

inline uint32_t byteswap32(uint32_t datum) {
	return ((datum << 24) & 0xff000000UL) | ((datum << 8) & 0x00ff0000UL) | ((datum >> 8 & 0x0000ff00UL) | ((datum >> 24) & 0x000000ffUL));
}

inline uint16_t htons(uint16_t datum) {
	return (littleendian() ? byteswap16(datum) : datum);
}

inline uint16_t ntohs(uint16_t datum) {
	return (littleendian() ? byteswap16(datum) : datum);
}

inline uint32_t htonl(uint32_t datum) {
	return (littleendian() ? byteswap32(datum) : datum);
}
inline uint32_t ntohl(uint32_t datum) {
	return (littleendian() ? byteswap32(datum) : datum);
}

}
}
}

#endif /* _COM_DIAG_AMIGO_BYTEORDER_H_ */
