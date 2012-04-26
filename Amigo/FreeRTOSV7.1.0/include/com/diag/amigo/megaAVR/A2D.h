#ifndef _COM_DIAG_AMIGO_MEGAAVR_A2D_H_
#define _COM_DIAG_AMIGO_MEGAAVR_A2D_H_

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
	 * suspect that's a typo, but it hoses up this implementation for that model
	 * of microcontroller. If you're porting this code to that model, you might
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

	/**
	 * Map a pin to a digital input disable register address.
	 * @param pin is a Pin enumerated value.
	 * @return a disable register address or NULL if invalid.
	 */
	static volatile void * a2d2disable(Pin pin);

	/**
	 * Map a pin to a ADC bit offset.
	 * @param pin is a Pin enumerated value.
	 * @return a control bit offset or ~0 if invalid.
	 */
	static uint8_t a2d2offset(Pin pin);

	/**
	 * Map a pin to a ADC bit mask.
	 * @param pin is a Pin enumerated value.
	 * @return a control bit mask or 0 if invalid.
	 */
	static uint8_t a2d2mask(Pin pin);

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

	/**
	 * Initialize the ADC and start interrupt driven I/O operations on it.
	 * @param divisor specifies the oscillator frequency divisor.
	 * @param role specifies whether this SPI controller is Master or Slave.
	 * @param order specifies Most or Least Significant Bit transmission order.
	 * @param polarity specifies Positive or Negative signal polarity.
	 * @param phase specifies signal phase Leading or Trailing.
	 */
	//void start(Divisor divisor = D4, Role role = MASTER, Order order = MSB, Polarity polarity = NORMAL, Phase phase = LEADING);

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

protected:

	volatile void * adcbase;
	TypedQueue<uint16_t> converted; // An incoming queue of ten-bit conversions.
	TypedQueue<uint8_t> requesting; // An outgoing queue of requests.
	Converter converter;

	void begin();

private:

	/**
	 * Implement the instance conversion complete interrupt service routine.
	 */
	void complete();

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

inline uint8_t A2D::a2d2mask(Pin pin) {
	uint8_t offset = a2d2offset(pin);
	return (offset != static_cast<uint8_t>(~0)) ? (1 << offset) : 0;
}

}
}
}

#endif /* _COM_DIAG_AMIGO_MEGAAVR_A2D_H_ */
