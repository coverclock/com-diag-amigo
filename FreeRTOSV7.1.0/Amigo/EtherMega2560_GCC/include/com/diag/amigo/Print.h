#ifndef _COM_DIAG_PRINT_H_
#define _COM_DIAG_PRINT_H_

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

class Print {

public:

	explicit Print(Sink & outputsink, bool progmemformat = false)
	: sink(&outputsink)
	, progmem(progmemformat)
	{
	}

	virtual ~Print() {
	}

	size_t operator() (const char * format, ...);

protected:

	Sink * sink;
	bool progmem;

};

}
}
}

#endif /* _COM_DIAG_SINK_H_ */
