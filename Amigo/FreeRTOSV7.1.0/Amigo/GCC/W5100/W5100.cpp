/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by and cribbed from w5100.h and w5100.cpp by
 * Cristian Maglie from Arduino 1.0. It is specific to the WIZnet W5100 chip
 * as used on the Arduino Ethernet shield and the FreeTronics EtherMega 2560.
 * It may work on other compatibles and clones. Your mileage may vary. It is
 * also based on the W5100 unit test stubs in the Amigo unit test suite.
 */

#include "com/diag/amigo/W5100/W5100.h"
#include "com/diag/amigo/CriticalSection.h"
#include "com/diag/amigo/Toggle.h"
#include "com/diag/amigo/Task.h"
#include "com/diag/amigo/countof.h"

namespace com {
namespace diag {
namespace amigo {
namespace W5100 {

/*******************************************************************************
 * CREATION AND DESTRUCTION
 ******************************************************************************/

void W5100::initialize() {
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

void W5100::reset(ticks_t resetting) {
	writeMR(1 << RST);
	if (resetting > 0) {
		Task::delay(resetting);
	}
}

void W5100::start(ticks_t resetting) {
	gpio.output(mask, mask); // Active low hence initially high.
	reset(resetting);
	writeTMSR(0x55); // 2KB to each of four sockets for transmitting.
	writeRMSR(0x55); // 2KB to each of four sockets for receiving.
}

void W5100::stop(ticks_t resetting) {
	reset(resetting);
}

/*******************************************************************************
 * SPI ACTIONS
 ******************************************************************************/

void W5100::write(address_t address, uint8_t datum) {
	CriticalSection cs(mutex);
	ToggleOff ss(gpio, mask);
	spi->master(0xf0);
	spi->master(address >> 8);
	spi->master(address & 0xff);
	spi->master(datum);
}

void W5100::write(address_t address, const void * data, size_t length) {
	CriticalSection cs(mutex);
	const uint8_t * here = static_cast<const uint8_t *>(data);
	for (; length > 0; --length) {
		ToggleOff ss(gpio, mask);
		spi->master(0xf0);
		spi->master(address >> 8);
		spi->master((address++) & 0xff);
		spi->master(*(here++));
	}
}

uint8_t W5100::read(address_t address) {
	CriticalSection cs(mutex);
	ToggleOff ss(gpio, mask);
	spi->master(0x0f);
	spi->master(address >> 8);
	spi->master(address & 0xff);
	uint8_t datum = spi->master();
	return datum;
}

void W5100::read(address_t address, void * buffer, size_t length) {
	CriticalSection cs(mutex);
	uint8_t * here = static_cast<uint8_t *>(buffer);
	for (; length > 0; --length) {
		ToggleOff ss(gpio, mask);
		spi->master(0x0f);
		spi->master(address >> 8);
		spi->master((address++) & 0xff);
		*(here++) = spi->master();
  }
}

/***************************************************************************
 * SOCKET ACTIONS
 **************************************************************************/

void W5100::execCmdSn(socket_t socket, SockCMD command) {
	// Send command to socket
	writeSnCR(socket, command);
	// Wait for command to complete
	while (readSnCR(socket))
		;
}

size_t W5100::getTXFreeSize(socket_t socket)
{
	uint16_t val = 0;
	uint16_t val1 = 0;

	do {
		val1 = readSnTX_FSR(socket);
		if (val1 != 0) {
			val = readSnTX_FSR(socket);
		}
	} while (val != val1);

	return val;
}

size_t W5100::getRXReceivedSize(socket_t socket)
{
	uint16_t val = 0;
	uint16_t val1 = 0;

	do {
		val1 = readSnRX_RSR(socket);
		if (val1 != 0) {
			val = readSnRX_RSR(socket);
		}
	} while (val != val1);

	return val;
}

void W5100::send_data_processing_offset(socket_t socket, size_t displacement, const void * data, size_t length)
{
	const uint8_t * here = static_cast<const uint8_t *>(data);
	address_t address = readSnTX_WR(socket) + displacement;
	size_t offset = address & SMASK;
	address_t pointer = sbase[socket] + offset;
	size_t size;

	if ((offset + length) > SSIZE) {
		// Wrap around circular buffer.
		size = SSIZE - offset;
		write(pointer, here, size);
		write(sbase[socket], here + size, length - size);
	} else {
		write(pointer, here, length);
	}

	writeSnTX_WR(socket, address + length);
}


void W5100::recv_data_processing(socket_t socket, void * buffer, size_t length, bool peek)
{
	address_t address = readSnRX_RD(socket);
	read_data(socket, address, buffer, length);
	if (!peek) {
		writeSnRX_RD(socket, address + length);
	}
}

void W5100::read_data(socket_t socket, address_t address, void * buffer, size_t length)
{
	uint8_t * here = static_cast<uint8_t *>(buffer);
	size_t offset = address & RMASK;
	address_t pointer = rbase[socket] + offset;
	size_t size;

	if ((offset + length) > RSIZE) {
		// Wrap around circular buffer.
		size = RSIZE - offset;
		read(pointer, here, size);
		read(rbase[socket], here + size, length - size);
	} else {
		read(pointer, here, length);
	}
}

}
}
}
}
