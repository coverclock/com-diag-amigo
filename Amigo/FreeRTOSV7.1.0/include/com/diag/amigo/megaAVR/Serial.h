#ifndef _COM_DIAG_AMIGO_MEGAAVR_SERIAL_H_
#define _COM_DIAG_AMIGO_MEGAAVR_SERIAL_H_

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

#include <avr/io.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/TypedQueue.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * Serial is an interrupt-driven device driver with an asynchronous producer
 * and consumer API or one or more USART serial devices. Characters received
 * from the USART by the interrupt service routine are pushed into a fixed-size
 * ring buffer in memory until the application can consume them. Characters
 * produced by the application to be transmitted are pushed into a fixed-sized
 * ring buffer in memory until the interrupt service routine can retrieve them
 * and transmit them via the USART. The sizes of both the transmit and receive
 * ring buffers can be specified during construction. These ring buffers are
 * independent of and in addition to the FIFO inside the USART hardware itself,
 * which is typically a couple of bytes long. The ring buffers not only provide
 * an asynchronous API between the application and the interrupt service
 * routines, but also provide forms of rate smoothing, traffic shaping, and
 * even traffic policing, depending on what the application does if and when
 * a ring buffer fills up (which should be generally considered to be a Bad
 * Thing). USART operations by this interrupt driven driver are handled
 * asynchronously as a background activity while the caller is blocked.
 * Concurrent tasks writing to the USART using this object can hence find their
 * output or input interleaved. Access to this object can be serialized using a
 * MutexSemaphore. This makes it easy to serialize use of the USART by using the
 * MutexSemaphore in a CriticalSection in the application.
 */
class Serial
{

public:

	/**
	 * Identifies the specific USART serial port to be associated with a
	 * particular Serial object. Some megaAVR devices only have one USART,
	 * some have several. The default console is assumed to be USART0, which
	 * is the USART used by the typical boot loader.
	 */
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

	/**
	 * Specifies the desired baud rate in bits per second from among the
	 * supported rates. If you are not sure what to choose, values like
	 * B115200 or for some applications B9600 are good choices.
	 */
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

	/**
	 * Specifies the number of data bits per character. If you are not sure
	 * what to choose, EIGHT is a good choice. Although the megaAVR USART
	 * may support nine data bits, that is just crazy talk, and this API does
	 * not support it.
	 */
	enum Data {
		FIVE,
		SIX,
		SEVEN,
		EIGHT
	};

	/**
	 * Specifies the number of parity bits per character. If you are not sure
	 * what to choose, NONE is a good choice.
	 */
	enum Parity {
		NONE,
		EVEN,
		ODD
	};

	/**
	 * Specifies the number of stop bits per character. If you are not sure
	 * what to choose, ONE is a good choice. There is always just one start bit.
	 */
	enum Stop {
		ONE,
		TWO
	};

	/**
	 * Defines the timeout value in ticks that causes the application to never
	 * block waiting for ring buffer space or data, but instead be returned an
	 * error.
	 */
	static const ticks_t IMMEDIATELY = Queue::IMMEDIATELY;

	/**
	 * Defines the timeout value in ticks that causes the application to block
	 * indefinitely waiting for ring buffer space or data.
	 */
	static const ticks_t NEVER = Queue::NEVER;

	/**
	 * Defines the default receive ring buffer size in bytes. This value is
	 * not even a scientific wild ass guess.
	 */
	static const size_t RECEIVES = 16;

	/**
	 * Defines the default transmit ring buffer size in bytes. If you can
	 * believe it, this value is based on the typical 1970s vintage dumb
	 * terminal screen width, which in turn was based on the size of a 1960s
	 * vintage punch card, plus a trailing carriage return and line feed.
	 */
	static const size_t TRANSMITS = 82;

	/**
	 * Defines the character that is returned when the received character had
	 * parity or other framing errors in it. It can be important to return
	 * some character rather than throwing it away, since the consumer may
	 * be waiting for a very specific number of characters from the producer.
	 * A question mark works well for printable data that will be displayed.
	 * For binary data, it is up to the application, although values like 0x00
	 * or 0xff might be usable. Note that the counter is kept of the number
	 * of received errors, and this counter can be interrogated and cleared
	 * by the application.
	 */
	static const uint8_t BAD = '?';

	/**
	 * This class method is called by the receive interrupt vector function.
	 * It in turn calls the instance method specific in the Serial object for
	 * the specified port. This method has to be public to be called from the
	 * interrupt vector function, which has C linkage; you should never call it.
	 * @param port identifies the USART port from which the interrupt occurred.
	 */
	static void receive(Port port);

	/**
	 * This class method is called by the transmit interrupt vector function.
	 * It in turn calls the instance method specific in the Serial object for
	 * the specified port. This method has to be public to be called from the
	 * interrupt vector function, which has C linkage; you should never call it.
	 * @param port identifies the USART port from which the interrupt occurred.
	 */
	static void transmit(Port port);

	/**
	 * This class method exists just for debugging. It reinitializes all static
	 * variables in this class back to their initial states. This is sometimes
	 * useful when debugging a "jump to reset vector" bug, but should never be
	 * necessary in practice.
	 */
	static void initialize();

protected:

	static Serial * serial[MAXUSART + 1];

public:

	/**
	 * Constructor. The interrupt service routine for the specified USART is
	 * automatically installed.
	 * @param myport identities the USART that this object manages.
	 * @param transmits specifies the size of the transmit ring buffer in bytes.
	 * @param receives specifies the size of the receive ring buffer in bytes.
	 * @param mybad specifies the character to be used for a receive error.
	 */
	explicit Serial(Port myport = CONSOLE, size_t transmits = TRANSMITS, size_t receives = RECEIVES, uint8_t mybad = BAD);

