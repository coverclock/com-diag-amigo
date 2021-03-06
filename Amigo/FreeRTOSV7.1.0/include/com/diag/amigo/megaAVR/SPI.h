#ifndef _COM_DIAG_AMIGO_MEGAAVR_SPI_H_
#define _COM_DIAG_AMIGO_MEGAAVR_SPI_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by FreeRTOS lib_spi and Arduino SPI.
 */

#include <avr/io.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/constants.h"
#include "com/diag/amigo/TypedQueue.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * SPI is an interrupt-driven device driver with an asynchronous producer
 * and consumer API for one or more Serial Peripheral Interface devices.
 * Characters received from the SPI by the interrupt service routine are pushed
 * into a fixed-size ring buffer in memory until the application can consume
 * them. Characters produced by the application to be transmitted are pushed
 * into a fixed-sized ring buffer in memory until the interrupt service
 * routine can retrieve them and transmit them via the SPI. The sizes of both
 * the transmit and receive ring buffers can be specified during construction.
 * Since SPI is always full duplex (you must transmit characters in order
 * to receive characters and vice versa), transmission and reception necessarily
 * happen concurrently. This is because SPI controllers in both SPI master and
 * slave devices are little more than hardware shift registers. SPI serial bus
 * operations by this interrupt driven driver are handled asynchronously as
 * a background activity while the caller is blocked. Because an SPI serial
 * bus is a hardware resource that may be shared among multiple SPI slave
 * devices, access not just to SPI but also to the individual slave select (SS)
 * pin on each SPI slave device must be serialized so that concurrent tasks
 * using SPI do not interfere with one another. This can be done with a
 * MutexSemaphore. This makes it easy to serialize use of the SPI and the
 * slave select pins (about which the SPI knows nothing) by using the
 * MutexSemaphore in a CriticalSection in the application.
 */
class SPI
{

	/***************************************************************************
	 * TYPES AND CONSTANTS
	 **************************************************************************/

public:

	/**
	 * Identifies the specific SPI controller to be associated with a particular
	 * SPI object. Generally there is only one SPI controller.
	 */
	enum Controller {
		SPI0 = 0,
		FAIL = 255
	};

	/**
	 * Identifies the bit order for transmission onto the SPI bus: Most
	 * Significant Bit first, or Least Significant Bit first.
	 */
	enum Order {
		MSB,
		LSB
	};

	/**
	 * Specifies the role of this SPI bus, Master or Slave. My long experience
	 * with another common embedded bus standard, I2C (a.k.a. TWI) suggests
	 * that it is a mistake to architect a system that switches back and forth
	 * between roles; the inability to reliably synchronize who is playing what
	 * role on the bus leads to wackiness ensuing. (I have no way currently to
	 * test the slave role.)
	 */
	enum Role {
		SLAVE,
		MASTER
	};

	/**
	 * Specifies signal polarity, normal or inverted.
	 */
	enum Polarity {
		NORMAL,
		INVERTED
	};

	/**
	 * Specifies the signal phase used to detect bits on the SPI bus, leading or
	 * trailing.
	 */
	enum Phase {
		LEADING,
		TRAILING
	};

	/**
	 * SPI devices require a SPI clock (pin SCK)  to drive the shift registers
	 * that clock bits onto and off of the SPI bus (pins MISO for Master In
	 * Slave Out and MOSI for Master Out Slave In). The SPI clock frequency
	 * depends on the abilities of both the master and slave device. For
	 * example, a divisor of D4 on a 16MHz ATmega328p or ATmega2560 requires
	 * that the master and slave devices be able to keep up with a 4MHz clock
	 * rate. Your mileage may vary.
	 */
	enum Divisor {
		D2,
		D4,
		D8,
		D16,
		D32,
		D64,
		D128
	};

	/**
	 * Defines the default receive ring buffer size in bytes. With the current
	 * design this should never need to be anything other than one for masters,
	 * but might need to be larger for slaves.
	 */
	static const size_t RECEIVES = 1;

	/**
	 * Defines the default transmit ring buffer size in bytes. All SPI transfers
	 * involve a transmit coupled with a matching receive, so there should be
	 * no reason why the size of the transmit and receive ring buffers aren't
	 * exactly the same for masters. In the current design,which hasn't been
	 * tested for slaves, there is never a reason for this to be more than one.
	 */
	static const size_t TRANSMITS = 1;

	/***************************************************************************
	 * CONSTRUCTING AND DESTRUCTING
	 **************************************************************************/

public:

	/**
	 * Constructor. The interrupt service routine for the specified SPI is
	 * automatically installed.
	 * @param mycontroller identities the SPI that this object manages.
	 * @param transmits specifies the size of the transmit ring buffer in bytes.
	 * @param receives specifies the size of the receive ring buffer in bytes.
	 */
	explicit SPI(Controller mycontroller = SPI0, size_t transmits = TRANSMITS, size_t receives = RECEIVES);

	/**
	 * Destructor. The stop() instance method is automatically called, and the
	 * interrupt service routine for this SPI is deinstalled.
	 */
	virtual ~SPI();

	/**
	 * Return true if construction was successful false otherwise.
	 * @return true if construction was successful, false otherwise.
	 */
	operator bool() const { return ((gpiobase != 0) && (spibase != 0)); }

