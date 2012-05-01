#ifndef _COM_DIAG_AMIGO_MEGAAVR_A2D_H_
#define _COM_DIAG_AMIGO_MEGAAVR_A2D_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by and cribbed from wiring.c,
 * wiring_analog.c and pins_arduino.h by David A. Mellis and Mark Sproul.
 */

#include <avr/io.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/TypedQueue.h"
#include "com/diag/amigo/target/GPIO.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * A2D is an interrupt-driven device driver with an asynchronous producer
 * and consumer API for an Analog to Digital Converter (ADC). (I would have
 * called it ADC but that's already a preprocessor symbol in one of the AVR
 * libc header files.)
 */
class A2D
{

public:

	/**
	 * Pin numbers. These names can be matched to those in the data sheet by,
	 * for example, translating PIN_0 to ADC0. The conditional compilation
	 * below is based on the actual values in the AVR libc <io*.h> header files.
	 * The header file <io90pwm81.h> defines ADC6D and ADC8D but not ADC7D. I
	 * suspect that's a typo, but it hoses up PIN_7 and PIN_8 for that
	 * microcontroller. If you're porting this code to that model, you might
	 * consider fixing that header file instead of changing this code.
	 */
	enum Pin {
#if defined(ADC3D)
		PIN_0 = 0,
		PIN_1 = 1,
		PIN_2 = 2,
		PIN_3 = 3,
#endif
#if defined(ADC5D)
		PIN_4 = 4,
		PIN_5 = 5,
#endif
#if defined(ADC6D)
		PIN_6 = 6,
#endif
#if defined(ADC7D)
		PIN_7 = 7,
#endif
#if defined(ADC10D)
		PIN_8 = 8,
		PIN_9 = 9,
		PIN_10 = 10,
#endif
#if defined(ADC11D)
		PIN_11 = 11,
#endif
#if defined(ADC13D)
		PIN_12 = 12,
		PIN_13 = 13,
#endif
#if defined(ADC15D)
		PIN_14 = 14,
		PIN_15 = 15,
#endif
		INVALID = 255
	};

	/**
	 * There is only one analog to digital converter, and this identifies it.
	 */
	enum Converter {
		CONVERTER0
	};

	/**
	 * These are the possible reference voltages against which the ADC measures
	 * the pin. The internal Vcc is the most typical, followed by a voltage
	 * applied to the external AREF analog reference pin. Some megaAVR models
	 * offer other internal voltage choices.
	 */
	enum Reference {
		AREF,
		AVCC,
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		V_1_1,
		V_2_56,
#endif
		V_INTERNAL
	};

	/**
	 * These are the events that can trigger the start of an ADC conversion
	 * once it has been enabled. Most typically it is free running, occurring a
	 * soon as the ADC is enabled. But I can easily see applications for
	 * triggering it on an pulse on the external interrupt 0 (INT0) pin. For
	 * any of the other choices, the application has to do its own setup on
	 * the timer/counters or the analog comparator.
	 */
	enum Trigger {
		ON_DEMAND,
		FREE_RUNNING,
		ANALOG_COMPARATOR,
		EXTINT0,
		MATCH0A,
		OVERFLOW0,
		MATCH1B,
		OVERFLOW1,
		CAPTURE1
	};

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
	 * Defines the default conversion ring buffer size in bytes.
	 */
	static const size_t CONVERSIONS = 1;

	/**
	 * Defines the default request ring buffer size in bytes.
	 */
	static const size_t REQUESTS = 1;

	/**
	 * This class method is called by the conversion complete interrupt vector
	 * function. It in turn calls the instance method specific in the A2D object
	 * for the specified converter. This method has to be public to be called
	 * from the interrupt vector function, which has C linkage; you should never
	 * call it.
	 * @param converter identifies the ADC from which the interrupt occurred.
	 */
	static void complete(Converter converter);

	/***************************************************************************
	 * MAPPING CLASS METHODS
	 **************************************************************************/

	/**
	 * Map a pin to its equivalent GPIO pin.
	 * @param pin is a Pin enumerated value.
	 * @return a GPIO pin enumerated value including GPIO::INVALID if invalid.
	 */
	static GPIO::Pin a2d2gpio(Pin pin);

	/**
	 * Map an Arduino digital pin number (which is typically printed right on
	 * the printed circuit board) to a Pin enumerated value. The mapping will be
	 * different for different models of Arduinos and AVR microcontrollers. Note
	 * that the Arduino software defines the analog pin names (e.g. "A0") to be
	 * their equivalent Arduino digital pin numbers (e.g. for "A0", 18).
	 * @param number is an Arduino digital pin number.
	 * @return a Pin enumerated value or INVALID if not a valid pin number.
	 */
	static Pin arduino2a2d(uint8_t number);

