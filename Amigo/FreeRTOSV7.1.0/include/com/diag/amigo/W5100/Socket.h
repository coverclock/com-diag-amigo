#ifndef _COM_DIAG_AMIGO_W5100_SOCKET_H_
#define _COM_DIAG_AMIGO_W5100_SOCKET_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by and cribbed from w5100h and w5100.cpp by
 * Cristian Maglie from Arduino 1.0. It is specific to the WIZnet W5100 chip
 * as used on the Arduino Ethernet shield and the FreeTronics EtherMega 2560.
 * It may work on other compatibles and clones. Your mileage may vary. It also
 * borrows from the SPI test fixture in the Amigo unit test suite.
 */

#include "com/diag/amigo/Socket.h"
#include "com/diag/amigo/W5100/W5100.h"

namespace com {
namespace diag {
namespace amigo {
namespace W5100 {

class Socket
: public com::diag::amigo::Socket
{

public:

	typedef W5100::socket_t socket_t;

	explicit Socket(W5100 & myw5100, socket_t mysocket = W5100::SOCKETS)
	: w5100(&myw5100)
	, sock(mysocket)
	{}

	~Socket();

	operator bool() const { return (sock != W5100::SOCKETS); }

	Socket & operator=(socket_t mysocket) { sock = mysocket; return *this; }

	virtual State state();

	virtual bool socket(Protocol protocol, port_t port, uint8_t flag = 0x00); // Opens a socket(TCP or UDP or IP_RAW mode)

	virtual void close(); // Close socket

	virtual bool connect(const uint8_t * address /* [IPV4ADDRESS] */, uint16_t port); // Establish TCP connection (Active connection)

	virtual void disconnect(); // disconnect the connection

	virtual bool listen();	// Establish TCP connection (Passive connection)

	virtual size_t free();

	virtual size_t available();

	virtual ssize_t send(const void * data, size_t length); // Send data (TCP)

	virtual ssize_t recv(void * buffer, size_t length);	// Receive data (TCP)

	virtual ssize_t peek(void * buffer);

	virtual ssize_t sendto(const void * data, size_t length, const uint8_t * address /* [IPV4ADDRESS] */, port_t port); // Send data (UDP/IP RAW)

	virtual ssize_t recvfrom(void * buffer, size_t length, uint8_t * address /* [IPV4ADDRESS] */, port_t * port); // Receive data (UDP/IP RAW)

	virtual ssize_t igmpsend(const void * data, size_t length);

	bool startUDP(const uint8_t * address /* [IPV4ADDRESS] */, port_t port);

	ssize_t bufferData(size_t offset, const void * data, size_t length);

	bool sendUDP();

protected:

	W5100 * w5100;
	socket_t sock;

};

}
}
}
}

#endif /* _COM_DIAG_AMIGO_W5100_SOCKET_H_ */
