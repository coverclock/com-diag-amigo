#ifndef _COM_DIAG_AMIGO_W5100_W5100_H_
#define _COM_DIAG_AMIGO_W5100_W5100_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by and cribbed from w5100.h and w5100.cpp by
 * Cristian Maglie from Arduino 1.0. It is specific to the WIZnet W5100 chip
 * as used on the Arduino Ethernet shield and the FreeTronics EtherMega 2560.
 * It may work on other compatibles and clones. Your mileage may vary. It also
 * borrows from the SPI test fixture in the Amigo unit test suite.
 */

#include "com/diag/amigo/Socket.h"
#include "com/diag/amigo/target/GPIO.h"
#include "com/diag/amigo/target/SPI.h"
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/MutexSemaphore.h"
#include "com/diag/amigo/Task.h"

namespace com {
namespace diag {
namespace amigo {
namespace W5100 {

class Socket;

/**
 * W5100 implements a device control interface to the WIZnet W5100 chip, which
 * provides not only an Ethernet interface but an internet protocol stack in
 * silicon. This interface uses uses the Amigo interrupt-driven SPI driver to
 * talk to the W5100 chip, the Amigo GPIO interface to control the Slave Select
 * pin, and expects to synchronize its use of the shared SPI bus with an Amigo
 * MutexSemaphore.
 *
 * Reference: WIZnet, "W5100 Datasheet", Version 1.2.4, 2011
 */
class W5100
{

	friend class com::diag::amigo::W5100::Socket;

	/***************************************************************************
	 * TYPES AND CONSTANTS
	 **************************************************************************/

public:

	/**
	 * This is the integral type used as a memory address when reading and
	 * writing memory in the W5100 via SPI.
	 */
	typedef uint16_t address_t;

	typedef com::diag::amigo::Socket::socket_t socket_t;

	typedef com::diag::amigo::Socket::port_t port_t;

	typedef com::diag::amigo::Socket::ipv4address_t ipv4address_t;

	typedef com::diag::amigo::Socket::macaddress_t macaddress_t;

	/**
	 * The W5100 is software-reset every time it is started. This is the
	 * default number of ticks to delay after starting.
	 * This is a long time, relatively speaking. See "Reset Timing", p. 64.
	 */
	static const ticks_t RESETTING = 10 /* milliseconds */ / Task::PERIOD;

	/**
	 * Legitimate values for variables of type socket_t in this implementation
	 * range from zero to (SOCKETS - 1). A value outside of this range (like,
	 * SOCKETS to pick an example completely at random, or NOSOCKET defined
	 * in the Socket base class) means this object is not connected to an
	 * active socket.
	 */
	static const size_t SOCKETS = 4;

	/**
	 * This is the size of an individual transmit buffer for a socket. Packets
	 * larger than this cannot be sent.
	 */
	static const size_t SSIZE = 2048;

	/**
	 * This is the size of an individual receive buffer for a socket. Packets
	 * larger than this cannot be received.
	 */
	static const size_t RSIZE = 2048;

	/***************************************************************************
	 * CONSTRUCTING AND DESTRUCTING
	 **************************************************************************/

public:

	/**
	 * Constructor.
	 * @param mymutex refers to a mutex used to synchronize access to the SPI.
	 * @param myss specifies the Slave Select pin for the W5100.
	 * @param myspi refers to the SPI.
	 */
	explicit W5100(MutexSemaphore & mymutex, GPIO::Pin myss, SPI & myspi);

	/**
	 * Constructor. This version of the constructor does not require a
	 * MutexSemaphore and should only be used when the W5100 is the only
	 * SPI slave device. Doing so can save a little memory.
	 * @param myss specifies the Slave Select pin for the W5100.
	 * @param myspi refers to the SPI.
	 */
	explicit W5100(GPIO::Pin myss, SPI & myspi);

	/**
	 * Destructor.
	 */
	virtual ~W5100();

	/***************************************************************************
	 * STARTING AND STOPPING
	 **************************************************************************/

public:

	/**
	 * Start the W5100.
	 * @param resetting is the number of ticks to delay following a reset.
	 */
	void start(ticks_t resetting = RESETTING);

	/**
	 * Stop the W5100.
	 * @param resetting is the number of ticks to delay following a reset.
	 */
	void stop(ticks_t resetting = RESETTING);

