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
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/values.h"
#include "com/diag/amigo/Queue.h"
#include "com/diag/amigo/SourceSink.h"

namespace com {
namespace diag {
namespace amigo {

class Serial {

public:

	static const Count TRANSMITS = 82;

	static const Count RECEIVES = 16;

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

protected:

	static Serial * driver[MAXUSART + 1];

public:

	static void receive(Port port);

	static void transmit(Port port);

	explicit Serial(Port port = USART0, Count transmits = TRANSMITS, Count receives = RECEIVES);

	virtual ~Serial();

	void start(Baud baud = B115200, Data data = EIGHT, Parity parity = NONE, Stop stop = ONE) const;

	void stop() const;

	int available() const;

	void flush() const;

	int peek(Ticks timeout = NEVER);

	int read(Ticks timeout = NEVER);

	size_t write(uint8_t ch, Ticks timeout = NEVER);

	void emit(uint8_t ch) const;

protected:

	Port index;
	volatile void * base;
	Queue transmitting;
	Queue received;

private:

	inline void enable() const;

	inline void disable() const;

	inline bool isEnabled() const;

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
	if (driver[port] != 0) {
		driver[port]->receiver();
	}
}

inline void Serial::transmit(Port port) {
	if (driver[port] != 0) {
		driver[port]->transmitter();
	}
}

}
}
}

#endif /* _COM_DIAG_SERIAL_H_ */
