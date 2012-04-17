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

#include "com/diag/amigo/W5100/W5100.h"
#include "com/diag/amigo/CriticalSection.h"
#include "com/diag/amigo/SPISlaveSelect.h"

namespace com {
namespace diag {
namespace amigo {

/*******************************************************************************
 * CREATION AND DESTRUCTION
 ******************************************************************************/

W5100::W5100(MutexSemaphore & mymutex, GPIO::Pin myss, SPI & myspi)
: mutex(&mymutex)
, spi(&myspi)
, gpio(GPIO::base(myss))
, mask(GPIO::mask(myss))
{
	for (uint8_t ii = 0; ii < SOCKETS; ++ii) {
		sbase[ii] = TXBUF_BASE + (SSIZE * ii);
		rbase[ii] = RXBUF_BASE + (RSIZE * ii);
	}
}

W5100::~W5100() {
}

/*******************************************************************************
 * STARTING AND STOPPING
 ******************************************************************************/

void W5100::start() {
	gpio.output(mask, mask); // Active low hence initially high.
	writeMR(1 << RST);
	writeTMSR(0x55);
	writeRMSR(0x55);
}

void W5100::stop() {
}

/*******************************************************************************
 * SPI ACTIONS
 ******************************************************************************/

void W5100::write(Address address, uint8_t datum) {
	CriticalSection cs(*mutex);
	SPISlaveSelect ss(gpio, mask);
	spi->master(0xf0);
	spi->master(address >> 8);
	spi->master(address & 0xff);
	spi->master(datum);
}

void W5100::write(Address address, const void * data, size_t length) {
	CriticalSection cs(*mutex);
	const uint8_t * here = static_cast<const uint8_t *>(data);
	for (; length > 0; --length) {
		SPISlaveSelect ss(gpio, mask);
		spi->master(0xf0);
		spi->master(address >> 8);
		spi->master((address++) & 0xff);
		spi->master(*(here++));
	}
}

uint8_t W5100::read(Address address) {
	CriticalSection cs(*mutex);
	SPISlaveSelect ss(gpio, mask);
	spi->master(0x0f);
	spi->master(address >> 8);
	spi->master(address & 0xff);
	uint8_t datum = spi->master();
	return datum;
}

void W5100::read(Address address, void * buffer, size_t length) {
	CriticalSection cs(*mutex);
	uint8_t * here = static_cast<uint8_t *>(buffer);
	for (; length > 0; --length) {
		SPISlaveSelect ss(gpio, mask);
		spi->master(0x0f);
		spi->master(address >> 8);
		spi->master((address++) & 0xff);
		*(here++) = spi->master();
  }
}

/***************************************************************************
 * SOCKET ACTIONS
 **************************************************************************/

void W5100::execCmdSn(Socket socket, SockCMD command) {
	// Send command to socket
	writeSnCR(socket, command);
	// Wait for command to complete
	while (readSnCR(socket))
		;
}

Size W5100::getTXFreeSize(Socket socket)
{
	Size val = 0;
	Size val1 = 0;

	do {
		val1 = readSnTX_FSR(socket);
		if (val1 != 0) {
			val = readSnTX_FSR(socket);
		}
	} while (val != val1);

	return val;
}

Size W5100::getRXReceivedSize(Socket socket)
{
	Size val = 0;
	Size val1 = 0;

	do {
		val1 = readSnRX_RSR(socket);
		if (val1 != 0) {
			val = readSnRX_RSR(socket);
		}
	} while (val != val1);

	return val;
}

void W5100::send_data_processing_offset(Socket socket, Size data_offset, const void * data, size_t length)
{
	const uint8_t * here = static_cast<const uint8_t *>(data);
	Address ptr = readSnTX_WR(socket);
	ptr += data_offset;
	Size offset = ptr & SMASK;
	Address dstAddr = offset + sbase[socket];

	if (offset + length > SSIZE) {
		// Wrap around circular buffer
		Size size = SSIZE - offset;
		write(dstAddr, here, size);
		write(sbase[socket], here + size, length - size);
	} else {
		write(dstAddr, here, length);
	}

	ptr += length;
	writeSnTX_WR(socket, ptr);
}


void W5100::recv_data_processing(Socket socket, void * buffer, size_t length, bool peek)
{
	Address ptr;
	ptr = readSnRX_RD(socket);
	read_data(socket, ptr, buffer, length);
	if (!peek) {
		ptr += length;
		writeSnRX_RD(socket, ptr);
	}
}

void W5100::read_data(Socket socket, Address address, void * buffer, size_t length)
{
	uint8_t * here = static_cast<uint8_t *>(buffer);
	Size size;
	Address src_mask;
	Address src_ptr;

	src_mask = address & RMASK;
	src_ptr = rbase[socket] + src_mask;

	if ((src_mask + length) > RSIZE) {
		size = RSIZE - src_mask;
		read(src_ptr, here, size);
		here += size;
		read(rbase[socket], here, length - size);
	} else {
		read(src_ptr, here, length);
	}
}

}
}
}
