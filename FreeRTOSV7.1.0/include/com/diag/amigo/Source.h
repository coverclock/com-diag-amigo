#ifndef _COM_DIAG_SOURCE_H_
#define _COM_DIAG_SOURCE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * Sink is a partly abstract type that can produce data a character at a time.
 * The deriving class defines available() and read() operations. Source defines
 * a read(void *, size_t) operation on top of those provided by the deriving
 * class.
 */
class Source
{

public:

	/**
	 * Constructor.
	 */
	explicit Source() {}

	/**
	 * Destructor.
	 */
	virtual ~Source();

	/**
	 * Return the number of bytes available from the underlying source
	 * without blocking, which may be zero.
	 * @return the number of characters available or <0 if fail.
	 */
	virtual int available() = 0;

	/**
	 * Return the next bytes from the underlying source.
	 * @return the next character or <0 if fail.
	 */
	virtual int read() = 0;

	/**
	 * Return a sequence of bytes from the underlying source of no more than
	 * the specified size, and store them in consecutive bytes in the provided
	 * buffer. The actual number of bytes read is returned; this can be any
	 * value less than or equal to size, including zero.
	 * @param buffer points to the buffer of at least size bytes.
	 * @param size specifies the maximum number of bytes to read.
	 * @return the actual number of bytes read.
	 */
	size_t read(void * buffer, size_t size);

};

}
}
}

#endif /* _COM_DIAG_SOURCE_H_ */
