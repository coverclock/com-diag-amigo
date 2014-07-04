#ifndef _COM_DIAG_AMIGO_TOGGLE_H_
#define _COM_DIAG_AMIGO_TOGGLE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/target/GPIO.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * ToggleOff toggles zero or more bits in a GPIO port off (low) in its
 * constructor, and on (high) in its destructor. A typical use is to toggle a
 * SPI Slave Select (SS) signal that is active low within a lexical scope in
 * which the caller is acting as an SPI master. Note that this class assumes
 * the GPIO bits are already defined to be outputs; not doing so would prevent
 * passing a 0 as a mask to effectively make this a no-op.
 */
class ToggleOff {

public:

	/**
	 * Constructor. The bits indicated by the mask are cleared.
	 * @param mygpio refers to a GPIO object for a particular GPIO port.
	 * @param mymask is a bit mask indicating what bits to control.
	 */
	ToggleOff(GPIO & mygpio, uint8_t mymask)
	: gpio(&mygpio)
	, mask(mymask)
	{
		gpio->clear(mask); /* Active low. */
	}

	/**
	 * Destructor.  The bits indicated by the mask are set.
	 */
	~ToggleOff() {
		gpio->set(mask); /* Active low. */
	}

private:

	GPIO * gpio;
	uint8_t mask;

};

/**
 * ToggleOn toggles zero or more bits in a GPIO port on (high) in its
 * constructor, and off (low) in its destructor. A typical use is to turn an
 * LED on and off in a lexical scope. Note that this class assumes
 * the GPIO bits are already defined to be outputs; not doing so would prevent
 * passing a 0 as a mask to effectively make this a no-op.
 */
class ToggleOn {

public:

	/**
	 * Constructor.  The bits indicated by the mask are set.
	 * @param mygpio refers to a GPIO object for a particular GPIO port.
	 * @param mymask is a bit mask indicating what bits to control.
	 */
	ToggleOn(GPIO & mygpio, uint8_t mymask)
	: gpio(&mygpio)
	, mask(mymask)
	{
		gpio->set(mask); /* Active low. */
	}

	/**
	 * Destructor.  The bits indicated by the mask are cleared.
	 */
	~ToggleOn() {
		gpio->clear(mask); /* Active low. */
	}

private:

	GPIO * gpio;
	uint8_t mask;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_TOGGLE_H_ */
