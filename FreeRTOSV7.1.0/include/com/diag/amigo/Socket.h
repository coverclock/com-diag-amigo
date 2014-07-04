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
#include "com/diag/amigo/constants.h"
#include "com/diag/amigo/Task.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * Socket is a partly abstract class that defines an interface to Berkeley
 * socket-like facilities for both server-side and client-side communication
 * with end points using Internet Protocol Version 4 (IPV4).
 */
class Socket
{
	/***************************************************************************
	 * TYPES AND CONSTANTS
	 **************************************************************************/

public:

	/**
	 * Individual socket connections are identified using a handle of this
	 * type. It would be wrong to assume that the full dynamic range of this
	 * type can be used to identify a unique socket. Typically the underlying
	 * implementation will use a (very) restricted subset of all possible
	 * socket handles.
	 */
	typedef uint8_t socket_t;

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
	 * A four octet Internet Protocol Version 4 address is used to identify a
	 * layer-3 entity. Conventionally such addresses are denoted using decimal
	 * numbers for each octet in network byte order in a syntax like
	 * "192.168.1.220". This is the length of an IPV4 address. Each octet is
	 * encoded as this type.
	 */
	typedef uint8_t ipv4address_t;

	/**
	 * A six octet IEEE 802.3 Media Access Control (MAC) address is used to
	 * identify a layer-2 entity. Conventionally such addresses are denoted
	 * using hexadecimal numbers for each octet in network byte order in a
	 * syntax like "90:a2:da:0x0d:03:4c". Each octet is encoded as this type.
	 */
	typedef uint8_t macaddress_t;

	/**
	 * A four octet Internet Protocol Version 4 address is used to identify a
	 * layer-3 entity. Conventionally such addresses are denoted using decimal
	 * numbers for each octet in network byte order in a syntax like
	 * "192.168.1.220". This is the length of an IPV4 address.
	 */
	static const size_t IPV4ADDRESS = 4;

	/**
	 * A six octet IEEE 802.3 Media Access Control (MAC) address is used to
	 * identify a layer-2 entity. Conventionally such addresses are denoted
	 * using hexadecimal numbers for each octet in network byte order in a
	 * syntax like "90:a2:da:0x0d:03:4c". This is the length of a MAC address.
	 */
	static const size_t MACADDRESS = 6;

	/**
	 * When we allocate temporary local ports, this is the number we start at.
	 * We increment it for each additional local port and wrap around when we
	 * reach MAXIMUMPORT. It is actually fairly important to separate the
	 * use of the same port number in time: even though we may think the socket
	 * has been closed, the far end may not have completed closing its own end
	 * of the connection; reusing the same port number too quickly may look to
	 * the far end like we're part of the prior connection. Wackiness may ensue.
	 */
	static const port_t LOCALPORT = 4097;

	/**
	 * This is the largest possible port value, local or otherwise.
	 */
	static const port_t MAXIMUMPORT = ~0;

	/**
	 * Zero is not a valid port number and can be used to indicate no port.
	 * (This value must be zero for the local port allocation to work
	 * correctly.)
	 */
	static const port_t NOPORT = 0;

	/**
	 * This is not a valid socket number and can be used to indicate no socket.
	 * Note that zero is a valid socket number. The underlying implementation
	 * may consider other socket values to also be invalid. But it must consider
	 * this value to be invalid.
	 */
	static const socket_t NOSOCKET = ~0;

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
	 * This is the default number of ticks the accept() methods delays between
	 * checking for socket connections.
	 */
	static const ticks_t ITERATION = 10 /* milliseconds */ / Task::PERIOD;

	/***************************************************************************
	 * CONSTRUCTING AND DESTRUCTING
	 **************************************************************************/

public:

	/**
	 * Constructor.
	 * @param mysocket identifies a specific socket or possibly no socket.
	 */
	explicit Socket(socket_t mysocket = NOSOCKET)
	: sock(mysocket)
	{};

	/**
	 * Destructor.
	 */
	virtual ~Socket();

