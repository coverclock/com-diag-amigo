#ifndef _COM_DIAG_MEGAAVR_SERIAL_H_
#define _COM_DIAG_MEGAAVR_SERIAL_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/TypedQueue.h"

namespace com {
namespace diag {
namespace amigo {

class Serial
{

public:

	enum Port {
		CONSOLE		= 0,
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
		FIVE,
		SIX,
		SEVEN,
		EIGHT
	};


	enum Parity {
		NONE,
		EVEN,
		ODD
	};

	enum Stop {
		ONE,
		TWO
	};

	static const Ticks IMMEDIATELY = Queue::IMMEDIATELY;

	static const Ticks NEVER = Queue::NEVER;

	static const Count RECEIVES = 16;

	static const Count TRANSMITS = 82;

	static const uint8_t BAD = '?';

	static void receive(Port port);

	static void transmit(Port port);

protected:

	static Serial * serial[MAXUSART + 1];

public:

	explicit Serial(Port myport = CONSOLE, Count transmits = TRANSMITS, Count receives = RECEIVES, uint8_t mybad = BAD);

	virtual ~Serial();

	void start(Baud baud = B115200, Data data = EIGHT, Parity parity = NONE, Stop stop = ONE) const;

	void start(uint32_t rate, Data data = EIGHT, Parity parity = NONE, Stop stop = ONE) const;

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

	void enable() const;

	void disable() const;

	void yield() const;

private:

	void receive();

	void transmit();

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
		yield();
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

#endif /* _COM_DIAG_MEGAAVR_SERIAL_H_ */
