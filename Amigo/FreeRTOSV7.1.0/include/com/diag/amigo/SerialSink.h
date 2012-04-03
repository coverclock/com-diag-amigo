#ifndef _COM_DIAG_SERIALSINK_H_
#define _COM_DIAG_SERIALSINK_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/Sink.h"

namespace com {
namespace diag {
namespace amigo {

class Serial;

/**
 * SerialSink implements a Sink interface for a Serial object. This allows
 * a Serial object to be used in any application that expects a Sink. Print
 * is an example of such an application.
 */
class SerialSink
: public Sink
{

public:

	/**
	 * Constructor.
	 * @param myserial refers to the Serial object.
	 */
	explicit SerialSink(Serial & myserial)
	: serial(&myserial)
	{}

	/**
	 * Destructor.
	 */
	virtual ~SerialSink();

	/**
	 * Implements the Sink write method for the Serial object.
	 * @param ch is the byte to write.
	 * @return the number of byte written (nominally one) or zero for fail.
	 */
	virtual size_t write(uint8_t ch);

	/**
	 * Implements the Sink flush method for the Serial object.
	 */
	virtual void flush();

	using Sink::write;

protected:

	Serial * serial;

};

}
}
}

#endif /* _COM_DIAG_SERIALSINK_H_ */
