/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by <util/baud.h>, FreeRTOS lib_serial.c,
 * and <http://feilipu.posterous.com/ethermega-arduino-mega-2560-and-freertos>.
 * A special THANK YOU goes to Richard Barry and Phillip Stevens.
 */

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "com/diag/amigo/Serial.h"
#include "com/diag/amigo/Uninterruptable.h"
#include "com/diag/amigo/io.h"

namespace com {
namespace diag {
namespace amigo {

#define UCSRA		COM_DIAG_AMIGO_MMIO_8(base, 0)
#define UCSRB		COM_DIAG_AMIGO_MMIO_8(base, 1)
#define UCSRC		COM_DIAG_AMIGO_MMIO_8(base, 2)
//      RESERVED	COM_DIAG_AMIGO_MMIO_8(base, 3)
#define UBRRL		COM_DIAG_AMIGO_MMIO_8(base, 4)
#define UBRRH		COM_DIAG_AMIGO_MMIO_8(base, 5)
#define UDR			COM_DIAG_AMIGO_MMIO_8(base, 6)

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

Serial::Serial(Port port, Count transmits, Count receives)
: index(port)
, received(receives)
, transmitting(transmits)
{
	switch (port) {
	default:
	case USART0:	base = &UCSR0A;	break;
#if defined(UCSR1A)
	case USART1:	base = &UCSR1A;	break;
#if defined(UCSR2A)
	case USART2:	base = &UCSR2A;	break;
#if defined(UCSR3A)
	case USART3:	base = &UCSR2A;	break;
#endif
#endif
#endif
	}
	serial[index] = this;
}

Serial::~Serial() {
	Uninterruptable uninterruptable;
	stop();
	serial[index] = 0;
}

void Serial::start(Baud baud, Data data, Parity parity, Stop stop) const {
	uint32_t rate;
	switch (baud) {
	case B50:		rate = 50;		break;
	case B75:		rate = 75;		break;
	case B110:		rate = 110;		break;
	case B134:		rate = 134;		break;
	case B150:		rate = 150;		break;
	case B200:		rate = 200;		break;
	case B300:		rate = 300;		break;
	case B600:		rate = 600;		break;
	case B1200:		rate = 1200;	break;
	case B1800:		rate = 1800;	break;
	case B2400:		rate = 2400;	break;
	case B4800:		rate = 4800;	break;
	case B9600:		rate = 9600;	break;
	case B19200:	rate = 19200;	break;
	case B38400:	rate = 38400;	break;
	case B57600:	rate = 57600;	break;
	default:
	case B115200:	rate = 115200;	break;
	}

	uint16_t value = (((configCPU_CLOCK_HZ) + (4UL * rate)) / (8UL * rate)) - 1UL;

	Uninterruptable uninterruptable;

	UCSRB = 0;

	UBRRL = value & 0xff;
	UBRRH = (value >> 8) & 0xff;

	UCSRA = _BV(U2X0);

	UCSRC = parity | stop | data;

	UCSRB = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
}

void Serial::stop() const {
	Uninterruptable uninterruptable;
	UCSRB = 0;
}

void Serial::enable() const {
	UCSRB |= _BV(UDRIE0);
}

void Serial::disable() const {
	UCSRB &= ~_BV(UDRIE0);
}

bool Serial::isEnabled() const {
	return ((UCSRB & _BV(UDRIE0)) != 0);
}

void Serial::emit(uint8_t ch) const {
	while ((UCSRA & _BV(UDRE0)) == 0) {
		;
	}
	UDR = ch;
}

void Serial::receiver() {
	uint8_t ch = ((UCSRA & (_BV(FE0) | _BV(DOR0) | _BV(UPE0))) == 0) ? UDR : 0xff;
	bool woken = false;
	received.sendFromISR(&ch, woken);
	if (woken) {
		taskYIELD();
	}
}

void Serial::transmitter() {
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
