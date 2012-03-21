#ifndef _COM_DIAG_AMIGO_UNINTERRUPTABLE_H_
#define _COM_DIAG_AMIGO_UNINTERRUPTABLE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * These symbols are in the global name space!\n
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "com/diag/amigo/types.h"

namespace com {
namespace diag {
namespace amigo {

class Uninterruptable {

public:

	Uninterruptable()
	: sreg(SREG)
	{
		cli();
	}

	~Uninterruptable() {
		SREG = sreg;
	}

	operator Register() {
		return sreg;
	}

protected:

	Register sreg;

private:

    /**
     *  Copy constructor. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Uninterruptable(const Uninterruptable& that);

    /**
     *  Assignment operator. POISONED.
     *
     *  @param that refers to an R-value object of this type.
     */
	Uninterruptable& operator=(const Uninterruptable& that);

};

}
}
}

#endif /* _COM_DIAG_AMIGO_UNINTERRUPTABLE_H_ */
