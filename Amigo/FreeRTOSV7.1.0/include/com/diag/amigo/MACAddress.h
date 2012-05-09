#ifndef _COM_DIAG_MACADDRESS_H_
#define _COM_DIAG_MACADDRESS_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <string.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/byteorder.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * MACAddress contains a Media Access Control address and implements
 * constructors and methods to import and export the address from and to
 * various forms.
 */
union MACAddress
{

public:

	typedef uint64_t Word;

	/**
	 * Constructor.
	 * The constructed address is the "no address".
	 */
	MACAddress()
	: word(0)
	{}

	/**
	 * Constructor.
	 * @param address points to an array of six octets containing the
	 * address in network byte order.
	 */
	MACAddress(const uint8_t * address /* [sizeof(bytes)] */)
	{ bytes[0] = 0; bytes[1] = 0; memcpy(bytes + 2, address, sizeof(bytes)); }

	/**
	 * Constructor.
	 * @param a is the first octet of the address in network byte order.
	 * @param b is the second octet of the address in network byte order.
	 * @param c is the third octet of the address in network byte order.
	 * @param d is the fourth octet of the address in network byte order.
	 * @param e is the fifth octet of the address in network byte order.
	 * @param f is the sixth octet of the address in network byte order.
	 */
	MACAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f)
	{ bytes[0] = 0; bytes[1] = 0; bytes[2] = a; bytes[3] = b; bytes[4] = c; bytes[5] = d; bytes[6] = e; bytes[7] = f; }

	/**
	 * Constructor.
	 * @param address is a word containing the address in host byte order.
	 */
	MACAddress(Word address)
	: word(htonll(address))
	{}

	/**
	 * Destructor.
	 */
	~MACAddress() {}

	/**
	 * Return a pointer to the octets of the address in network byte order.
	 * @return a pointer to the octets of the address in network byte order.
	 */
	operator uint8_t * const () { return bytes + 2; }

	/**
	 * Return the address in host byte order.
	 * @return the address in host byte order.
	 */
	operator Word () { return ntohll(word); }

	/**
	 * Convert a string representing an address in canonical colon notation.
	 * @param string points to a representation of an address in canonical colon
	 * notation.
	 * @return true if the address was valid, false otherwise.
	 */
	bool aton(const char * string);

	/**
	 * Express an address in a NUL-terminated string in canonical colon notation.
	 * @param buffer points to the butter into which the string is copied.
	 * @param length is the length of the buffer in bytes.
	 * @return a pointer to the buffer.
	 */
	const char * ntoa(char * buffer /* [18] */, size_t length);

protected:

	Word word;
	uint8_t bytes[sizeof(word)];

};

}
}
}

#endif /* _COM_DIAG_MACADDRESS_H_ */