	/***************************************************************************
	 * INITIALIZING
	 **************************************************************************/

protected:

	enum SockCMD {
	  Sock_OPEN      = 0x01,
	  Sock_LISTEN    = 0x02,
	  Sock_CONNECT   = 0x04,
	  Sock_DISCON    = 0x08,
	  Sock_CLOSE     = 0x10,
	  Sock_SEND      = 0x20,
	  Sock_SEND_MAC  = 0x21,
	  Sock_SEND_KEEP = 0x22,
	  Sock_RECV      = 0x40
	};

	class SnMR {
	public:
	  static const uint8_t CLOSE  = 0x00;
	  static const uint8_t TCP    = 0x01;
	  static const uint8_t UDP    = 0x02;
	  static const uint8_t IPRAW  = 0x03;
	  static const uint8_t MACRAW = 0x04;
	  static const uint8_t PPPOE  = 0x05;
	  static const uint8_t ND     = 0x20;
	  static const uint8_t MULTI  = 0x80;
	};

	class SnIR {
	public:
	  static const uint8_t SEND_OK = 0x10;
	  static const uint8_t TIMEOUT = 0x08;
	  static const uint8_t RECV    = 0x04;
	  static const uint8_t DISCON  = 0x02;
	  static const uint8_t CON     = 0x01;
	};

	class SnSR {
	public:
	  static const uint8_t CLOSED      = 0x00;
	  static const uint8_t ARP         = 0x01;
	  static const uint8_t INIT        = 0x13;
	  static const uint8_t LISTEN      = 0x14;
	  static const uint8_t SYNSENT     = 0x15;
	  static const uint8_t SYNRECV     = 0x16;
	  static const uint8_t ESTABLISHED = 0x17;
	  static const uint8_t FIN_WAIT    = 0x18;
	  static const uint8_t CLOSING     = 0x1A;
	  static const uint8_t TIME_WAIT   = 0x1B;
	  static const uint8_t CLOSE_WAIT  = 0x1C;
	  static const uint8_t LAST_ACK    = 0x1D;
	  static const uint8_t UDP         = 0x22;
	  static const uint8_t IPRAW       = 0x32;
	  static const uint8_t MACRAW      = 0x42;
	  static const uint8_t PPPOE       = 0x5F;
	};

	class IPPROTO {
	public:
	  static const uint8_t IP   = 0;
	  static const uint8_t ICMP = 1;
	  static const uint8_t IGMP = 2;
	  static const uint8_t GGP  = 3;
	  static const uint8_t TCP  = 6;
	  static const uint8_t PUP  = 12;
	  static const uint8_t UDP  = 17;
	  static const uint8_t IDP  = 22;
	  static const uint8_t ND   = 77;
	  static const uint8_t RAW  = 255;
	};

private:

	static const uint8_t RST = 7; // Reset BIT

	static const uint16_t SMASK = SSIZE - 1; // Tx buffer mask

	static const uint16_t RMASK = RSIZE - 1; // Rx buffer mask

	static const uint16_t TX_BUF = 0x1100;

	static const uint16_t RX_BUF = (TX_BUF + SSIZE);

	static const uint16_t TXBUF_BASE = 0x4000;

	static const uint16_t RXBUF_BASE = 0x6000;

	static const uint16_t CH_BASE = 0x0400;

	static const uint16_t CH_SIZE = 0x0100;

	void initialize();

	/***************************************************************************
	 * GENERAL PURPOSE REGISTER OPERATING
	 **************************************************************************/

private:

	void write(address_t address, uint8_t datum);

	void write(address_t address, const void * data, size_t length);

	uint8_t read(address_t address);