	/**
	 * Associate this object with the specified underlying socket, or
	 * disassociate it from any underlying socket.
	 * @param mysocket identifies a specific socket or possibly no socket.
	 */
	virtual Socket & operator=(socket_t mysocket) { sock = mysocket; return *this; }

	/**
	 * Return the socket number for this object.
	 * @return the socket number for this object.
	 */
	virtual operator socket_t() const { return sock; }

	/**
	 * Return true if this object represents an underlying socket.
	 * @return true if this object represents an underlying socket.
	 */
	virtual operator bool() const = 0;

	/***************************************************************************
	 * MANAGING
	 **************************************************************************/

public:


	/**
	 * Associate this socket to an unused underlying physical resource if it is
	 * available. If this fails then all the sockets are busy.
	 * @return true if successful, false otherwise.
	 */
	virtual bool socket() = 0;

	/**
	 * Bind this socket to a given protocol and port number. If the port
	 * number is zero, a local port is automatically allocated and used.
	 * @param protocol is the desired layer-3 protocol.
	 * @param port is the port number or zero.
	 * @param flag is an implementation-dependent flag.
	 * @return true if successful, false otherwise.
	 */
	virtual bool bind(Protocol protocol, port_t port = NOPORT, uint8_t flag = 0x00) = 0;

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
	virtual bool connect(const ipv4address_t * address /* [IPV4ADDRESS] */, uint16_t port) = 0;

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
	 * Return true if a connection request as arrived from a far end client.
	 * The application can explicitly poll this routine, or by providing
	 * appropriate timeout and iteration durations in ticks, have the method
	 * poll itself. The method fails if there is no connection request, or if
	 * the socket has been or is being closed.
	 * @return true if successful, false otherwise.
	 */
	virtual bool accept(ticks_t timeout = IMMEDIATELY, ticks_t iteration = ITERATION) = 0;

	/***************************************************************************
	 * MONITORING
	 **************************************************************************/

public:

	/**
	 * Return true if the socket is in the listening state, false otherwise.
	 * Only sockets used by the near end acting as a server and waiting for
	 * incoming connection requests will be in this state.
	 * @return true if listening, false otherwise.
	 */
	virtual bool listening() = 0;

	/**
	 * Return true if the socket is in a connected state, false otherwise. A
	 * socket that is closing is still considered connected if there is incoming
	 * data queued and waiting to be received by the application.
	 * @return true if connected, false otherwise.
	 */
	virtual bool connected() = 0;

	/**
	 * Return true if the socket is in a disconnected state, false otherwise.
	 * A socket is considered disconnected only if the far end disconnected and
	 * there is no incoming data queued and waiting to be received by the
	 * application.
	 * @return true if disconnected, false otherwise.
	 */
	virtual bool disconnected() = 0;

	/**
	 * Return true if the socket is in a closing state, false otherwise. This
	 * is a transitional state in which the near end and far end are negotiating
	 * closing down the socket. The socket will be in this state only very
	 * briefly.
	 * @return true if closing, false otherwise.
	 */
	virtual bool closing() = 0;

	/**
	 * Return true if the socket is in the closed state, false otherwise. No
	 * further action on this socket is possible. The application still needs
	 * to do a close() on the near end to release resources.
	 * @return true if closed, false otherwise.
	 */
	virtual bool closed() = 0;

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

	/***************************************************************************
	 * SENDING AND RECEIVING
	 **************************************************************************/

public:

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
	virtual ssize_t sendto(const void * data, size_t length, const ipv4address_t * address /* [IPV4ADDRESS] */, port_t port) = 0;

	/**
	 * Receive data using the unreliable, connectionless User Datagram Protocol
	 * (UDP) layer-3 protocol. The address and port of the sender is returned.
	 * @param buffer points to a buffer in which to place received data.
	 * @param length is the number of bytes to receive.
	 * @param address points to where the IPV4 address of the sender will be stored.
	 * @param port points to where the port number of the sender will be stored.
	 * @return the number of bytes received, 0 for closed connection, <0 for error.
	 */
	virtual ssize_t recvfrom(void * buffer, size_t length, ipv4address_t * address /* [IPV4ADDRESS] */, port_t * port) = 0;

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

	socket_t sock;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_SOCKET_H_ */
