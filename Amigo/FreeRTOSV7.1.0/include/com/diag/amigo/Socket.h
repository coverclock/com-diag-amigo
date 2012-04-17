#ifndef _COM_DIAG_AMIGO_SOCKET_H_
#define _COM_DIAG_AMIGO_SOCKET_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/MutexSemaphore.h"

namespace com {
namespace diag {
namespace amigo {

class Socket
{

public:

	typedef uint16_t port_t;

	static const size_t IPV4ADDRESS = 4;

	static const size_t MACADDRESS = 6;

	static const port_t LOCAL = 4097;

	enum Protocol {
		TCP,
		UDP,
		IPRAW,
		MACRAW,
		PPPOE
	};

	static port_t allocate();

	explicit Socket() {};

	virtual ~Socket();

	virtual bool socket(Protocol protocol, port_t port, uint8_t flag) = 0; // Opens a socket(TCP or UDP or IP_RAW mode)

	virtual void close() = 0; // Close socket

	virtual bool connect(const uint8_t * address /* [IPV4ADDRESS] */, uint16_t port) = 0; // Establish TCP connection (Active connection)

	virtual void disconnect() = 0; // disconnect the connection

	virtual bool listen() = 0;	// Establish TCP connection (Passive connection)

	virtual ssize_t send(const void * data, size_t length) = 0; // Send data (TCP)

	virtual ssize_t recv(void * buffer, size_t length) = 0;	// Receive data (TCP)

	virtual ssize_t peek(void * buffer) = 0;

	virtual ssize_t sendto(const void * data, size_t length, const uint8_t * address /* [IPV4ADDRESS] */, port_t port) = 0; // Send data (UDP/IP RAW)

	virtual ssize_t recvfrom(void * buffer, size_t length, uint8_t * address /* [IPV4ADDRESS] */, port_t * port) = 0; // Receive data (UDP/IP RAW)

	virtual ssize_t igmpsend(const void * data, size_t length) = 0;

protected:

	static MutexSemaphore mutex;

	static port_t localport;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_SOCKET_H_ */