	void read(address_t address, void * buffer, size_t length);

protected:

#define COM_DIAG_AMIGO_W5100_GP_8(_NAME_, _ADDRESS_)			\
	void write##_NAME_(uint8_t datum) {							\
		write(_ADDRESS_, datum);								\
	}															\
	uint8_t read##_NAME_() {									\
		return read(_ADDRESS_);									\
	}

#define COM_DIAG_AMIGO_W5100_GP_16(_NAME_, _ADDRESS_)			\
	void write##_NAME_(uint16_t datum) {						\
		write(_ADDRESS_,     datum >> 8);						\
		write(_ADDRESS_ + 1, datum & 0xff);						\
	}															\
	uint16_t read##_NAME_() {									\
		uint16_t result = read(_ADDRESS_);						\
		result = (result << 8) + read(_ADDRESS_ + 1);			\
		return result;                                     		\
	}

#define COM_DIAG_AMIGO_W5100_GP_N(_NAME_, _ADDRESS_, _SIZE_)	\
	void write##_NAME_(const void * data) {						\
		write(_ADDRESS_, data, _SIZE_);							\
	}															\
	void read##_NAME_(void * buffer) {							\
		read(_ADDRESS_, buffer, _SIZE_);						\
	}

	COM_DIAG_AMIGO_W5100_GP_8 (MR,		0x0000);	// Mode
	COM_DIAG_AMIGO_W5100_GP_N (GAR,		0x0001, 4);	// Gateway IP address
	COM_DIAG_AMIGO_W5100_GP_N (SUBR,	0x0005, 4);	// Subnet mask address
	COM_DIAG_AMIGO_W5100_GP_N (SHAR,	0x0009, 6);	// Source MAC address
	COM_DIAG_AMIGO_W5100_GP_N (SIPR,	0x000F, 4);	// Source IP address
	COM_DIAG_AMIGO_W5100_GP_8 (IR,		0x0015);	// Interrupt
	COM_DIAG_AMIGO_W5100_GP_8 (IMR,		0x0016);	// Interrupt Mask
	COM_DIAG_AMIGO_W5100_GP_16(RTR,		0x0017);	// Timeout address
	COM_DIAG_AMIGO_W5100_GP_8 (RCR,		0x0019);	// Retry count
	COM_DIAG_AMIGO_W5100_GP_8 (RMSR,	0x001A);	// Receive memory size
	COM_DIAG_AMIGO_W5100_GP_8 (TMSR,	0x001B);	// Transmit memory size
	COM_DIAG_AMIGO_W5100_GP_8 (PATR,	0x001C);	// Authentication type address in PPPoE mode
	COM_DIAG_AMIGO_W5100_GP_8 (PTIMER,	0x0028);	// PPP LCP Request Timer
	COM_DIAG_AMIGO_W5100_GP_8 (PMAGIC,	0x0029);	// PPP LCP Magic Number
	COM_DIAG_AMIGO_W5100_GP_N (UIPR,	0x002A, 4);	// Unreachable IP address in UDP mode
	COM_DIAG_AMIGO_W5100_GP_16(UPORT,	0x002E);	// Unreachable Port address in UDP mode

public:

	/**
	 * Get the gateway (router) IP address from the W5100.
	 * @param buffer points to a buffer into which the IP address is stored.
	 */
	void getGatewayIp(ipv4address_t * buffer /* [IPV4ADDRESS] */) { readGAR(buffer); }

	/**
	 * Set the gateway (router) IP address in the W5100.
	 * @param data points to the array from which the IP address is loaded.
	 */
	void setGatewayIp(const ipv4address_t * data /* [IPV4ADDRESS] */) { writeGAR(data); }

	/**
	 * Get the IP subnet mask from the W5100.
	 * @param buffer points to a buffer into which the IP subnet mask is stored.
	 */
	void getSubnetMask(ipv4address_t * buffer /* [IPV4ADDRESS] */) { readSUBR(buffer); }

	/**
	 * Set the IP subnet mask in the W5100.
	 * @param data points to the array from which the IP subnet mask is loaded.
	 */
	void setSubnetMask(const ipv4address_t * data /* [IPV4ADDRESS] */) { writeSUBR(data); }

	/**
	 * Get the MAC address for this end point from the W5100.
	 * @param buffer points to a buffer into which the MAC address is stored.
	 */
	void getMACAddress(macaddress_t * buffer /* [MACADDRESS] */) { readSHAR(buffer); }

	/**
	 * Set the MAC address for this end point in the W5100.
	 * @param data points to the array from which the MAC address is loaded.
	 */
	void setMACAddress(const macaddress_t * data /* [MACADDRESS] */) { writeSHAR(data); }

	/**
	 * Get the IP address for this end point from the W5100.
	 * @param buffer points to a buffer into which the IP address is stored.
	 */
	void getIPAddress(ipv4address_t * buffer /* [IPV4ADDRESS] */) { readSIPR(buffer); }

	/**
	 * Set the IP address for this end point to the W5100.
	 * @param data points to the array from which the IP address is loaded.
	 */
	void setIPAddress(const ipv4address_t * data /* [IPV4ADDRESS] */) { writeSIPR(data); }

	/**
	 * Get the retransmission time parameter from the W5100.
	 * @return the retransmission time parameter.
	 */
	uint16_t getRetransmissionTime() { return readRTR(); }

	/**
	 * Set the retransmission time parameter in the W5100.
	 * @param timeout is the retransmission time parameter.
	 */
	void setRetransmissionTime(uint16_t timeout) { writeRTR(timeout); }

	/**
	 * Get the retransmission count parameter from the W5100.
	 * @return the retransmission count parameter.
	 */
	uint8_t getRetransmissionCount() { return readRCR(); }

	/**
	 * Set the retransmission count parameter in the W5100.
	 * @param retry is the retransmission count parameter.
	 */
	void setRetransmissionCount(uint8_t retry) { writeRCR(retry); }

	/***************************************************************************
	 * SOCKET REGISTER OPERATING
	 **************************************************************************/

