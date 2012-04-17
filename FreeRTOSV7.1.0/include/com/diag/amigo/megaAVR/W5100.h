#ifndef _COM_DIAG_MEGAAVR_W5100_H_
#define _COM_DIAG_MEGAAVR_W5100_H_

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

#include "com/diag/amigo/MutexSemaphore.h"
#include "com/diag/amigo/target/GPIO.h"
#include "com/diag/amigo/target/SPI.h"
#include "com/diag/amigo/types.h"

namespace com {
namespace diag {
namespace amigo {

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

	/***************************************************************************
	 * CREATION AND DESTRUCTION
	 **************************************************************************/

public:

	explicit W5100(MutexSemaphore & mymutex, GPIO::Pin myss, SPI & myspi);

	virtual ~W5100();

	/***************************************************************************
	 * TYPES AND SIZES
	 **************************************************************************/

public:

	typedef uint16_t Address;

	typedef uint16_t Size;

	typedef uint8_t Socket;

	typedef uint16_t Port;

	static const size_t SOCKETS = 4;

	static const size_t MACADDRESS = 6;

	static const size_t IPV4ADDRESS = 4;

	static const size_t SSIZE = 2048; // Max Tx buffer size

	static const size_t RSIZE = 2048; // Max Rx buffer size

	/***************************************************************************
	 * STARTING AND STOPPING
	 **************************************************************************/

public:

	void start();

	void stop();

	/***************************************************************************
	 * DEFINITIONS
	 **************************************************************************/

public:

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

	static const uint16_t SMASK = 0x07FF; // Tx buffer MASK

	static const uint16_t RMASK = 0x07FF; // Rx buffer MASK

	static const uint16_t TX_RX_MAX_BUF_SIZE = 2048;

	static const uint16_t TX_BUF = 0x1100;

	static const uint16_t RX_BUF = (TX_BUF + TX_RX_MAX_BUF_SIZE);

	static const uint16_t TXBUF_BASE = 0x4000;

	static const uint16_t RXBUF_BASE = 0x6000;

	static const uint16_t CH_BASE = 0x0400;

	static const uint16_t CH_SIZE = 0x0100;

	/***************************************************************************
	 * GENERAL PURPOSE REGISTER ACTIONS
	 **************************************************************************/

private:

	void write(Address address, uint8_t datum);

	void write(Address address, const void * data, size_t length);

	uint8_t read(Address address);

	void read(Address address, void * buffer, size_t length);

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

	void getGatewayIp(uint8_t * buffer /* [IPV4ADDRESS] */) { readGAR(buffer); }

	void setGatewayIp(const uint8_t * data /* [IPV4ADDRESS] */) { writeGAR(data); }

	void getSubnetMask(uint8_t * buffer /* [IPV4ADDRESS] */) { readSUBR(buffer); }

	void setSubnetMask(const uint8_t * data /* [IPV4ADDRESS] */) { writeSUBR(data); }

	void getMACAddress(uint8_t * buffer /* [MACADDRESS] */) { readSHAR(buffer); }

	void setMACAddress(const uint8_t * data /* [MACADDRESS] */) { writeSHAR(data); }

	void getIPAddress(uint8_t * buffer /* [IPV4ADDRESS] */) { readSIPR(buffer); }

	void setIPAddress(const uint8_t * data /* [IPV4ADDRESS] */) { writeSIPR(data); }

	void setRetransmissionTime(uint16_t timeout) { writeRTR(timeout); }

	void setRetransmissionCount(uint8_t retry) { writeRCR(retry); }

	/***************************************************************************
	 * SOCKET REGISTER ACTIONS
	 **************************************************************************/

private:

	uint8_t readSn(Socket socket, Address address) { return read(CH_BASE + (socket * CH_SIZE) + address); }

	void writeSn(Socket socket, Address address, uint8_t datum) { write(CH_BASE + (socket * CH_SIZE) + address, datum); }

	void readSn(Socket socket, Address address, void * buffer, size_t length) { read(CH_BASE + (socket * CH_SIZE) + address, buffer, length); }

	void writeSn(Socket socket, Address address, const void * data, size_t length) { write(CH_BASE + (socket * CH_SIZE) + address, data, length); }

public:

#define COM_DIAG_AMIGO_W5100_SO_8(_NAME_, _ADDRESS_)			\
	void write##_NAME_(Socket socket, uint8_t datum) {			\
		writeSn(socket, _ADDRESS_, datum);						\
	}															\
	uint8_t read##_NAME_(Socket socket) {						\
		return readSn(socket, _ADDRESS_);						\
	}

#define COM_DIAG_AMIGO_W5100_SO_16(_NAME_, _ADDRESS_)			\
	void write##_NAME_(Socket socket, uint16_t datum) {			\
		writeSn(socket, _ADDRESS_,     datum >> 8);				\
	    writeSn(socket, _ADDRESS_ + 1, datum & 0xFF);			\
	}															\
	uint16_t read##_NAME_(Socket socket) {						\
	    uint16_t result = readSn(socket, _ADDRESS_);			\
	    result = (result << 8) + readSn(socket, _ADDRESS_ + 1);	\
	    return result;											\
	}

#define COM_DIAG_AMIGO_W5100_SO_N(_NAME_, _ADDRESS_, _SIZE_)	\
	  void write##_NAME_(Socket socket, void * data) {			\
		writeSn(socket, _ADDRESS_, data, _SIZE_);				\
	  }															\
	  void read##_NAME_(Socket socket, void * buffer) {			\
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

	uint8_t status(Socket socket) { return readSnSR(socket); }

	void read_data(Socket socket, Address address, void * buffer, size_t length);

	void send_data_processing(Socket socket, const void * data, size_t length) { send_data_processing_offset(socket, 0, data, length); }

	void send_data_processing_offset(Socket socket, Size data_offset, const void * data, size_t length);

	void recv_data_processing(Socket socket, void * buffer, size_t length, bool peek = false);

	void execCmdSn(Socket socket, SockCMD command);

	Size getTXFreeSize(Socket socket);

	Size getRXReceivedSize(Socket socket);

	/***************************************************************************
	 * ANCILLARY STUFF
	 **************************************************************************/

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

protected:

	MutexSemaphore * mutex;
	SPI * spi;
	GPIO gpio;
	uint8_t mask;
	Address sbase[SOCKETS]; // Tx buffer base address
	Address rbase[SOCKETS]; // Rx buffer base address

};

}
}
}

#endif /* _COM_DIAG_MEGAAVR_W5100_H_ */