	/***************************************************************************
	 * STARTING AND STOPPING
	 **************************************************************************/

public:

	/**
	 * Initialize the SPI and start interrupt driven I/O operations on it.
	 * @param divisor specifies the oscillator frequency divisor.
	 * @param role specifies whether this SPI controller is Master or Slave.
	 * @param order specifies Most or Least Significant Bit transmission order.
	 * @param polarity specifies Positive or Negative signal polarity.
	 * @param phase specifies signal phase Leading or Trailing.
	 */
	void start(Divisor divisor = D4, Role role = MASTER, Order order = MSB, Polarity polarity = NORMAL, Phase phase = LEADING);

	/**
	 * The SPI is disabled and all further interrupt driven I/O operations
	 * cease.
	 */
	void stop();

	/**
	 * Restart interrupt driven I/O operations after a stop() without
	 * reinitializing the SPI.
	 */
	void restart();

	/***************************************************************************
	 * READING AND WRITING
	 **************************************************************************/

public:

	/**
	 * Append a byte to the end of the transmit ring buffer and remove the byte
	 * at the beginning of the receive ring buffer and return it. This is only
	 * meaningful (I think) when acting as a master. All SPI I/O operations
	 * performed by a master are always a transmit of a byte coupled with a
	 * receive of a byte, although the received byte may be ignored. If there
	 * are multiple tasks using the SPI bus (whether they are using the same
	 * SPI slave device or different devices), use of this method should be
	 * serialized with a MutexSemaphore semaphore; this is the responsibility
	 * of the application.
	 * @param ch is the byte to append.
	 * @param timeout is the number of ticks to wait when the buffer is full.
	 * @return the first character or <0 if fail.
	 */
	int master(uint8_t ch = 0, ticks_t timeout = NEVER);

	/**
	 * Append a byte to the end of the transmit ring buffer and remove the byte
	 * at the beginning of the receive ring buffer and return it. This is only
	 * meaningful (I think) when acting as a master. All SPI I/O operations
	 * performed by a master are always a transmit of a byte coupled with a
	 * receive of a byte, although the received byte may be ignored. If there
	 * are multiple tasks using the SPI bus (whether they are using the same
	 * SPI slave device or different devices), use of this method should be
	 * serialized with a MutexSemaphore semaphore; this is the responsibility
	 * of the application. (This method is currently identical to master()
	 * and I have no way to test this driver in the slave role. However, I
	 * suspect that the primary difference between master and slave is that the
	 * master drives SCK and the slave passively listens to SCK. And obviously
	 * the master reads MISO and writes MOSI and the slave vice versa. Otherwise
	 * the software interface is the same. I wanted to have different API
	 * methods to leave open the possibility that I am full of crap.)
	 * @param ch is the byte to append.
	 * @param timeout is the number of ticks to wait when the buffer is full.
	 * @return the first character or <0 if fail.
	 */
	int slave(uint8_t ch = 0, ticks_t timeout = NEVER);

	/***************************************************************************
	 * CHECKING
	 **************************************************************************/

public:

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
	SPI & operator=(uint8_t value);

	/***************************************************************************
	 * INTERRUPTING
	 **************************************************************************/

public:

	/**
	 * This class method is called by the transfer complete interrupt vector
	 * function. It in turn calls the instance method specific in the SPI object
	 * for the specified controller. This method has to be public to be called
	 * from the interrupt vector function, which has C linkage; you should never
	 * call it.
	 * @param controller identifies the SPI from which the interrupt occurred.
	 */
	static void complete(Controller controller);

protected:

	volatile void * spibase;
	volatile void * gpiobase;
	TypedQueue<uint8_t> received;
	TypedQueue<uint8_t> transmitting;
	Controller controller;
	// Below is _our_ Slave Select (SS) when we are operating in slave mode.
	// When we are operating in master mode, each individual slave device on
	// the SPI bus will have its own SS pin which will be typically connected
	// to one of our GPIO pins and which we will have to toggle to enable the
	// device when we want to talk to it.
	uint8_t ss;		// Slave Select
	uint8_t sck;	// SPI Clock
	uint8_t mosi;	// Master Out Slave In
	uint8_t miso;	// Master In Slave Out
	uint8_t errors;

	/**
	 * Start an interrupt-driven I/O.
	 */
	void begin();

	/**
	 * Implement the instance transfer complete interrupt service routine.
	 */
	void complete();

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	SPI(const SPI & that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	SPI & operator=(const SPI& that);

};

inline int SPI::master(uint8_t ch, ticks_t timeout) {
	if (!transmitting.send(&ch, timeout)) {
		return -1;
	} else {
		begin();
		if (!received.receive(&ch, timeout)) {
			return -2;
		} else {
			return ch;
		}
	}
}

inline int SPI::slave(uint8_t ch, ticks_t timeout) {
	if (!transmitting.send(&ch, timeout)) {
		return -1;
	} else {
		begin();
		if (!received.receive(&ch, timeout)) {
			return -2;
		} else {
			return ch;
		}
	}
}

}
}
}

#endif /* _COM_DIAG_AMIGO_MEGAAVR_SPI_H_ */