private:

	uint8_t readSn(socket_t socket, address_t address) { return read(CH_BASE + (socket * CH_SIZE) + address); }

	void writeSn(socket_t socket, address_t address, uint8_t datum) { write(CH_BASE + (socket * CH_SIZE) + address, datum); }

	void readSn(socket_t socket, address_t address, void * buffer, size_t length) { read(CH_BASE + (socket * CH_SIZE) + address, buffer, length); }

	void writeSn(socket_t socket, address_t address, const void * data, size_t length) { write(CH_BASE + (socket * CH_SIZE) + address, data, length); }

protected:

#define COM_DIAG_AMIGO_W5100_SO_8(_NAME_, _ADDRESS_)			\
	void write##_NAME_(socket_t socket, uint8_t datum) {		\
		writeSn(socket, _ADDRESS_, datum);						\
	}															\
	uint8_t read##_NAME_(socket_t socket) {						\
		return readSn(socket, _ADDRESS_);						\
	}

#define COM_DIAG_AMIGO_W5100_SO_16(_NAME_, _ADDRESS_)			\
	void write##_NAME_(socket_t socket, uint16_t datum) {		\
		writeSn(socket, _ADDRESS_,     datum >> 8);				\
	    writeSn(socket, _ADDRESS_ + 1, datum & 0xFF);			\
	}															\
	uint16_t read##_NAME_(socket_t socket) {					\
	    uint16_t result = readSn(socket, _ADDRESS_);			\
	    result = (result << 8) + readSn(socket, _ADDRESS_ + 1);	\
	    return result;											\
	}

#define COM_DIAG_AMIGO_W5100_SO_N(_NAME_, _ADDRESS_, _SIZE_)	\
	  void write##_NAME_(socket_t socket, const void * data) {	\
		writeSn(socket, _ADDRESS_, data, _SIZE_);				\
	  }															\
	  void read##_NAME_(socket_t socket, void * buffer) {		\
		  readSn(socket, _ADDRESS_, buffer, _SIZE_);			\
	  }

	COM_DIAG_AMIGO_W5100_SO_8 (SnMR,		0x0000)		// Mode
	COM_DIAG_AMIGO_W5100_SO_8 (SnCR,		0x0001)		// Command
	COM_DIAG_AMIGO_W5100_SO_8 (SnIR,		0x0002)		// Interrupt
	COM_DIAG_AMIGO_W5100_SO_8 (SnSR,		0x0003)		// Status
	COM_DIAG_AMIGO_W5100_SO_16(SnPORT,		0x0004)		// Source Port
	COM_DIAG_AMIGO_W5100_SO_N (SnDHAR,		0x0006, 6)	// Destination MAC Address
	COM_DIAG_AMIGO_W5100_SO_N (SnDIPR,		0x000C, 4)	// Destination IP Address
	COM_DIAG_AMIGO_W5100_SO_16(SnDPORT,		0x0010)		// Destination Port
	COM_DIAG_AMIGO_W5100_SO_16(SnMSSR,		0x0012)		// Max Segment Size
	COM_DIAG_AMIGO_W5100_SO_8 (SnPROTO,		0x0014)		// Protocol in IP RAW Mode
	COM_DIAG_AMIGO_W5100_SO_8 (SnTOS,		0x0015)		// IP TOS
	COM_DIAG_AMIGO_W5100_SO_8 (SnTTL,		0x0016)		// IP TTL
	COM_DIAG_AMIGO_W5100_SO_16(SnTX_FSR,	0x0020)		// TX Free Size
	COM_DIAG_AMIGO_W5100_SO_16(SnTX_RD,		0x0022)		// TX Read Pointer
	COM_DIAG_AMIGO_W5100_SO_16(SnTX_WR,		0x0024)		// TX Write Pointer
	COM_DIAG_AMIGO_W5100_SO_16(SnRX_RSR,	0x0026)		// RX Free Size
	COM_DIAG_AMIGO_W5100_SO_16(SnRX_RD,		0x0028)		// RX Read Pointer
	COM_DIAG_AMIGO_W5100_SO_16(SnRX_WR,		0x002A)		// RX Write Pointer (supported?)

