#ifndef _COM_DIAG_AMIGO_MEGAAVR_INTERRUPTS_H_
#define _COM_DIAG_AMIGO_MEGAAVR_INTERRUPTS_H_

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
 * Enable interrupts system-wide. This is typically called once in main()
 * when interrupt-driven I/O is desired. This function can be called from either
 * C or C++ translation units.
 * @return the prior SREG value before interrupts were enabled.
 */
CXXCINLINE uint8_t amigo_interrupts_enable(void) {
	uint8_t sreg = SREG;
	sei();
	return sreg;
}

/**
 * Disable interrupts and returns the value of SREG before it did so. This
 * function can be called from either C or C++ translation units.
 * @return the prior SREG value before interrupts were disabled.
 */
CXXCINLINE uint8_t amigo_interrupts_disable(void) {
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
CXXCINLINE void amigo_interrupts_restore(uint8_t sreg) {
	SREG = sreg;
}

#if defined(__cplusplus)

namespace com {
namespace diag {
namespace amigo {
namespace interrupts {

/**
 * Enable interrupts system-wide. This is typically called once in main()
 * when interrupt-driven I/O is desired. This function can be called from either
 * C or C++ translation units.
 * @return the prior SREG value before interrupts were enabled.
 */
inline uint8_t enable() {
	return amigo_interrupts_enable();
}

/**
 * Disable interrupts and returns the value of SREG before it did so. This
 * function can be called from either C or C++ translation units.
 * @return the prior SREG value before interrupts were disabled.
 */
inline uint8_t disable() {
	return amigo_interrupts_disable();
}

/**
 * Set the SREG to the specified value. This can be used to restore the prior
 * value of the SREG before interrupts were disabled. This function can be
 * called from either C or C++ translation units.
 * @param sreg is the new SREG value.
 */
inline void restore(uint8_t sreg) {
	amigo_interrupts_restore(sreg);
}

}
}
}
}

#endif /* defined(__cplusplus) */

#endif /* _COM_DIAG_AMIGO_MEGAAVR_INTERRUPTS_H_ */
