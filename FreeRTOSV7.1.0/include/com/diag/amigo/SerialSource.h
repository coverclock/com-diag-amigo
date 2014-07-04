#ifndef _COM_DIAG_SERIALSOURCE_H_
#define _COM_DIAG_SERIALSOURCE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/Source.h"

namespace com {
namespace diag {
namespace amigo {

class Serial;

/**
 * SerialSource implements a Source interface for a Serial object. This allows
 * a Serial object to be used in any application that expects a Sink. Print
 * is an example of such an application.
 */
class SerialSource
: public Source
{

public:

	/**
	 * Constructor.
	 * @param myserial refers to the Serial object.
	 */
	explicit SerialSource(Serial & myserial)
	: serial(&myserial)
	{}

	/**
	 * Destructor.
	 */
	virtual ~SerialSource();

	/**
	 * Implement the Source available method for the Serial object.
	 * @return the number of characters available or <0 if fail.
	 */
	virtual int available();

	/**
	 * Implement the Source read method for the Serial object.
	 * @return the next character or <0 if fail.
	 */
	virtual int read();

	using Source::read;

protected:

	Serial * serial;

};

}
}
}

#endif /* _COM_DIAG_SERIALSOURCE_H_ */
