/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * Reference: http://wiki.osdev.org/C%2B%2B
 */

#include "com/diag/amigo/cxxcapi.h"
#include "com/diag/amigo/fatal.h"
#include "com/diag/amigo/target/Uninterruptible.h"

/**
 * When you declare a pure virtual function, such as in an abstract interface
 * class, C++ has to put some pointer into the vtable for that function in
 * in the implementation of that abstract class as a default. (Surprise! Pure
 * abstract interface classes have implementations!). GCC expects this function
 * to be available for that default. The standard libc++ library defines this
 * function, AVR libc does not. So we do it here. It should never be called
 * unless something is seriously wrong, in which case wackiness ensues.
 */
CXXCAPI void __cxa_pure_virtual(void);

CXXCAPI void __cxa_pure_virtual(void) {
	com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
}

/**
 * This type defines a guard used by GCC for a flag that indicates whether
 * or not a static constructor has been called yet just in case it is running
 * in a multithreaded environment. We may assume that the compiler provides a
 * variable of this type and initializes it to zero to indicate that the
 * static constructor has not yet been called.
 */
__extension__ typedef int guard_t __attribute__((mode (__DI__)));

/**
 * GCC uses a kind of double check locking pattern to insure that static
 * constructors are called once and only once just in case it is running
 * in a multithreaded environment. This function returns C true
 * if the static constructor has not yet been called and hence the calling
 * thread must do so.
 * @param guard points to a guard object provided by the compiler.
 * @return non-zero if the static constructor needs to be called.
 */
CXXCAPI int __cxa_guard_acquire(guard_t * guard);

CXXCAPI int __cxa_guard_acquire(guard_t * guard) {
	com::diag::amigo::Uninterruptible ui;
	return !(*(char *)guard);
}

/**
 * GCC uses a kind of double check locking pattern to insure that static
 * constructors are called once and only once just in case it is running
 * in a multithreaded environment. This function is used to indicate that
 * the static constructor has been successfully called and hence the static
 * object has completed construction.
 * @param guard points to a guard object provided by the compiler.
 */
CXXCAPI void __cxa_guard_release(guard_t * guard);

CXXCAPI void __cxa_guard_release(guard_t * guard) {
	com::diag::amigo::Uninterruptible ui;
	*(char *)guard = 1;
}

/**
 * GCC uses a kind of double check locking pattern to insure that static
 * constructors are called once and only once just in case it is running
 * in a multithreaded environment. This function is used to indicate that
 * the static constructor failed (typically by throwing an exception) and
 * hence the static object has not been successfully constructed.
 * @param guard points to a guard object provided by the compiler.
 */
CXXCAPI void __cxa_guard_abort(guard_t * guard);

CXXCAPI void __cxa_guard_abort(guard_t * guard) {
}
