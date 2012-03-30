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

#define UCSRA		AMIGO_MMIO_8(base, 0)
#define UCSRB		AMIGO_MMIO_8(base, 1)
#define UCSRC		AMIGO_MMIO_8(base, 2)
//      RESERVED	AMIGO_MMIO_8(base, 3)
#define UBRRL		AMIGO_MMIO_8(base, 4)
#define UBRRH		AMIGO_MMIO_8(base, 5)
#define UDR			AMIGO_MMIO_8(base, 6)

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
: port(myport)
, base(0)
, received(receives)
, transmitting(transmits)
, bad(mybad)
{
	serial[port] = this;

	// Important safety tip: placing the code below in a static map function
	// that returned a volatile pointer returned NULL instead. Putting some
	// debugging code in that function to display the pointer caused the
	// function to return the correct value. Not good.

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

void Serial::disable() const {
	Uninterruptable uninterruptable;
	UCSRB &= ~_BV(UDRIE0);
}

void Serial::yield() const {
	taskYIELD();
}

size_t Serial::emit(uint8_t ch) const {
	Uninterruptable uninterrutable;
	uint8_t ucsrb = UCSRB;
	UCSRB = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
	while ((UCSRA & _BV(UDRE0)) == 0) {
		// Do nothing.
	}
	UDR = ch;
	UCSRB = ucsrb;
	return 1;
}

inline void Serial::receive(Port port) {
	if (serial[port] != 0) {
		serial[port]->receive();
	}
}

void Serial::receive() {
	// Only called from an ISR hence implicitly uninterrutable;
	uint8_t ch = ((UCSRA & (_BV(FE0) | _BV(DOR0) | _BV(UPE0))) == 0) ? UDR : bad;
	bool woken = false;
	received.sendFromISR(&ch, woken);
#if 0
	if (woken) {
		yield();
	}
#endif
}

inline void Serial::transmit(Port port) {
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
		disable();
	}
}

}
}
}

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
