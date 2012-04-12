#ifndef _COM_DIAG_SINK_H_
#define _COM_DIAG_SINK_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <string.h>
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/target/harvard.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * Sink is a partly abstract type that can consume data a character at a time.
 * The deriving class defines write(uint8_t) and flush() operations. Sink
 * defines write(const void *, size_t) and write(const char *) operations on
 * top of those provided by the deriving class.
 */
class Sink
{

public:

	/**
	 * Constructor.
	 */
	explicit Sink() {}

	/**
	 * Destructor.
	 */
	virtual ~Sink();

	/**
	 * Write a single byte to the underlying implementation.
	 * @param ch is the byte to write.
	 * @return the number of byte written (nominally one) or zero for fail.
	 */
	virtual size_t write(uint8_t ch) = 0;

	/**
	 * Flush all pending data from the underlying implementation. What this
	 * means depends on the underlying implementation.
	 */
	virtual void flush() = 0;

	/**
	 * Write a sequence of contiguous bytes of a specified length from data
	 * space to the underlying implementation.
	 * @param datum points to a contiguous sequence of bytes to write.
	 * @param size is the number of bytes to write.
	 * @return the number of bytes written or zero for fail.
	 */
    size_t write(const void * datum, size_t size);

	/**
	 * Write a sequence of contiguous bytes of a specified length from program
	 * space to the underlying implementation.
	 * @param datum points to a contiguous sequence of bytes to write.
	 * @param size is the number of bytes to write.
	 * @return the number of bytes written or zero for fail.
	 */
    size_t write_P(PGM_VOID_P datum, size_t size);

    /**
     * Write a nul-terminated C string from data space to the underlying
     * implementation. The terminating nul is not written.
     * @param string points to the nul-terminated C string.
	 * @return the number of bytes written or zero for fail.
     */
    size_t write(const char * string) { return write(string, strlen(string)); }

    /**
     * Write a nul-terminated C string from program space to the underlying
     * implementation. The terminating nul is not written.
     * @param string points to the nul-terminated C string.
	 * @return the number of bytes written or zero for fail.
     */
    size_t write_P(PGM_P string) { return write_P(string, strlen_P(string)); }

};

}
}
}

#endif /* _COM_DIAG_SINK_H_ */
