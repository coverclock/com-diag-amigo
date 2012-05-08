#ifndef _COM_DIAG_AMIGO_MEGAAVR_UNINTERRUPTIBLE_H_
#define _COM_DIAG_AMIGO_MEGAAVR_UNINTERRUPTIBLE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/target/interrupts.h"

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
		sreg = interrupts::disable();
	}

	/**
	 * Destructor. The saved value of SREG is restored from an instance
	 * variable. The interrupt state in the saved SREG value (which could
	 * have been enabled, or in the case of nested Uninterruptible scopes,
	 * disabled) is restored.
	 */
	~Uninterruptible() {
		interrupts::restore(sreg);
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

#endif /* _COM_DIAG_AMIGO_MEGAAVR_UNINTERRUPTIBLE_H_ */