public:

	/**
	 * Return the state of the W5100 socket.
	 * @param socket identifies the socket.
	 * @return the state of the socket.
	 */
	uint8_t state(socket_t socket) { return readSnSR(socket); }

	/**
	 * Read data from the W5100 socket buffer.
	 * @param socket identifies the socket.
	 * @param address specifies the W5100 memory address at which to start.
	 * @param buffer points to a buffer in which to store data.
	 * @param length is the length of the buffer in bytes.
	 */
	void read_data(socket_t socket, address_t address, void * buffer, size_t length);

	/**
	 * Send data to a W5100 socket at zero bytes displacement.
	 * @param socket identifies the socket.
	 * @param data points to the data to be sent.
	 * @param length is the length of the data in bytes.
	 */
	void send_data_processing(socket_t socket, const void * data, size_t length) { send_data_processing_offset(socket, 0, data, length); }

	/**
	 * Send data to a W5100 socket at a specified byte displacement.
	 * @param socket identifies the socket.
	 * @param displacement specifies a byte offset.
	 * @param data points to the data to be sent.
	 * @param length is the length of the data in bytes.
	 */
	void send_data_processing_offset(socket_t socket, size_t displacement, const void * data, size_t length);

	/**
	 * Receive data from a W5100 socket.
	 * @param socket identifies the socket.
	 * @param buffer points to a buffer into which the data is stored.
	 * @param length is the length of the buffer in bytes.
	 * @param peek is true indicates not to consume the data from the W5100.
	 */
	void recv_data_processing(socket_t socket, void * buffer, size_t length, bool peek = false);

	/**
	 * Execute a W5100 socket command.
	 * @param socket identifies the socket.
	 * @param command is the socket command.
	 */
	void execCmdSn(socket_t socket, SockCMD command);

	/**
	 * Get the number of free bytes in a W5100 socket buffer.
	 * @param socket identifies the socket.
	 * @return the number of free bytes.
	 */
	size_t getTXFreeSize(socket_t socket);

	/**
	 * Get the number of received bytes in a W5100 socket buffer.
	 * @param socket identifies the socket.
	 * @return the number of received bytes.
	 */
	size_t getRXReceivedSize(socket_t socket);

	/***************************************************************************
	 * ANCILLARY STUFF
	 **************************************************************************/

protected:

	MutexSemaphore * mutex;
	SPI * spi;
	GPIO gpio;
	uint8_t mask;
	address_t sbase[SOCKETS]; // Tx buffer base address
	address_t rbase[SOCKETS]; // Rx buffer base address

	void reset(ticks_t resetting);

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	W5100(const W5100 & that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	W5100 & operator=(const W5100& that);

};

inline W5100::W5100(MutexSemaphore & mymutex, GPIO::Pin myss, SPI & myspi)
: mutex(&mymutex)
, spi(&myspi)
, gpio(GPIO::gpio2base(myss))
, mask(GPIO::gpio2mask(myss))
{
	initialize();
}

inline W5100::W5100(GPIO::Pin myss, SPI & myspi)
: mutex(0)
, spi(&myspi)
, gpio(GPIO::gpio2base(myss))
, mask(GPIO::gpio2mask(myss))
{
	initialize();
}

}
}
}
}

#endif /* _COM_DIAG_AMIGO_W5100_W5100_H_ */