	/***************************************************************************
	 * CREATION AND DESTRUCTION
	 **************************************************************************/

public:

	/**
	 * Constructor. The interrupt service routine for the specified ADC is
	 * automatically installed.
	 * @param myconverter identities the ADC that this object manages.
	 * @param requests specifies the size of the request ring buffer in bytes.
	 * @param conversions specifies the size of the conversion ring buffer in bytes.
	 */
	explicit A2D(Converter myconverter = CONVERTER0, size_t requests = REQUESTS, size_t conversions = CONVERSIONS);

	/**
	 * Destructor. The stop() instance method is automatically called, and the
	 * interrupt service routine for this ADC is deinstalled.
	 */
	virtual ~A2D();

	/**
	 * Return true if construction was successful false otherwise.
	 * @return true if construction was successful, false otherwise.
	 */
	operator bool() const { return (adcbase != 0); }

	/***************************************************************************
	 * STARTING AND STOPPING
	 **************************************************************************/

	/**
	 * Initialize the ADC and start interrupt driven I/O operations on it.
	 */
	void start(Trigger trigger = ON_DEMAND, Divisor divisor = D128);

	/**
	 * The ADC is disabled and all further interrupt driven I/O operations
	 * cease.
	 */
	void stop();

	/**
	 * Restart interrupt driven I/O operations after a stop() without
	 * reinitializing the ADC.
	 */
	void restart();

	/***************************************************************************
	 * READING AND WRITING
	 **************************************************************************/

	/**
	 * Return the number of samples in the conversion ring buffer available to
	 * be read.
	 * @return the number of samples available or <0 if fail.
	 */
	int available() const;

	/**
	 * Append a request composed of a pin number and a reference to the end of
	 * the request ring buffer.
	 * @param request is the pin number to append.
	 * @param timeout is the number of ticks to wait when the buffer is full.
	 * @return one if the function was successful, zero otherwise.
	 */
	size_t request(Pin pin, Reference reference = AVCC, ticks_t timeout = NEVER);

	/**
	 * Return the first sample in the conversion ring buffer and remove it from
	 * the buffer. The sample will be an unsigned quantity in the lower ten-bits
	 * of the returned value. If the returned value is not negative, you can
	 * simply assigned it to an uint16_t.
	 * @param timeout is the number of ticks to wait when the buffer is empty.
	 * @return the first sample or <0 if fail.
	 */
	int conversion(ticks_t timeout = NEVER);

	/**
	 * Combine the sending of a request and the receiving of its conversion
	 * in single method. If there are multiple tasks using the ADC (whether
	 * they are using the same A2D pin or different pins), use of this method
	 * should be serialized with a MutexSemaphore semaphore; this is the
	 * responsibility of the application.
	 * @param request is the pin to sample and convert.
	 * @param timeout is the number of ticks to wait when the buffer is empty.
	 * @return the first sample or <0 if fail.
	 */
	int convert(Pin pin, Reference reference = AVCC, ticks_t timeout = NEVER);

	/***************************************************************************
	 * CHECKING
	 **************************************************************************/

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
	A2D & operator=(uint8_t value);

protected:

	volatile void * adcbase;
	TypedQueue<uint16_t> converted; // An incoming queue of ten-bit conversions.
	TypedQueue<uint8_t> requesting; // An outgoing queue of requests.
	Converter converter;
	uint8_t errors;

	/**
	 * Start an interrupt-driven I/O.
	 */
	void begin();

	/**
	 * Start an analog-to-digital conversion.
	 * @param request combines a reference enumerated value with a pin enumerated value.
	 */
	void adc(uint8_t request);

	/**
	 * Implement the instance conversion complete interrupt service routine.
	 */
	void complete();

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	A2D(const A2D & that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	A2D & operator=(const A2D& that);

};

inline int A2D::available() const {
	return converted.available();
}

inline size_t A2D::request(Pin pin, Reference reference, ticks_t timeout) {
	uint8_t request = (reference << 4) | (pin & 0x0f);
	if (!requesting.send(&request, timeout)) {
		return 0;
	} else {
		begin();
		return 1;
	}
}

inline int A2D::conversion(ticks_t timeout) {
	uint16_t sample;
	return (converted.receive(&sample, timeout) ? sample : -1);

}

}
}
}

#endif /* _COM_DIAG_AMIGO_MEGAAVR_A2D_H_ */
