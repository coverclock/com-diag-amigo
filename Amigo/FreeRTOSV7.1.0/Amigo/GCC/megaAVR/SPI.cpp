/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by FreeRTOS lib_spi and Arduino SPI.
 */

#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "task.h"
#include "com/diag/amigo/target/SPI.h"
#include "com/diag/amigo/target/Uninterruptable.h"
#include "com/diag/amigo/io.h"

namespace com {
namespace diag {
namespace amigo {

// These are our memory mapped I/O register addresses expressed as displacements
// from the base address identifying the specific SPI. This makes this code
// independent of the specific SPI. (I don't know of an megaAVR that has more
// than one SPI controller, but I'm trying to follow the pattern set by
// Serial.)

#define SPICR		AMIGO_MMIO_8(spibase, 0)
#define SPISR		AMIGO_MMIO_8(spibase, 1)
#define SPIDR		AMIGO_MMIO_8(spibase, 2)

#define PIN			AMIGO_MMIO_8(gpiobase, 0)
#define DDR			AMIGO_MMIO_8(gpiobase, 1)
#define PORT		AMIGO_MMIO_8(gpiobase, 2)

/**
 * This table in SRAM will be filled in with the this pointer from a specific
 * SPI object when it is instantiated. Only one SPI object per controller should
 * be instantiated; otherwise it will overwrite the this pointer of any previous
 * SPI object for that same port.
 */
SPI * SPI::spi[] = {
	0
};

SPI::SPI(Controller mycontroller, Count transmits, Count receives)
: spibase(0)
, gpiobase(0)
, received(receives)
, transmitting(transmits)
, controller(mycontroller)
, ss(0)
, sck(0)
, mosi(0)
, miso(0)
, errors(0)
{
	spi[controller] = this;

	switch (controller) {
	default:
	case SPI0:
		spibase = &SPCR;
#if defined(__AVR_ATmega2560__)
		gpiobase = &PINB;
		ss = _BV(0);
		sck = _BV(1);
		mosi = _BV(2);
		miso = _BV(3);
#elif defined(__AVR_ATmega328P__)
		gpiobase = &PINB;
		ss = _BV(2);
		sck = _BV(5);
		mosi = _BV(3);
		miso = _BV(4);
#else
#	error SPI has to be configured for this microcontroller!
#endif
		break;
	}
}

SPI::~SPI() {
	Uninterruptable uninterruptable;
	SPICR &= ~(_BV(SPIE) | _BV(SPE));
	spi[controller] = 0;
}

void SPI::start(Divisor divisor, Role role, Order order, Polarity polarity, Phase phase) {
	uint8_t spi2x;
	switch (divisor) {
	default:
	case D4:
	case D16:
	case D64:
	case D128:
		spi2x = 0;
		break;
	case D2:
	case D8:
	case D32:
		spi2x = _BV(SPI2X);
		break;
	}

	uint8_t spr1;
	switch (divisor) {
	default:
	case D2:
	case D4:
	case D8:
	case D16:
		spr1 = 0;
		break;
	case D32:
	case D64:
	case D128:
		spr1 = _BV(SPR1);
		break;
	}

	uint8_t spr0;
	switch (divisor) {
	default:
	case D2:
	case D4:
	case D32:
	case D64:
		spr0 = 0;
		break;
	case D8:
	case D16:
	case D128:
		spr0 = _BV(SPR0);
		break;
	}

	uint8_t dord;
	switch (order) {
	default:
	case MSB:
		dord = 0;
		break;
	case LSB:
		dord = _BV(DORD);
		break;
	}

	uint8_t cpol;
	switch (polarity) {
	default:
	case NORMAL:
		cpol = 0;
		break;
	case INVERTED:
		cpol = _BV(CPOL);
		break;
	}

	uint8_t cpha;
	switch (phase) {
	default:
	case LEADING:
		cpha = 0;
		break;
	case TRAILING:
		cpha = _BV(CPHA);
		break;
	}

	Uninterruptable uninterruptable;

	uint8_t mstr;
	switch (role) {
	default:
	case MASTER:
		DDR &= ~miso;
		DDR |= (sck | mosi | ss);
		PORT &= ~(sck | mosi);
		PORT |= ss;
		mstr = _BV(MSTR);
		break;
	case SLAVE:
		DDR &= ~(ss | sck | mosi);
		DDR |= miso;
		mstr = 0;
		break;
	}

	SPICR = dord | mstr | cpol | cpha | spr1 | spr0;

	SPISR |= spi2x;

	SPICR |= _BV(SPIE) | _BV(SPE);

	uint8_t ch;
	if (transmitting.receive(&ch, IMMEDIATELY)) {
		SPIDR = ch;
	}
}

void SPI::stop() const {
	Uninterruptable uninterruptable;
	SPICR &= ~(_BV(SPIE) | _BV(SPE));
}

void SPI::activate() {
	Uninterruptable uninterruptable;
	uint8_t ch;
	if (transmitting.receive(&ch, IMMEDIATELY)) {
		SPIDR = ch;
	}
}

void SPI::restart() {
	Uninterruptable uninterruptable;
	SPICR |= _BV(SPIE) | _BV(SPE);
	uint8_t ch;
	if (transmitting.receive(&ch, IMMEDIATELY)) {
		SPIDR = ch;
	}
}

SPI & SPI::operator=(uint8_t value) {
	// It is fun to think about why this has to be uninterruptable.
	Uninterruptable uninterruptable;
	errors = value;
	return *this;
}

inline void SPI::complete(Controller controller) {
	// Only called from an ISR hence implicitly uninterruptable.
	if (spi[controller] != 0) {
		spi[controller]->complete();
	}
}

void SPI::complete() {
	// Only called from an ISR hence implicitly uninterruptable.
	if ((SPISR & _BV(WCOL)) == 0) {
		// Do nothing.
	} else if (errors < ~static_cast<uint8_t>(0)) {
		++errors;
	} else {
		// Do nothing.
	}
	uint8_t ch = SPIDR;
	bool woken = false;
	if (received.sendFromISR(&ch, woken)) {
		// Do nothing.
	} else if (errors < ~static_cast<uint8_t>(0)) {
		++errors;
	} else {
		// Do nothing.
	}
	if (transmitting.receiveFromISR(&ch)) {
		SPIDR = ch;
	}
	if (woken) {
		// Doing a context switch from inside an ISR seems wrong, both from an
		// architectural POV and a correctness POV. But that's what happens in
		// other FreeRTOS code, and I understand the rationale for it. Careful
		// perusal of the underlying code has _mostly_ convinced me that this
		// works as intended. Interrupts will be re-enabled in the next task
		// when its SREG is restored from the context frame on its stack. They
		// will be reenabled in the new task when it is returned from this ISR.
		// It still seems like a violation against the laws of Man and God.
		taskYIELD();
	}
}

}
}
}

// These are the ISR routines which are jumped to from an ISR vector in low
// memory. Note that the class methods are inlined, so there is really only
// two levels of indirection: a jump from the vector to this routine, then an
// inline class method call (which doesn't count because it's inline), then
// a call to the instance method that actually implements the ISR for a
// specific instance of the SPI object. Note that these functions have C
// linkage.

extern "C" {

#if defined(SPI_STC_vect)
ISR(SPI_STC_vect) {
	com::diag::amigo::SPI::complete(com::diag::amigo::SPI::SPI0);
}
#endif

}
