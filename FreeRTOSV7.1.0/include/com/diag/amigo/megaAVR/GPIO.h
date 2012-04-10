#ifndef _COM_DIAG_AMIGO_MEGAAVR_GPIO_H_
#define _COM_DIAG_AMIGO_MEGAAVR_GPIO_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/unused.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * GPIO maps an abstract GPIO pin number to a base memory-mapped register base
 * address, an bit offset, or eight-bit mask. Note that this is a simple
 * sequential mapping. Arduino uses a much more complicated mapping that
 * bears little resemblance to this.
 */
class GPIO {

public:

	/**
	 * Pin numbers. I would have preferred the usual nomenclature PA0, PB7,
	 * etc. but those are defined in <avr/io.h> as preprocessor symbols that
	 * are merely the bit offset in each individual register.
	 */
	enum Pin {
#if defined(PINA)
		PIN_A0 = 8 * 0,
		PIN_A1 = PIN_A0 + 1,
		PIN_A2 = PIN_A0 + 2,
		PIN_A3 = PIN_A0 + 3,
		PIN_A4 = PIN_A0 + 4,
		PIN_A5 = PIN_A0 + 5,
		PIN_A6 = PIN_A0 + 6,
		PIN_A7 = PIN_A0 + 7,
#endif
#if defined(PINB)
		PIN_B0 = 8 * 1,
		PIN_B1 = PIN_B0 + 1,
		PIN_B2 = PIN_B0 + 2,
		PIN_B3 = PIN_B0 + 3,
		PIN_B4 = PIN_B0 + 4,
		PIN_B5 = PIN_B0 + 5,
		PIN_B6 = PIN_B0 + 6,
		PIN_B7 = PIN_B0 + 7,
#endif
#if defined(PINC)
		PIN_C0 = 8 * 2,
		PIN_C1 = PIN_C0 + 1,
		PIN_C2 = PIN_C0 + 2,
		PIN_C3 = PIN_C0 + 3,
		PIN_C4 = PIN_C0 + 4,
		PIN_C5 = PIN_C0 + 5,
		PIN_C6 = PIN_C0 + 6,
		PIN_C7 = PIN_C0 + 7,
#endif
#if defined(PIND)
		PIN_D0 = 8 * 3,
		PIN_D1 = PIN_D0 + 1,
		PIN_D2 = PIN_D0 + 2,
		PIN_D3 = PIN_D0 + 3,
		PIN_D4 = PIN_D0 + 4,
		PIN_D5 = PIN_D0 + 5,
		PIN_D6 = PIN_D0 + 6,
		PIN_D7 = PIN_D0 + 7,
#endif
#if defined(PINE)
		PIN_E0 = 8 * 4,
		PIN_E1 = PIN_E0 + 1,
		PIN_E2 = PIN_E0 + 2,
		PIN_E3 = PIN_E0 + 3,
		PIN_E4 = PIN_E0 + 4,
		PIN_E5 = PIN_E0 + 5,
		PIN_E6 = PIN_E0 + 6,
		PIN_E7 = PIN_E0 + 7,
#endif
#if defined(PINF)
		PIN_F0 = 8 * 5,
		PIN_F1 = PIN_F0 + 1,
		PIN_F2 = PIN_F0 + 2,
		PIN_F3 = PIN_F0 + 3,
		PIN_F4 = PIN_F0 + 4,
		PIN_F5 = PIN_F0 + 5,
		PIN_F6 = PIN_F0 + 6,
		PIN_F7 = PIN_F0 + 7,
#endif
#if defined(PING)
		PIN_G0 = 8 * 6,
		PIN_G1 = PIN_G0 + 1,
		PIN_G2 = PIN_G0 + 2,
		PIN_G3 = PIN_G0 + 3,
		PIN_G4 = PIN_G0 + 4,
		PIN_G5 = PIN_G0 + 5,
//		PIN_G6 = PIN_G0 + 6,
//		PIN_G7 = PIN_G0 + 7,
#endif
#if defined(PINH)
		PIN_H0 = 8 * 7,
		PIN_H1 = PIN_H0 + 1,
		PIN_H2 = PIN_H0 + 2,
		PIN_H3 = PIN_H0 + 3,
		PIN_H4 = PIN_H0 + 4,
		PIN_H5 = PIN_H0 + 5,
		PIN_H6 = PIN_H0 + 6,
		PIN_H7 = PIN_H0 + 7,
#endif
#if defined(PINJ)
		PIN_J0 = 8 * 8,
		PIN_J1 = PIN_J0 + 1,
		PIN_J2 = PIN_J0 + 2,
		PIN_J3 = PIN_J0 + 3,
		PIN_J4 = PIN_J0 + 4,
		PIN_J5 = PIN_J0 + 5,
		PIN_J6 = PIN_J0 + 6,
		PIN_J7 = PIN_J0 + 7,
#endif
#if defined(PINK)
		PIN_K0 = 8 * 9,
		PIN_K1 = PIN_K0 + 1,
		PIN_K2 = PIN_K0 + 2,
		PIN_K3 = PIN_K0 + 3,
		PIN_K4 = PIN_K0 + 4,
		PIN_K5 = PIN_K0 + 5,
		PIN_K6 = PIN_K0 + 6,
		PIN_K7 = PIN_K0 + 7,
#endif
#if defined(PINL)
		PIN_L0 = 8 * 10,
		PIN_L1 = PIN_L0 + 1,
		PIN_L2 = PIN_L0 + 2,
		PIN_L3 = PIN_L0 + 3,
		PIN_L4 = PIN_L0 + 4,
		PIN_L5 = PIN_L0 + 5,
		PIN_L6 = PIN_L0 + 6,
		PIN_L7 = PIN_L0 + 7,
#endif
	};

	/**
	 * Map an absolute pin number to a base address and a bit offset that may
	 * be used as a left shift value. If the pin is not valid, neither the
	 * base variable nor the offset variable are modified.
	 * @param pin is an absolute pin number.
	 * @param mybase refers to a base address variable.
	 * @param myoffset refers to an offset variable.
	 * @return true if the pin is valid for this target, false otherwise.
	 */
	static bool map(uint8_t pin, volatile void * & mybase, uint8_t & myoffset);

	/**
	 * Map an absolute pin number into a volatile base address.
	 * @param pin is an absolute pin number.
	 * @return a volatile base address or NULL if invalid.
	 */
	static volatile void * base(uint8_t pin);

	/**
	 * Map an absolute pin number into a bit offset that can be used as a left
	 * shift value to generate a mask.
	 * @param pin is an absolute pin number.
	 * @return a bit offset or ~0 if invalid.
	 */
	static uint8_t offset(uint8_t pin);

	/**
	 * Map an absolute pin number to a eight-bit mask that is simply the offset
	 * of the same pin applied to a left shift operator.
	 * @param pin is an absolute pin number.
	 * @return an eight-bit mask or zero if invalid.
	 */
	static uint8_t mask(uint8_t pin);

};

inline volatile void * GPIO::base(uint8_t pin) {
	volatile void * mybase;
	return (map(pin, mybase, unused.u8) ? mybase : 0);
}

inline uint8_t GPIO::offset(uint8_t pin) {
	uint8_t myoffset;
	return (map(pin, unused.vvp, myoffset) ? myoffset : ~0);
}

inline uint8_t GPIO::mask(uint8_t pin) {
	uint8_t myoffset;
	return (map(pin, unused.vvp, myoffset) ? (1 << myoffset) : 0);
}

}
}
}

#endif /* _COM_DIAG_AMIGO_MEGAAVR_GPIO_H_ */
