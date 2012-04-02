#ifndef _COM_DIAG_AMIGO_MEGAAVR_UNINTERRUPTABLE_H_
#define _COM_DIAG_AMIGO_MEGAAVR_UNINTERRUPTABLE_H_

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
CXXCINLINE uint8_t amigo_uninterruptable_begin() {
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
CXXCINLINE void amigo_uninterruptable_end(uint8_t sreg) {
	SREG = sreg;
}

#if defined(__cplusplus)

namespace com {
namespace diag {
namespace amigo {

/**
 * Uninterruptable saves the value of SREG and disables interrupts in its
 * constructor, and restores the value of SREG in its destructor. This allows
 * scoped uninterruptable sections of code to be written, exploiting the
 * "Resource Acquisition is Initialization" idiom.
 */
class Uninterruptable
{

public:

	/**
	 * Constructor. The value of SREG is saved in an instance variable and
	 * interrupts are disabled.
	 */
	Uninterruptable()
	{
		sreg = amigo_uninterruptable_begin();
	}

	/**
	 * Destructor. The saved value of SREG is restored from an instance
	 * variable. The interrupt state in the saved SREG value (which could
	 * have been enabled, or in the case of nested Uninterruptable scopes,
	 * disabled) is restored.
	 */
	~Uninterruptable() {
		amigo_uninterruptable_end(sreg);
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
	Uninterruptable(const Uninterruptable & that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Uninterruptable & operator=(const Uninterruptable & that);

};

}
}
}

#endif /* defined(__cplusplus) */

#endif /* _COM_DIAG_AMIGO_MEGAAVR_UNINTERRUPTABLE_H_ */
