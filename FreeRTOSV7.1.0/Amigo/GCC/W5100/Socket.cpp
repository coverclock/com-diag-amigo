/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by and cribbed from w5100h and w5100.cpp by
 * Cristian Maglie from Arduino 1.0. It is specific to the WIZnet W5100 chip
 * as used on the Arduino Ethernet shield and the FreeTronics EtherMega 2560.
 * It may work on other compatibles and clones. Your mileage may vary. It is
 * also based on the W5100 unit test stubs in the Amigo unit test suite.
 */

#include "com/diag/amigo/W5100/Socket.h"
#include "com/diag/amigo/CriticalSection.h"
#include "com/diag/amigo/countof.h"

namespace com {
namespace diag {
namespace amigo {
namespace W5100 {

static Socket::port_t busy[W5100::SOCKETS] = { Socket::NOPORT };

static MutexSemaphore * mutex = 0;

static Socket::port_t localport = Socket::LOCALPORT;

void Socket::provide(MutexSemaphore & mymutex) {
	mutex = &mymutex;
}

Socket::~Socket() {
	close();
}

Socket::operator bool() const {
	return (sock < W5100::SOCKETS);
}

Socket & Socket::operator=(socket_t mysocket) {
	sock = mysocket;
	return *this;
}

bool Socket::socket() {
	uint8_t state;
	if (sock < W5100::SOCKETS) {
		state = w5100->state(sock);
		if (state != W5100::SnSR::CLOSED) {
			close();
		}
	}
	for (socket_t candidate = 0; candidate < W5100::SOCKETS; ++candidate) {
		state = w5100->state(candidate);
		if (state == W5100::SnSR::CLOSED) {
			sock = candidate;
			break;
		}
	}
	return (sock != NOSOCKET);
}

bool Socket::bind(Protocol protocol, port_t port, uint8_t flag) {
	if (sock >= W5100::SOCKETS) {
		return false;
	}

	uint8_t proto;
	switch (protocol) {
	case PROTOCOL_TCP:		proto = W5100::SnMR::TCP;		break;
	case PROTOCOL_UDP:		proto = W5100::SnMR::UDP;		break;
	case PROTOCOL_IPRAW:	proto = W5100::SnMR::IPRAW;		break;
	case PROTOCOL_MACRAW:	proto = W5100::SnMR::MACRAW;	break;
	case PROTOCOL_PPPOE:	proto = W5100::SnMR::PPPOE;		break;
	default:
		return false;
	}

	{
		CriticalSection cs(mutex);

		// Since there are far more local port numbers than there are sockets
		// in the W5100, this is guaranteed to complete. If the application
		// has a resource leak because it forgot to close a socket, then it
		// may never find an unused socket number to assign to this object.
		// But that won't effect this algorithm.

		while (port == NOPORT) {
			port = localport++;
			if (localport == NOPORT) {
				localport = LOCALPORT;
			}
			for (uint8_t ii = 0; ii < countof(busy); ++ii) {
				if (busy[ii] == port) {
					port = NOPORT;
					break;
				}
			}
		}

		busy[sock] = port;
	}

	w5100->execCmdSn(sock, W5100::Sock_CLOSE);
	w5100->writeSnIR(sock, 0xFF);
	w5100->writeSnMR(sock, proto | flag);
	w5100->writeSnPORT(sock, port);
	w5100->execCmdSn(sock, W5100::Sock_OPEN);

	return true;
}

void Socket::close() {
	if (sock >= W5100::SOCKETS) {
		return;
	}
	w5100->execCmdSn(sock, W5100::Sock_CLOSE);
	w5100->writeSnIR(sock, 0xFF);
	{
		CriticalSection cs(mutex);
		busy[sock] = NOPORT;
	}
	sock = NOSOCKET;
}

bool Socket::listen() {
	if (sock >= W5100::SOCKETS) { return false; }
	if (w5100->state(sock) != W5100::SnSR::INIT) { return false; }
	w5100->execCmdSn(sock, W5100::Sock_LISTEN);
	return true;
}

bool Socket::accept(ticks_t timeout, ticks_t iteration) {
	if (sock >= W5100::SOCKETS) { return false; }
	uint8_t state = w5100->state(sock);
	if ((state == W5100::SnSR::ESTABLISHED) || (state == W5100::SnSR::CLOSE_WAIT)) { return true; }
	if (timeout == IMMEDIATELY) { return false; }
	ticks_t then = Task::elapsed();
	while (true) {
		Task::delay(iteration);
		if (sock >= W5100::SOCKETS) { return false; }
		state = w5100->state(sock);
		if ((state == W5100::SnSR::ESTABLISHED) || (state == W5100::SnSR::CLOSE_WAIT)) { return true; }
		if ((state == W5100::SnSR::CLOSED) || (state == W5100::SnSR::FIN_WAIT) || (state == W5100::SnSR::CLOSING) || (state == W5100::SnSR::TIME_WAIT) || (state == W5100::SnSR::LAST_ACK)) { return false; }
		if (timeout == NEVER) { continue; }
		if ((Task::elapsed() - then) >= timeout) { return false; }
	}
}

bool Socket::connect(const ipv4address_t * address, port_t port) {
	if (sock >= W5100::SOCKETS) {
		return false;
	}

	if (
		((address[0] == 0xff) && (address[1] == 0xff) && (address[2] == 0xff) && (address[3] == 0xff)) ||
		((address[0] == 0x00) && (address[1] == 0x00) && (address[2] == 0x00) && (address[3] == 0x00)) ||
		(port == NOPORT)
	) {
		return false;
	}

	// Set destination IP.
	w5100->writeSnDIPR(sock, address);
	w5100->writeSnDPORT(sock, port);
	w5100->execCmdSn(sock, W5100::Sock_CONNECT);

	return true;
}

void Socket::disconnect() {
	if (sock < W5100::SOCKETS) {
		w5100->execCmdSn(sock, W5100::Sock_DISCON);
	}
}

size_t Socket::free() {
	return ((sock < W5100::SOCKETS) ? w5100->getTXFreeSize(sock) : 0);
}

size_t Socket::available() {
	return ((sock < W5100::SOCKETS) ? w5100->getRXReceivedSize(sock) : 0);
}

bool Socket::listening() {
	return ((sock < W5100::SOCKETS) && (w5100->state(sock) == W5100::SnSR::LISTEN));
}

bool Socket::connected() {
	if (sock >= W5100::SOCKETS) { return false; }
	uint8_t state = w5100->state(sock);
	// We do it this way because the far end could have connected, sent us some
	// data, and disconnected, all before we did this check. So there is data
	// for us to receive even though the far end is gone.
	return ((state == W5100::SnSR::ESTABLISHED) || ((state == W5100::SnSR::CLOSE_WAIT) && (w5100->getRXReceivedSize(sock) > 0)));
}

bool Socket::disconnected() {
	// We only consider our self truly disconnected if there is no data for us
	// to read.
	return ((sock < W5100::SOCKETS) && (w5100->state(sock) == W5100::SnSR::CLOSE_WAIT) && (w5100->getRXReceivedSize(sock) == 0));
}

bool Socket::closing() {
	if (sock >= W5100::SOCKETS) { return false; }
	uint8_t state = w5100->state(sock);
	return ((state == W5100::SnSR::FIN_WAIT) || (state == W5100::SnSR::CLOSING) || (state == W5100::SnSR::TIME_WAIT) || (state == W5100::SnSR::LAST_ACK));
}

bool Socket::closed() {
	return ((sock < W5100::SOCKETS) && (w5100->state(sock) == W5100::SnSR::CLOSED));
}

ssize_t Socket::send(const void * data, size_t length) {
	if (sock >= W5100::SOCKETS) {
		return -2;
	}

	ssize_t result = (length < W5100::SSIZE) ? length : W5100::SSIZE; // check size not to exceed MAX size.
	size_t freesize;
	uint8_t state;

	// If freebuf is available, start.
	do {
		freesize = w5100->getTXFreeSize(sock);
		state = w5100->state(sock);
		if ((state != W5100::SnSR::ESTABLISHED) && (state != W5100::SnSR::CLOSE_WAIT)) {
			result = 0;
			break;
		}
	} while (freesize < result);

	// Copy data.
	w5100->send_data_processing(sock, data, result);
	w5100->execCmdSn(sock, W5100::Sock_SEND);

	/* +2008.01 bj */
	while ((w5100->readSnIR(sock) & W5100::SnIR::SEND_OK) != W5100::SnIR::SEND_OK) {
		/* m2008.01 [bj] : reduce code */
		if (w5100->state(sock) == W5100::SnSR::CLOSED) {
			close();
			return 0;
		}
	}

	/* +2008.01 bj */
	w5100->writeSnIR(sock, W5100::SnIR::SEND_OK);

	return result;
}


ssize_t Socket::recv(void * buffer, size_t length) {
	if (sock >= W5100::SOCKETS) {
		return -2;
	}

	size_t result = w5100->getRXReceivedSize(sock); // Check how much data is available

	if (result == 0) {
		// No data available.
		switch (w5100->state(sock)) {
		case W5100::SnSR::LISTEN:
		case W5100::SnSR::CLOSED:
		case W5100::SnSR::CLOSE_WAIT:
			// The remote end has closed its side of the connection: EOF.
			return 0;
		default:
			// The connection is still up, but there's no data waiting to be read: ERROR.
			return -1;
		}
	}

	if (result > length) {
		result = length;
	}

	if (result > 0) {
		w5100->recv_data_processing(sock, buffer, result);
		w5100->execCmdSn(sock, W5100::Sock_RECV);
	}

	return result;
}

ssize_t Socket::peek(void * buffer) {
	if (sock >= W5100::SOCKETS) {
		return -2;
	}
	w5100->recv_data_processing(sock, buffer, 1, 1);
	return 1;
}

ssize_t Socket::sendto(const void * data, size_t length, const ipv4address_t * address , port_t port) {
	if (sock >= W5100::SOCKETS) {
		return -2;
	}

	ssize_t result = (length < W5100::SSIZE) ? length : W5100::SSIZE; // check size not to exceed MAX size.

	if (
		((address[0] == 0x00) && (address[1] == 0x00) && (address[2] == 0x00) && (address[3] == 0x00)) ||
		(port == 0x0000) ||
		(result == 0)
	) {
		/* +2008.01 [bj] : added return value */
		result = 0;
	} else {
		w5100->writeSnDIPR(sock, address);
		w5100->writeSnDPORT(sock, port);

		// Copy data.
		w5100->send_data_processing(sock, data, result);
		w5100->execCmdSn(sock, W5100::Sock_SEND);

		/* +2008.01 bj */
		while ( (w5100->readSnIR(sock) & W5100::SnIR::SEND_OK) != W5100::SnIR::SEND_OK )
		{
			if (w5100->readSnIR(sock) & W5100::SnIR::TIMEOUT)
			{
				/* +2008.01 [bj]: clear interrupt */
				w5100->writeSnIR(sock, (W5100::SnIR::SEND_OK | W5100::SnIR::TIMEOUT)); /* clear SEND_OK & TIMEOUT */
				return 0;
			}
		}

		/* +2008.01 bj */
		w5100->writeSnIR(sock, W5100::SnIR::SEND_OK);
	}
	return result;
}


ssize_t Socket::recvfrom(void * buffer, size_t length, ipv4address_t * address, port_t * port) {
	if (sock >= W5100::SOCKETS) {
		return -2;
	}

	ssize_t result = 0;
	uint8_t head[8];
	W5100::address_t ptr = 0;

	if (length > 0) {

		ptr = w5100->readSnRX_RD(sock);

		switch (w5100->readSnMR(sock) & 0x07) {

		case W5100::SnMR::UDP :
			w5100->read_data(sock, ptr, head, 8);
			ptr += 8;

			// read peer's IP address, port number.
			address[0] = head[0];
			address[1] = head[1];
			address[2] = head[2];
			address[3] = head[3];
			*port = head[4];
			*port = (*port << 8) + head[5];
			result = head[6];
			result = (result << 8) + head[7];

			w5100->read_data(sock, ptr, buffer, result); // Data copy.
			ptr += result;

			w5100->writeSnRX_RD(sock, ptr);
			break;

		case W5100::SnMR::IPRAW :
			w5100->read_data(sock, ptr, head, 6);
			ptr += 6;

			address[0] = head[0];
			address[1] = head[1];
			address[2] = head[2];
			address[3] = head[3];
			result = head[4];
			result = (result << 8) + head[5];

			w5100->read_data(sock, ptr, buffer, result); // Data copy.
			ptr += result;

			w5100->writeSnRX_RD(sock, ptr);
			break;

		case W5100::SnMR::MACRAW:
			w5100->read_data(sock, ptr, head, 2);
			ptr += 2;

			result = head[0];
			result = (result << 8) + head[1] - 2;

			w5100->read_data(sock, ptr, buffer, result);
			ptr += result;

			w5100->writeSnRX_RD(sock, ptr);
			break;

		default:
			break;

		}

		w5100->execCmdSn(sock, W5100::Sock_RECV);
	}

	return result;
}


ssize_t Socket::igmpsend(const void * data, size_t length) {
	if (sock >= W5100::SOCKETS) {
		return -2;
	}

	ssize_t result = (length < W5100::SSIZE) ? length : W5100::SSIZE; // Check size not to exceed MAX size.
	uint8_t status;

	if (result == 0) {
		return 0;
	}

	w5100->send_data_processing(sock, data, result);
	w5100->execCmdSn(sock, W5100::Sock_SEND);

	while ((w5100->readSnIR(sock) & W5100::SnIR::SEND_OK) != W5100::SnIR::SEND_OK) {
		status = w5100->readSnSR(sock);
		if ((w5100->readSnIR(sock) & W5100::SnIR::TIMEOUT) != 0) {
			/* In case of IGMP, if send fails, then socket closed. */
			/* If you want change, remove this code. */
			close();
			return 0;
		}
	}

	w5100->writeSnIR(sock, W5100::SnIR::SEND_OK);

	return result;
}

ssize_t Socket::bufferData(size_t offset, const void * data, size_t length) {
	if (sock >= W5100::SOCKETS) {
		return -2;
	}

	size_t free = w5100->getTXFreeSize(sock);
	ssize_t result = (length < free) ? length : free;

	w5100->send_data_processing_offset(sock, offset, data, result);

	return result;
}

bool Socket::startUDP(const ipv4address_t * address, port_t port) {
	if (sock >= W5100::SOCKETS) {
		return false;
	}

	if (
		((address[0] == 0x00) && (address[1] == 0x00) && (address[2] == 0x00) && (address[3] == 0x00)) ||
		(port == 0x00)
	) {
		return false;
	} else {
		w5100->writeSnDIPR(sock, address);
		w5100->writeSnDPORT(sock, port);
		return true;
	}
}

bool Socket::sendUDP() {
	if (sock >= W5100::SOCKETS) {
		return false;
	}

	w5100->execCmdSn(sock, W5100::Sock_SEND);

	/* +2008.01 bj */
	while ((w5100->readSnIR(sock) & W5100::SnIR::SEND_OK) != W5100::SnIR::SEND_OK) {
		if (w5100->readSnIR(sock) & W5100::SnIR::TIMEOUT)
		{
			/* +2008.01 [bj]: clear interrupt */
			w5100->writeSnIR(sock, (W5100::SnIR::SEND_OK | W5100::SnIR::TIMEOUT));
			return false;
		}
	}

	/* +2008.01 bj */
	w5100->writeSnIR(sock, W5100::SnIR::SEND_OK);

	/* Sent ok. */
	return true;
}

}
}
}
}
