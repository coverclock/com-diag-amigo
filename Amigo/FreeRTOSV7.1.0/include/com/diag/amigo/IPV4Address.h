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
	 * For some applications it is convenient to store the IPV4 address as a
	 * array of bytes. This is its length. Note that it is the same as the
	 * size of the IPV4 address when stored as an integer type. (This is not
	 * necessarily the case for other kinds of network addresses.)
	 */
	static const size_t LENGTH = sizeof(Word);

	/**
	 * Given a word containing an IPV4 address, generate a word containing
	 * the standard default network mask for that class of IPV4 address.
	 * For example, the network mask of the class C IPV4 address 192.168.1.253
	 * would be 255.255.255.0.
	 * @param address is an IPV4 address in host byte order.
	 * @return a network mask in host byte order.
	 */
	static Word netmask(Word address);

	/**
	 * Given a word containing an IPV4 address, and a word containing a
	 * network mask, generate a word containing the standard broadcast
	 * address. For example, the broadcast address of the class C IPV4 address
	 * 192.168.1.253 with a network mask of 255.255.255.0 would be
	 * 192.168.1.255.
	 * @param address is an IPV4 address in host byte order.
	 * @param netmask is a network mask in host byte order.
	 * @return a broadcast address in host byte order.
	 */
	static Word broadcast(Word address, Word netmask);

	/**
	 * Given a word containing an IPV4 address, a word containing a
	 * network mask, and a word containing the host part of another address,
	 * generate a new IPV4 address in the same subnetwork. For example, if
	 * the host part represents 0.0.0.1, this could be used to compute a gateway
	 * address like 192.168.1.1. If the host part represents 0.0.0.0, this be
	 * used to compute a subnetwork number like 192.168.1.0.
	 * @param address is an IPV4 address in host byte order.
	 * @param netmask is a network mask in host byte order.
	 * @param hostpart is a host part of a new address in host byte order.
	 * @return a new IPV4 address on the same subnetwork in host byte order.
	 */
	static Word address(Word address, Word netmask, Word hostpart);

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
	IPV4Address(const uint8_t * address /* [LENGTH] */)
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

/**
 * IPV4Address_D extends IPV4Address to permit construction of an Internet
 * Protocol Version 4 address based on a C-string in program memory containing
 * an IPV4 address is canonical decimal dot notation, for example
 * "192.168.1.253".
 */
struct IPV4Address_P : public IPV4Address {
	/**
	 * Constructor.
	 * @param string points to a C-string in program memory containing an
	 * IPV4 address is canonical decimal dot notation, for example
	 * "192.168.1.253".
	 */
	IPV4Address_P(PGM_P string) { aton_P(string); }
};

/**
 * IPV4Address_D extends IPV4Address to permit construction of an Internet
 * Protocol Version 4 address based on a C-string in data memory containing
 * an IPV4 address is canonical decimal dot notation, for example
 * "192.168.1.253".
 */
struct IPV4Address_D : public IPV4Address {
	/**
	 * Constructor.
	 * @param string points to a C-string in data memory containing an
	 * IPV4 address is canonical decimal dot notation, for example
	 * "192.168.1.253".
	 */
	IPV4Address_D(const char * string) { aton(string); }
};

}
}
}

#endif /* _COM_DIAG_IPV4ADDRESS_H_ */
