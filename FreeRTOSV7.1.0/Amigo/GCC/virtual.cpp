/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/cxxcapi.h"
#include "com/diag/amigo/fatal.h"

/**
 * When you declare a pure virtual function, such as in an abstract interface
 * class, C++ has to put some pointer into the vtable for that function in
 * in the implementation of that abstract class as a default. (Surprise! Pure
 * abstract interface classes have implementations!). It expects this function
 * to be available for that default. The standard libc++ library defines this
 * function, AVR libc does not. So we do it here. If it is ever called,
 * wackiness ensues.
 */
CXXCAPI void __cxa_pure_virtual() {
	com::diag::amigo::fatal(__FILE__, __LINE__);
}
