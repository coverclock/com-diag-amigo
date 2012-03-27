#ifndef _COM_DIAG_SERIAL_H_
#define _COM_DIAG_SERIAL_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/values.h"
#include "com/diag/amigo/TypedQueue.h"

namespace com {
namespace diag {
namespace amigo {

class Serial
{

public:

	enum Port {
		USART0		= 0,
#if defined(UCSR1A)
		USART1		= 1,
#if defined(UCSR2A)
		USART2 		= 2,
#if defined(UCSR3A)
		USART3		= 3,
		MAXUSART	= 3
#else
		MAXUSART	= 2
#endif
#else
		MAXUSART	= 1
#endif
#else
		MAXUSART	= 0
#endif
	};

	enum Baud {
		B50,
		B75,
		B110,
		B134,
		B150,
		B200,
		B300,
		B600,
		B1200,
		B1800,
		B2400,
		B4800,
		B9600,
		B19200,
		B38400,
		B57600,
		B115200
	};

	enum Data {
		FIVE		= 0,
		SIX			= _BV(UCSZ00),
		SEVEN		= _BV(UCSZ01),
		EIGHT		= _BV(UCSZ01) | _BV(UCSZ00)
	};


	enum Parity {
		NONE		= 0,
		EVEN		= _BV(UPM01),
		ODD			= _BV(UPM01) | _BV(UPM00)
	};

	enum Stop {
		ONE			= 0,
		TWO			= _BV(USBS0)
	};

	static const Count RECEIVES = 16;

	static const Count TRANSMITS = 82;

	static const uint8_t BAD = '?';

	static void receive(Port port);

	static void transmit(Port port);

protected:

	static Serial * serial[MAXUSART + 1];

public:

	explicit Serial(Port myport = USART0, Count transmits = TRANSMITS, Count receives = RECEIVES, uint8_t mybad = BAD);

	virtual ~Serial();

	void start(Baud baud = B115200, Data data = EIGHT, Parity parity = NONE, Stop stop = ONE) const;

	void restart() const;

	void stop() const;

	int available() const;

	void flush() const;

	int peek(Ticks timeout = NEVER);

	int read(Ticks timeout = NEVER);

	size_t write(uint8_t ch, Ticks timeout = NEVER);

	size_t emit(uint8_t ch) const;

protected:

	Port port;
	volatile void * base;
	TypedQueue<uint8_t> received;
	TypedQueue<uint8_t> transmitting;
	uint8_t bad;

private:

	void enable() const;

	void disable() const;

	void receiver();

	void transmitter();

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Serial(const Serial& that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Serial& operator=(const Serial& that);

};

inline void Serial::receive(Port port) {
	if (serial[port] != 0) {
		serial[port]->receiver();
	}
}

inline void Serial::transmit(Port port) {
	if (serial[port] != 0) {
		serial[port]->transmitter();
	}
}

inline void Serial::restart() const {
	if (transmitting.available() > 0) {
		enable();
	}
}

inline int Serial::available() const {
	return received.available();
}

inline void Serial::flush() const {
	while (transmitting.available() > 0) {
		taskYIELD();
	}
}

inline int Serial::peek(Ticks timeout) {
	uint8_t ch;
	return (received.peek(&ch, timeout) ? ch : -1);
}

inline int Serial::read(Ticks timeout) {
	uint8_t ch;
	return (received.receive(&ch, timeout) ? ch : -1);
}

inline size_t Serial::write(uint8_t ch, Ticks timeout) {
	if (!transmitting.send(&ch, timeout)) {
		return 0;
	} else {
		enable();
		return 1;
	}
}

}
}
}

#endif /* _COM_DIAG_SERIAL_H_ */
