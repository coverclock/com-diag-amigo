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

/**
 * Socket is an abstract class that defines an interface to Berkeley socket-like
 * facilities for both server-side and client-side communication end points
 * using Internet Protocol version 4 (IPV4).
 */
class Socket
{

public:

	/**
	 * Internet Protocol has a sixteen bit port type. A pair of port numbers
	 * identify a specific service provider on the far end and a specific
	 * service requester on the near end. 0 is not a valid port number.
	 * Nominally port numbers 1024 and lower, or conventionally 4096 and
	 * lower, are reserved for well known services. For example, web service
	 * via the Hypertext Transfer Protocol (HTTP) is available on port 80.
	 */
	typedef uint16_t port_t;

	/**
	 * A four octet Internet Protocol address is used to identify a layer-3
	 * entity. Conventionally such addresses are denoted using decimal numbers
	 * for each octet in network byte order in a syntax like "192.168.1.220".
	 */
	static const size_t IPV4ADDRESS = 4;

	/**
	 * A six octet Media Access Control (MAC) address is used to identify a
	 * layer-2 entity. Conventionally such addresses are denoted using
	 * hexadecimal numbers for each octet in network byte order in a syntax
	 * like "90:a2:da:0x0d:03:4c".
	 */
	static const size_t MACADDRESS = 6;

	/**
	 * When we allocate temporary local ports, this is the number we start at.
	 * We increment it for each additional local port and wrap around when we
	 * reach 65535 or 0xffff. It is actually fairly important to separate the
	 * use of the same port number in time: even though we may think the socket
	 * has been closed, the far end may not have completed closing its own end
	 * of the connection; reusing the same port number too quickly may look to
	 * the far end like we're part of the prior connection.
	 */
	static const port_t LOCAL = 4097;

	/**
	 * These are the layer-3 protocols we know about. The underlying hardware
	 * may or may not implement all of these. Or it may implement others we
	 * don't know about.
	 */
	enum Protocol {
		PROTOCOL_TCP,
		PROTOCOL_UDP,
		PROTOCOL_IPRAW,
		PROTOCOL_MACRAW,
		PROTOCOL_PPPOE
	};

	/**
	 * These are the connection states we know about. The underlying hardware
	 * may or may not implement all of these. Or it may implement others we
	 * don't know about.
	 */
	enum State {
		STATE_CLOSED,
		STATE_INIT,
		STATE_LISTEN,
		STATE_ESTABLISHED,
		STATE_CLOSE_WAIT,
		STATE_UDP,
		STATE_IPRAW,
		STATE_MACRAW,
		STATE_PPPOE,
		STATE_SYNSENT,
		STATE_SYNRECV,
		STATE_FIN_WAIT,
		STATE_CLOSING,
		STATE_TIME_WAIT,
		STATE_LAST_ACK,
		STATE_ARP,
		STATE_OTHER
	};

	/**
	 * Allocate a new local port number that is unlikely to have been used
	 * for a while. The underlying code does not actually keep track of whether
	 * port numbers are in use or not. It merely wraps around the local port
	 * range from LOCAL to 65535 (0xffff) inclusive.
	 * @return a new local port number.
	 */
	static port_t allocate();

	/**
	 * Constructor.
	 */
	explicit Socket() {};

	/**
	 * Destructor.
	 */
	virtual ~Socket();

	/**
	 * Return the state of this socket.
	 * @return the state of this socket.
	 */
	virtual State state() = 0;

	/**
	 * Initialize this socket for a given protocol and port number. If the port
	 * number is zero, a local port is automatically allocated and used.
	 * @param protocol is the desired layer-3 protocol.
	 * @param port is the port number or zero.
	 * @param flag is an implementation-dependent flag.
	 * @return true if successful, false otherwise.
	 */
	virtual bool socket(Protocol protocol, port_t port, uint8_t flag = 0x00) = 0;

	/**
	 * Close this socket. Any underlying connection is broken.
	 */
	virtual void close() = 0;

