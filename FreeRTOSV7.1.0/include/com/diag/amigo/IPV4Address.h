#ifndef _COM_DIAG_IPV4ADDRESS_H_
#define _COM_DIAG_IPV4ADDRESS_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <string.h>
#include <avr/pgmspace.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/byteorder.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * IPV4Address encapsulates an Internet Protocol Version 4 address and
 * implements constructors and methods to import and export the address from
 * and to various forms.
 */
class IPV4Address
{

public:

	/**
	 * For some applications it is convenient (and possible) to store the
	 * IP address as a single integer type. This is that type.
	 */
	typedef uint32_t Word;

	/**
	 * Constructor.
	 * The constructed address is the "no address".
	 */
	IPV4Address()
	{
		payload.word = 0;
	}

	/**
	 * Constructor.
	 * @param address points to an array of four octets containing the
	 * address in network byte order.
	 */
	IPV4Address(const uint8_t * address /* [sizeof(payload.bytes)] */)
	{
		memcpy(payload.bytes, address, sizeof(payload.bytes));
	}

	/**
	 * Constructor.
	 * @param a is the first octet of the address in network byte order.
	 * @param b is the second octet of the address in network byte order.
	 * @param c is the third octet of the address in network byte order.
	 * @param d is the fourth octet of the address in network byte order.
	 */
	IPV4Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
	{
		payload.bytes[0] = a;
		payload.bytes[1] = b;
		payload.bytes[2] = c;
		payload.bytes[3] = d;
	}

	/**
	 * Constructor.
	 * @param address is a word containing the address in host byte order.
	 */
	IPV4Address(Word address)
	{
		payload.word = htonl(address);
	}

	/**
	 * Destructor.
	 */
	~IPV4Address() {}

	/**
	 * Return true if construction succeeded. Success means the underlying IPV4
	 * address is non-zero.
	 * @return true for success, false otherwise.
	 */
	operator bool () const { return (payload.word != 0); }

	/**
	 * Return a pointer to the octets of the address in network byte order.
	 * @return a pointer to the octets of the address in network byte order.
	 */
	operator uint8_t * const () { return payload.bytes; }

	/**
	 * Return the address in host byte order.
	 * @return the address in host byte order.
	 */
	operator Word () { return ntohl(payload.word); }

	/**
	 * Convert a string representing an address in canonical dot notation from
	 * data memory.
	 * @param string points to a representation of an address in canonical dot
	 * notation.
	 * @return true if the address was valid, false otherwise.
	 */
	bool aton(const char * string);

	/**
	 * Convert a string representing an address in canonical dot notation from
	 * program memory.
	 * @param string points to a representation of an address in canonical dot
	 * notation.
	 * @return true if the address was valid, false otherwise.
	 */
	bool aton_P(PGM_P string);

	/**
	 * Express an address in a NUL-terminated string in canonical dot notation.
	 * @param buffer points to the butter into which the string is copied.
	 * @param length is the length of the buffer in bytes.
	 * @return a pointer to the buffer.
	 */
	const char * ntoa(char * buffer /* [sizeof("NNN.NNN.NNN.NNN")] */, size_t length);

protected:

	union {
		Word word;
		uint8_t bytes[sizeof(word)];
	} payload;

};

class IPV4Address_P : public IPV4Address {
public: IPV4Address_P(PGM_P string) { aton_P(string); }
};

}
}
}

#endif /* _COM_DIAG_IPV4ADDRESS_H_ */
