#ifndef _COM_DIAG_AMIGO_BYTEORDER_H_
#define _COM_DIAG_AMIGO_BYTEORDER_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This header file places C++ symbols in the global namespace!
 */

#include "com/diag/amigo/littleendian.h"
#include "com/diag/amigo/cxxcapi.h"

/**
 * Swap the bytes in a short word.
 * @param datum is the input word.
 * @return a byte swapped output word.
 */
CXXCINLINE uint16_t amigo_byteswap16(uint16_t datum) {
	union { uint16_t word; uint8_t byte[sizeof(uint16_t)]; } before, after;
	before.word = datum;
	after.byte[0] = before.byte[1];
	after.byte[1] = before.byte[0];
	return after.word;
}

/**
 * Swap the bytes in a long word.
 * @param datum is the input word.
 * @return a byte swapped output word.
 */
CXXCINLINE uint32_t amigo_byteswap32(uint32_t datum) {
	union { uint32_t word; uint8_t byte[sizeof(uint32_t)]; } before, after;
	before.word = datum;
	after.byte[0] = before.byte[3];
	after.byte[1] = before.byte[2];
	after.byte[2] = before.byte[1];
	after.byte[3] = before.byte[0];
	return after.word;
}

/**
 * Swap the bytes in a long long word.
 * @param datum is the input word.
 * @return a byte swapped output word.
 */
CXXCINLINE uint64_t amigo_byteswap64(uint64_t datum) {
	union { uint64_t word; uint8_t byte[sizeof(uint64_t)]; } before, after;
	before.word = datum;
	after.byte[0] = before.byte[7];
	after.byte[1] = before.byte[6];
	after.byte[2] = before.byte[5];
	after.byte[3] = before.byte[4];
	after.byte[4] = before.byte[3];
	after.byte[5] = before.byte[2];
	after.byte[6] = before.byte[1];
	after.byte[7] = before.byte[0];
	return after.word;
}

// The functions below following the usual POSIX/Linux pattern.

/**
 * Take a short word in host byte order and return it in network byte order.
 * @param datum is the input word.
 * @return the output word.
 */
CXXCINLINE uint16_t htons(uint16_t datum) {
	return (amigo_littleendian16() ? amigo_byteswap16(datum) : datum);
}

/**
 * Take a short word in network byte order and return it in host byte order.
 * @param datum is the input word.
 * @return the output word.
 */
CXXCINLINE uint16_t ntohs(uint16_t datum) {
	return (amigo_littleendian16() ? amigo_byteswap16(datum) : datum);
}

/**
 * Take a long word in host byte order and return it in network byte order.
 * @param datum is the input word.
 * @return the output word.
 */
CXXCINLINE uint32_t htonl(uint32_t datum) {
	return (amigo_littleendian32() ? amigo_byteswap32(datum) : datum);
}

/**
 * Take a long word in network byte order and return it in host byte order.
 * @param datum is the input word.
 * @return a byte swapped output word.
 */
CXXCINLINE uint32_t ntohl(uint32_t datum) {
	return (amigo_littleendian32() ? amigo_byteswap32(datum) : datum);
}

/**
 * Take a long long word in host byte order and return it in network byte order.
 * @param datum is the input word.
 * @return the output word.
 */
CXXCINLINE uint64_t htonll(uint64_t datum) {
	return (amigo_littleendian64() ? amigo_byteswap64(datum) : datum);
}

/**
 * Take a long long word in network byte order and return it in host byte order.
 * @param datum is the input word.
 * @return the output word.
 */
CXXCINLINE uint64_t ntohll(uint64_t datum) {
	return (amigo_littleendian64() ? amigo_byteswap64(datum) : datum);
}

#if defined(__cplusplus)

namespace com {
namespace diag {
namespace amigo {
namespace byteorder {

// Eclipse doesn't seem to grok the polymorphism of the functions below. This
// is made more complicated by the fact that the C++ function selection rules
// require I define both signed and unsigned versions.

/**
 * Take a short word and its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline uint16_t swapbytes(uint16_t datum) {
	return amigo_byteswap16(datum);
}

/**
 * Take a short word and its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline int16_t swapbytes(int16_t datum) {
	return amigo_byteswap16(datum);
}

/**
 * Take a long word and swap its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline uint32_t swapbytes(uint32_t datum) {
	return amigo_byteswap32(datum);
}

/**
 * Take a long word and swap its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline int32_t swapbytes(int32_t datum) {
	return amigo_byteswap32(datum);
}

/**
 * Take a long long word and swap its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline uint64_t swapbytes(uint64_t datum) {
	return amigo_byteswap64(datum);
}

/**
 * Take a long long word and swap its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline int64_t swapbytes(int64_t datum) {
	return amigo_byteswap64(datum);
}

/**
 * Take a short word and if the host has a different byte order from the
 * network swap its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline uint16_t swapbytesif(uint16_t datum) {
	return (amigo_littleendian16() ? amigo_byteswap16(datum) : datum);
}

/**
 * Take a short word and if the host has a different byte order from the
 * network swap its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline int16_t swapbytesif(int16_t datum) {
	return (amigo_littleendian16() ? amigo_byteswap16(datum) : datum);
}

/**
 * Take a long word and if the host has a different byte order from the
 * network swap its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline uint32_t swapbytesif(uint32_t datum) {
	return (amigo_littleendian32() ? amigo_byteswap32(datum) : datum);
}

/**
 * Take a long word and if the host has a different byte order from the
 * network swap its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline int32_t swapbytesif(int32_t datum) {
	return (amigo_littleendian32() ? amigo_byteswap32(datum) : datum);
}

/**
 * Take a long long word and if the host has a different byte order from the
 * network swap its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline uint64_t swapbytesif(uint64_t datum) {
	return (amigo_littleendian64() ? amigo_byteswap64(datum) : datum);
}
/**
 * Take a long long word and if the host has a different byte order from the
 * network swap its bytes.
 * @param datum is the input word.
 * @return the output word.
 */
inline int64_t swapbytesif(int64_t datum) {
	return (amigo_littleendian64() ? amigo_byteswap64(datum) : datum);
}

}
}
}
}

#endif

#endif /* _COM_DIAG_AMIGO_BYTEORDER_H_ */
