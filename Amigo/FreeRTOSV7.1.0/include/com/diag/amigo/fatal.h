#ifndef _COM_DIAG_AMIGO_FATAL_H_
#define _COM_DIAG_AMIGO_FATAL_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/cxxcapi.h"
#include "com/diag/amigo/target/harvard.h"

/**
 * This is the function from which nothing ever returns. It disables
 * interrupts, takes over the console serial port, prints a message if it can
 * using busy waiting, and infinite loops. This version can be called from
 * either C or C++ translation units.
 * @param file points to a file name in program space, typically PSTR(__FILE__).
 * @param line is a line number, typically __LINE__.
 */
CXXCAPI void amigo_fatal(PGM_P file, int line);

/**
 * This function prints a warning to the console serial port using busy waiting.
 * While the event it reports is not fatal, it is pretty serious, for example a
 * resource leak or failure in the underlying implementation. This version can
 * be called from either C or C++ translation units.
 * @param file points to a file name in program space, typically PSTR(__FILE__).
 * @param line is a line number, typically __LINE__.
 */
CXXCAPI void amigo_event(PGM_P file, int line);

#if defined(__cplusplus)

namespace com {
namespace diag {
namespace amigo {

/**
 * This is the function from which nothing ever returns. It disables
 * interrupts, takes over the console serial port, prints a message if it can
 * using busy waiting, and infinite loops.
 * @param file points to a file name in program space, typically PSTR(__FILE__).
 * @param line is a line number, typically __LINE__.
 */
inline void fatal(PGM_P file, int line) {
	amigo_fatal(file, line);
}

/**
 * This function prints a warning to the console serial port using busy waiting.
 * While the event it reports is not fatal, it is pretty serious, for example a
 * resource leak or failure in the underlying implementation. This version can
 * be called from either C or C++ translation units.
 * @param file points to a file name in program space, typically PSTR(__FILE__).
 * @param line is a line number, typically __LINE__.
 */
inline void event(PGM_P file, int line) {
	amigo_event(file, line);
}

}
}
}

#endif

#endif /* _COM_DIAG_AMIGO_FATAL_H_ */
