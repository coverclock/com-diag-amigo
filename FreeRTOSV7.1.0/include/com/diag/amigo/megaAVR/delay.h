#ifndef _COM_DIAG_AMIGO_MEGAAVR_DELAY_H_
#define _COM_DIAG_AMIGO_MEGAAVR_DELAY_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This forces the <util/delay.h> header file on later versions of the tool
 * chain to define the old style delay that is less precise but allows a
 * variable argument. The "optimized" delay requires a constant, which is not
 * relevant to our interests. This header file must be included before any
 * other use, including in nested header files, of <util/delay.h>.
 */

#if !defined(__DELAY_BACKWARD_COMPATIBLE__)
#	define __DELAY_BACKWARD_COMPATIBLE__
#	define COM_DIAG_AMIGO_DELAY_BACKWARD_COMPATIBLE
#endif

#include <util/delay.h>

#if defined(COM_DIAG_AMIGO_DELAY_BACKWARD_COMPATIBLE)
#	undef __DELAY_BACKWARD_COMPATIBLE__
#endif

#endif /* _COM_DIAG_AMIGO_MEGAAVR_DELAY_H_ */