	/**
	 * Destructor. The stop() instance method is automatically called, and the
	 * interrupt service routine for this USART is deinstalled.
	 */
	virtual ~Serial();

	/**
	 * Initialize the USART and start interrupt driven I/O operations on it.
	 * @param baud specifies the desired baud rate.
	 * @param data specifies the desired number of data bits per character.
	 * @param parity specifies the desired parity bits per character.
	 * @param stop specifies the number of parity bits per character.
	 */
	void start(Baud baud = B115200, Data data = EIGHT, Parity parity = NONE, Stop stop = ONE);

	/**
	 * Initialize the USART and start interrupt driven I/O operations on it.
	 * This interface exists to support Arduino, whose hardware serial interface
	 * expects to be able to specify the baud rate numerically. No matter what
	 * baud rate is specified, the underlying code will compute a value to be
	 * used by the USART that will quantize the specified rate into a much
	 * smaller set of implementable values. It is best to stick to the values
	 * implied by the Baud enumeration.
	 * @param rate specifies the desired baud rate in bits per second.
	 * @param data specifies the desired number of data bits per character.
	 * @param parity specifies the desired parity bits per character.
	 * @param stop specifies the number of parity bits per character.
	 */
	void start(uint32_t rate, Data data = EIGHT, Parity parity = NONE, Stop stop = ONE);

	/**
	 * The USART is disabled and all further interrupt driven I/O operations
	 * cease.
	 */
	void stop();

	/**
	 * Restart interrupt driven I/O operations after a stop() without
	 * reinitializing the USART.
	 */
	void restart();

	/**
	 * Return the number of characters in the receive ring buffer available to
	 * be read. This includes bad characters.
	 * @return the number of characters available or <0 if fail.
	 */
	int available() const;

	/**
	 * Block the calling task until the transmit ring buffer is empty. Since
	 * the USART hardware that has a FIFO that is typically two characters long,
	 * this is not quite the same thing as saying all pending characters having
	 * been transmitted. This interface exists to support Arduino, whose
	 * generic Stream interface expects to have this functionality.
	 */
	void flush();

	/**
	 * Clear all the data in the receive ring buffer, discarding it.
	 */
	void clear(ticks_t timeout = IMMEDIATELY);

	/**
	 * Return the first character in the receive ring buffer without removing
	 * it from the buffer. This is vastly useful when implementing certain
	 * classes of parsers. I have always suspected it is the reason why UNIX
	 * I/O streams implemented the ungetch() function.
	 * @param timeout is the number of ticks to wait when the buffer is empty.
	 * @return the first character or <0 if fail.
	 */
	int peek(ticks_t timeout = NEVER);

	/**
	 * Return the first character in the receive ring buffer and remove it from
	 * the buffer.
	 * @param timeout is the number of ticks to wait when the buffer is empty.
	 * @return the first character or <0 if fail.
	 */
	int read(ticks_t timeout = NEVER);

	/**
	 * Append a character to the end of the transmit ring buffer.
	 * @param ch is the character to be appended.
	 * @param timeout is the number of ticks to wait when the buffer is full.
	 * @return one if the function was successful, zero otherwise.
	 */
	size_t write(uint8_t ch, ticks_t timeout = NEVER);

	/**
	 * Prepend a character to the beginning of the transmit ring buffer. This
	 * will be the next character to be transmitted from the buffer, ahead of
	 * all other pending characters in the buffer. This is useful for sending
	 * special characters "out of band" for purposes of, for example, flow
	 * control.
	 * @param ch is the character to be prepended.
	 * @param timeout is the number of ticks to wait when the buffer is full.
	 * @return one if the function was successful, zero otherwise.
	 */
	size_t express(uint8_t ch, ticks_t timeout = NEVER);

	/**
	 * Cast this object to an integer by returning the error counter. The error
	 * counter is cumulative. The application is responsible for interrogating
	 * it using this operator and resetting it.
	 * @return the error counter.
	 */
	operator uint8_t() const { return errors; }

	/**
	 * Set the error counter to the specified integer value. Zero is a good
	 * value, which resets the error counter.
	 * @param value is the new error counter value.
	 * @return a reference to this object.
	 */
	Serial & operator=(uint8_t value);

protected:

	volatile void * base;
	TypedQueue<uint8_t> received;
	TypedQueue<uint8_t> transmitting;
	Port port;
	double microseconds;
	uint8_t bad;
	uint8_t errors;

	void begin();

private:

	/**
	 * Implement the instance receive interrupt service routine.
	 */
	void receive();

	/**
	 * Implement the instance transmit interrupt service routine.
	 */
	void transmit();

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Serial(const Serial & that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Serial & operator=(const Serial& that);

};

inline int Serial::available() const {
	return received.available();
}

inline void Serial::clear(ticks_t timeout) {
	uint8_t ch;
	while (received.receive(&ch, timeout)) {
		// Do nothing.
	}
}

inline int Serial::peek(ticks_t timeout) {
	uint8_t ch;
	return (received.peek(&ch, timeout) ? ch : -1);
}

inline int Serial::read(ticks_t timeout) {
	uint8_t ch;
	return (received.receive(&ch, timeout) ? ch : -1);
}

inline size_t Serial::write(uint8_t ch, ticks_t timeout) {
	if (!transmitting.send(&ch, timeout)) {
		return 0;
	} else {
		begin();
		return 1;
	}
}

inline size_t Serial::express(uint8_t ch, ticks_t timeout) {
	if (!transmitting.express(&ch, timeout)) {
		return 0;
	} else {
		begin();
		return 1;
	}
}


}
}
}

#endif /* _COM_DIAG_AMIGO_MEGAAVR_SERIAL_H_ */
