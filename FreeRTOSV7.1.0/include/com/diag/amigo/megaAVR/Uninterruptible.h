#ifndef _COM_DIAG_AMIGO_MEGAAVR_UNINTERRUPTIBLE_H_
#define _COM_DIAG_AMIGO_MEGAAVR_UNINTERRUPTIBLE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/cxxcapi.h"

/**
 * Disable interrupts and returns the value of SREG before it did so. This
 * function can be called from either C or C++ translation units.
 * @return the prior SREG value before interrupts were disabled.
 */
CXXCINLINE uint8_t amigo_uninterruptible_begin() {
	uint8_t sreg = SREG;
	cli();
	return sreg;
}

/**
 * Set the SREG to the specified value. This can be used to restore the prior
 * value of the SREG before interrupts were disabled. This function can be
 * called from either C or C++ translation units.
 * @param sreg is the new SREG value.
 */
CXXCINLINE void amigo_uninterruptible_end(uint8_t sreg) {
	SREG = sreg;
}

#if defined(__cplusplus)

namespace com {
namespace diag {
namespace amigo {

/**
 * Uninterruptible saves the value of SREG and disables interrupts in its
 * constructor, and restores the value of SREG in its destructor. This allows
 * scoped uninterruptible sections of code to be written, exploiting the
 * "Resource Acquisition is Initialization" idiom.
 */
class Uninterruptible
{

public:

	/**
	 * Constructor. The value of SREG is saved in an instance variable and
	 * interrupts are disabled.
	 */
	Uninterruptible()
	{
		sreg = amigo_uninterruptible_begin();
	}

	/**
	 * Destructor. The saved value of SREG is restored from an instance
	 * variable. The interrupt state in the saved SREG value (which could
	 * have been enabled, or in the case of nested Uninterruptible scopes,
	 * disabled) is restored.
	 */
	~Uninterruptible() {
		amigo_uninterruptible_end(sreg);
	}

	/**
	 * Returns the saved value of SREG from its instance variable.
	 * @return the saved value of SREG.
	 */
	operator uint8_t() {
		return sreg;
	}

protected:

	uint8_t sreg;

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Uninterruptible(const Uninterruptible & that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Uninterruptible & operator=(const Uninterruptible & that);

};

}
}
}

#endif /* defined(__cplusplus) */

#endif /* _COM_DIAG_AMIGO_MEGAAVR_UNINTERRUPTIBLE_H_ */