	/**
	 * Connect this near end socket to a far end service. This is only necessary
	 * when using the reliable, connection-oriented Transmission Control Protocol
	 * (TCP) layer-3 protocol.
	 * @param address points to an IPV4 address in an array of four binary octets.
	 * @param port is a port number.
	 * @return true if successful, false otherwise.
	 */
	virtual bool connect(const uint8_t * address /* [IPV4ADDRESS] */, uint16_t port) = 0;

	/**
	 * Disconnect this near end socket from a connected far end service. The
	 * far end is notified of the disconnection.
	 */
	virtual void disconnect() = 0;

	/**
	 * Listen for a connection request from a far end client for a service on
	 * this end associated with this unconnected socket. Success here merely
	 * means that the socket was placed in the listening state.
	 * @return true if successful, false otherwise.
	 */
	virtual bool listen() = 0;

	/**
	 * Return the number of buffer bytes free for outgoing packets to be
	 * transmitted in the underlying implementation.
	 * @return the number of buffer bytes free.
	 */
	virtual size_t free() = 0;

	/**
	 * Return the number of data bytes available from incoming packets that
	 * have been received in the underlying implementation.
	 * @return the number of data bytes available.
	 */
	virtual size_t available() = 0;

	/**
	 * Send data using the reliable, connection-oriented Transmission Control
	 * Protocol (TCP) layer-3 protocol.
	 * @param data points to the data to be sent.
	 * @param length is the number of bytes to be sent.
	 * @return the number of bytes sent, 0 for closed connection, <0 for error.
	 */
	virtual ssize_t send(const void * data, size_t length) = 0;

	/**
	 * Receive data using the reliable, connection-oriented Transmission Control
	 * Protocol (TCP) layer-3 protocol. An error (which may possibly be
	 * temporary) is returned if there is no data to be received.
	 * @param buffer points to a buffer in which to place received data.
	 * @param length is the number of bytes to receive.
	 * @return the number of bytes received, 0 for closed connection, <0 for error.
	 */
	virtual ssize_t recv(void * buffer, size_t length) = 0;

	/**
	 * Peek at the first received byte using the reliable, connection-oriented
	 * Transmission Control Protocol (TCP) layer-3 protocol without actually
	 * consuming it. The same byte will be received on the subsequent receive.
	 * This method assumes that a byte is available; it will return whatever
	 * byte happens to be in the receive buffer.
	 * @param buffer points to a buffer in which to place peeked data.
	 * @return the number of bytes peeked.
	 */
	virtual ssize_t peek(void * buffer) = 0;

	/**
	 * Send data using the unreliable, connectionless User Datagram Protocol
	 * (UDP) layer-3 protocol. The address and port of the receiver must be
	 * specified.
	 * @param data points to the data to be sent.
	 * @param length is the number of bytes to be sent.
	 * @param address points to an IPV4 address in an array of four binary octets.
	 * @param port is a port number.
	 * @return the number of bytes sent, 0 for closed connection, <0 for error.
	 */
	virtual ssize_t sendto(const void * data, size_t length, const uint8_t * address /* [IPV4ADDRESS] */, port_t port) = 0;

	/**
	 * Receive data using the unreliable, connectionless User Datagram Protocol
	 * (UDP) layer-3 protocol. The address and port of the sender is returned.
	 * @param buffer points to a buffer in which to place received data.
	 * @param length is the number of bytes to receive.
	 * @param address points to where the IPV4 address of the sender will be stored.
	 * @param port points to where the port number of the sender will be stored.
	 * @return the number of bytes received, 0 for closed connection, <0 for error.
	 */
	virtual ssize_t recvfrom(void * buffer, size_t length, uint8_t * address /* [IPV4ADDRESS] */, port_t * port) = 0;

	/**
	 * Send data using the Internet Group Management Protocol (IGMP) multi-cast
	 * layer-3 protocol. I have no frakin' clue, I've never used IGMP, but
	 * it is apparently used for stuff like video streaming.
	 * @param data points to the data to be sent.
	 * @param length is the number of bytes to be sent.
	 * @return the number of bytes sent, 0 for closed connection, <0 for error.
	 */
	virtual ssize_t igmpsend(const void * data, size_t length) = 0;

protected:

	static MutexSemaphore mutex;

	static port_t localport;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_SOCKET_H_ */
