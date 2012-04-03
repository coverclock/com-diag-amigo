/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by <util/baud.h>, FreeRTOS lib_serial.
 * and <http://feilipu.posterous.com/ethermega-arduino-mega-2560-and-freertos>.
 * A special THANK YOU goes to Richard Barry and Phillip Stevens.
 */

#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "task.h"
#include "com/diag/amigo/target/Serial.h"
#include "com/diag/amigo/target/Uninterruptable.h"
#include "com/diag/amigo/io.h"

namespace com {
namespace diag {
namespace amigo {

// These are our memory mapped I/O register addresses expressed as displacements
// from the base address identifying the specific USART. This makes this code
// independent of the specific USART.

#define UCSRA		AMIGO_MMIO_8(base, 0)
#define UCSRB		AMIGO_MMIO_8(base, 1)
#define UCSRC		AMIGO_MMIO_8(base, 2)
//      RESERVED	AMIGO_MMIO_8(base, 3)
#define UBRRL		AMIGO_MMIO_8(base, 4)
#define UBRRH		AMIGO_MMIO_8(base, 5)
#define UDR			AMIGO_MMIO_8(base, 6)

/**
 * This table in SRAM will be filled in with the this pointer from a specific
 * Serial object when it is instantiated. Only one Serial object per port
 * should be instantiated; otherwise it will overwrite the this pointer of any
 * previous Serial object for that same port.
 */
Serial * Serial::serial[] = {
	0,
#if defined(UCSR1A)
	0,
#if defined(UCSR2A)
	0,
#if defined(UCSR3A)
	0,
#endif
#endif
#endif
};

Serial::Serial(Port myport, Count transmits, Count receives, uint8_t mybad)
: base(0)
, received(receives)
, transmitting(transmits)
, port(myport)
, bad(mybad)
, errors(0)
{
	serial[port] = this;

	// Important safety tip: placing the code below in a static function
	// that returned a volatile pointer returned NULL instead. Putting some
	// debugging code in that function to display the pointer caused the
	// function to return the correct value. Not good. Seems like yet another
	// compiler bug in dealing with volatiles. This occurred when building on
	// my Mac using GCC 4.5.1 and AVR libc 1.8.0. It did NOT occur when building
	// on Windows using AVR Studio 5.1 with GCC 4.5.1 and AVR libc 1.7.1.

	switch (port) {
	default:
	case USART0:	base = &UCSR0A;	break;
#if defined(UCSR1A)
	case USART1:	base = &UCSR1A;	break;
#if defined(UCSR2A)
	case USART2:	base = &UCSR2A;	break;
#if defined(UCSR3A)
	case USART3:	base = &UCSR3A;	break;
#endif
#endif
#endif
	}
}

Serial::~Serial() {
	stop();
	serial[port] = 0;
}

void Serial::start(Baud baud, Data data, Parity parity, Stop stop) const {
	uint32_t rate;
	switch (baud) {
	case B50:		rate = 50UL;		break;
	case B75:		rate = 75UL;		break;
	case B110:		rate = 110UL;		break;
	case B134:		rate = 134UL;		break;
	case B150:		rate = 150UL;		break;
	case B200:		rate = 200UL;		break;
	case B300:		rate = 300UL;		break;
	case B600:		rate = 600UL;		break;
	case B1200:		rate = 1200UL;		break;
	case B1800:		rate = 1800UL;		break;
	case B2400:		rate = 2400UL;		break;
	case B4800:		rate = 4800UL;		break;
	case B9600:		rate = 9600UL;		break;
	case B19200:	rate = 19200UL;		break;
	case B38400:	rate = 38400UL;		break;
	case B57600:	rate = 57600UL;		break;
	default:
	case B115200:	rate = 115200UL;	break;
	}

	start(rate, data, parity, stop);
}

// This form of the start() function exists only to support Arduino, whose
// HardwareSerial interface expects to be able to specify the baud rate as an
// unsigned long in its begin() function. Otherwise it's definitely safer to
// use other form of start() whose Baud enumeration keeps you from coding a
// baud rate that is not supported. However, the counter calculation quantizes
// the result so that even incorrect rates will probably yield something useful.
void Serial::start(uint32_t rate, Data data, Parity parity, Stop stop) const {
	uint16_t counter = (((configCPU_CLOCK_HZ) + (4UL * rate)) / (8UL * rate)) - 1UL;

	uint8_t databits;
	switch (data) {
	case FIVE:	databits = 0;							break;
	case SIX:	databits = _BV(UCSZ00);					break;
	case SEVEN:	databits = _BV(UCSZ01);					break;
	default:
	case EIGHT:	databits = _BV(UCSZ01) | _BV(UCSZ00);	break;
	}

	uint8_t paritybits;
	switch (parity) {
	default:
	case NONE:	paritybits = 0;							break;
	case EVEN:	paritybits = _BV(UPM01);				break;
	case ODD:	paritybits = _BV(UPM01) | _BV(UPM00);	break;
	}

	uint8_t stopbits;
	switch (stop) {
	default:
	case ONE:	stopbits = 0;			break;
	case TWO:	stopbits = _BV(USBS0);	break;
	}

	Uninterruptable uninterruptable;

	UCSRB = 0;

	UBRRL = counter & 0xff;
	UBRRH = (counter >> 8) & 0xff;

	UCSRA = _BV(U2X0);

	UCSRC = databits | paritybits | stopbits;

	UCSRB = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
}

void Serial::stop() const {
	Uninterruptable uninterruptable;
	UCSRB = 0;
}

void Serial::enable() const {
	Uninterruptable uninterruptable;
	UCSRB |= _BV(UDRIE0);
}

void Serial::restart() const {
	Uninterruptable uninterruptable;
	UCSRB = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
	if (transmitting.available() > 0) {
		UCSRB |= _BV(UDRIE0);
	}
}

void Serial::flush() const {
	while (transmitting.available() > 0) {
		taskYIELD();
	}
}

Serial & Serial::operator=(uint8_t value) {
	// It is fun to think about why this has to be uninterruptable.
	Uninterruptable uninterruptable;
	errors = value;
	return *this;
}

inline void Serial::receive(Port port) {
	// Only called from an ISR hence implicitly uninterrutable;
	if (serial[port] != 0) {
		serial[port]->receive();
	}
}

void Serial::receive() {
	// Only called from an ISR hence implicitly uninterrutable;
	uint8_t ch;
	if ((UCSRA & (_BV(FE0) | _BV(DOR0) | _BV(UPE0))) == 0) {
		ch = UDR;
	} else {
		ch = bad;
		if (errors < ~static_cast<uint8_t>(0)) {
			++errors;
		}
	}
	bool woken = false;
	received.sendFromISR(&ch, woken);
	// Doing a context switch from inside an ISR seems wrong, both from an
	// architectural POV and a correctness POV. But that's what happens in
	// other FreeRTOS code, and I understand the rationale for it. Careful
	// perusal of the underlying code has _mostly_ convinced me that this
	// works as intended. Interrupts will be re-enabled in the next task when
	// its SREG is restored from the context frame on its stack. They will be
	// reenabled in the new task when it is returned from this ISR. It still
	// seems like a violation against the laws of Man and God.
	if (woken) {
		taskYIELD();
	}
}

inline void Serial::transmit(Port port) {
	// Only called from an ISR hence implicitly uninterrutable;
	if (serial[port] != 0) {
		serial[port]->transmit();
	}
}

void Serial::transmit() {
	// Only called from an ISR hence implicitly uninterrutable;
	uint8_t ch;
	if (transmitting.receiveFromISR(&ch)) {
		UDR = ch;
	} else {
		UCSRB &= ~_BV(UDRIE0);
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
// specific instance of the Serial object. Note that these functions have C
// linkage.

extern "C" {

#if defined(USART_RX_vect)
ISR(USART_RX_vect) {
	com::diag::amigo::Serial::receive(com::diag::amigo::Serial::USART0);
}
#endif

#if defined(USART0_RX_vect)
ISR(USART0_RX_vect) {
	com::diag::amigo::Serial::receive(com::diag::amigo::Serial::USART0);
}
#endif

#if defined(USART1_RX_vect)
ISR(USART1_RX_vect) {
	com::diag::amigo::Serial::receive(com::diag::amigo::Serial::USART1);
}
#endif

#if defined(USART2_RX_vect)
ISR(USART2_RX_vect) {
	com::diag::amigo::Serial::receive(com::diag::amigo::Serial::USART2);
}
#endif

#if defined(USART3_RX_vect)
ISR(USART3_RX_vect) {
	com::diag::amigo::Serial::receive(com::diag::amigo::Serial::USART3);
}
#endif

#if defined(USART_UDRE_vect)
ISR(USART_UDRE_vect) {
	com::diag::amigo::Serial::transmit(com::diag::amigo::Serial::USART0);
}
#endif

#if defined(USART0_UDRE_vect)
ISR(USART0_UDRE_vect) {
	com::diag::amigo::Serial::transmit(com::diag::amigo::Serial::USART0);
}
#endif

#if defined(USART1_UDRE_vect)
ISR(USART1_UDRE_vect) {
	com::diag::amigo::Serial::transmit(com::diag::amigo::Serial::USART1);
}
#endif

#if defined(USART2_UDRE_vect)
ISR(USART2_UDRE_vect) {
	com::diag::amigo::Serial::transmit(com::diag::amigo::Serial::USART2);
}
#endif

#if defined(USART3_UDRE_vect)
ISR(USART3_UDRE_vect) {
	com::diag::amigo::Serial::transmit(com::diag::amigo::Serial::USART3);
}
#endif

}