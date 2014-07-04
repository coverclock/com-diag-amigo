#ifndef _COM_DIAG_PRINT_H_
#define _COM_DIAG_PRINT_H_

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

/**
 * Print implements a functor the implements a printf-style facility whose
 * output is directed to the specified Sink.
 */
class Print
{

public:

	/**
	 * Constructor.
	 * @param outputsink refers to the Sink.
	 * @param progmemformat if true indicates the format string is in program
	 * memory.
	 */
	Print(Sink & outputsink, bool progmemformat = false)
	: sink(&outputsink)
	, progmem(progmemformat)
	{}

	/**
	 * Destructor.
	 */
	~Print() {}

	/**
	 * Functor which formats a variable length argument list using the
	 * specified format string and writes the result to the Sink.
	 * @param format points to the format string.
	 * @return the number of bytes written to the Sink.
	 */
	size_t operator() (const char * format, ...);

protected:

	Sink * sink;
	bool progmem;

};

}
}
}

#endif /* _COM_DIAG_SINK_H_ */
