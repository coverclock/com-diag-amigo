#ifndef _COM_DIAG_SERIALSINK_H_
#define _COM_DIAG_SERIALSINK_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Sink.h"

namespace com {
namespace diag {
namespace amigo {

class Serial;

class SerialSink
: public Sink
{

public:

	explicit SerialSink(Serial & myserial)
	: serial(&myserial)
	{}

	virtual ~SerialSink();

	virtual size_t write(uint8_t ch);

	virtual void flush();

	using Sink::write;

protected:

	Serial * serial;

};

}
}
}

#endif /* _COM_DIAG_SERIALSINK_H_ */
