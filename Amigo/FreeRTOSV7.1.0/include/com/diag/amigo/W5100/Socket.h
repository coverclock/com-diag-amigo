#ifndef _COM_DIAG_AMIGO_W5100_SOCKET_H_
#define _COM_DIAG_AMIGO_W5100_SOCKET_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by and cribbed from socket.h and socket.cpp
 * by Cristian Maglie from Arduino 1.0. It is specific to the WIZnet W5100 chip
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

/**
 * Socket implements the Amigo Socket interface for the WizNET W5100 chip.
 * The W5100 provides not just the Ethernet physical (PHY) and Media Access
 * Control (MAC) layers but the lowest layers of the Internet Protocol (IP)
 * stack for the connection-oriented reliable Transmission Control Protocol
 * (TCP) and the connection-less best-effort User Datagram Protocol (UDP) as
 * well. Socket provides an API on top of this for sending and receiving byte
 * streams or datagrams. A Client or Server layer would be implemented on top
 * of Socket.
 */
class Socket
: public com::diag::amigo::Socket
{

public:

	/**
	 * Constructor.
	 * @param myw5100 refers to the object controlling the W5100 chip.
	 * @param mysocket identifies a specific socket, or W5100::SOCKETS for none.
	 */
	explicit Socket(W5100 & myw5100, socket_t mysocket = W5100::SOCKETS)
	: com::diag::amigo::Socket(mysocket)
	, w5100(&myw5100)
	{}

	/**
	 * Destructor. If the socket is connected, it is disconnected and closed.
	 */
	~Socket();

	/**
	 * Start a packet to be sent via connectionless User Datagram Protocol to
	 * the specified address and port. The packet will be passed incrementally
	 * to the underlying implementation.
	 * @param address points to an IPV4 address in an array of four binary octets.
	 * @param port is a port number.
	 * @return true if successful, false otherwise.
	 */
	bool startUDP(const ipv4address_t * address /* [IPV4ADDRESS] */, port_t port);

	/**
	 * Add data to a UDP packet being built incrementally.
	 * @param offset is the offset from the beginning of the UDP packet.
	 * @param data points to the data to be appended.
	 * @param length is the length of the data to be appended.
	 * @return the number of bytes appended or <0 for error.
	 */
	ssize_t bufferData(size_t offset, const void * data, size_t length);

	/**
	 * Send the UDP packet that was built incrementally.
	 * @return true if successful, false otherwise.
	 */
	bool sendUDP();

	/*
	 * See <com/diag/amigo/Socket.h> for descriptions of these methods.
	 * (Doxygen automatically imports the comments from the base class.)
	 */

	virtual operator bool() const;

	virtual Socket & operator=(socket_t mysocket);

	virtual State state();

	virtual bool socket(Protocol protocol, port_t port, uint8_t flag = 0x00);

	virtual void close();

	virtual bool connect(const ipv4address_t * address /* [IPV4ADDRESS] */, uint16_t port);

	virtual void disconnect();

	virtual bool listen();

	virtual size_t free();

	virtual size_t available();

	virtual ssize_t send(const void * data, size_t length);

	virtual ssize_t recv(void * buffer, size_t length);

	virtual ssize_t peek(void * buffer);

	virtual ssize_t sendto(const void * data, size_t length, const ipv4address_t * address /* [IPV4ADDRESS] */, port_t port);

	virtual ssize_t recvfrom(void * buffer, size_t length, ipv4address_t * address /* [IPV4ADDRESS] */, port_t * port);

	virtual ssize_t igmpsend(const void * data, size_t length);

protected:

	W5100 * w5100;

};

}
}
}
}

#endif /* _COM_DIAG_AMIGO_W5100_SOCKET_H_ */
