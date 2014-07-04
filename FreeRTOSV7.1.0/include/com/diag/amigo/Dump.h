#ifndef _COM_DIAG_DUMP_H_
#define _COM_DIAG_DUMP_H_

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
 * Dump implements a functor that dumps blocks of memory in printable
 * hexadecimal digits to the specified Sink. Note that dumping primitive
 * multi-byte data types like short, int, or long may yield reversed results
 * if the host (as in the case of the megaAVR) stores data in little-endian
 * byte order.
 */
class Dump
{

public:

	/**
	 * Constructor.
	 * @param outputsink refers to the Sink.
	 * @param progmemblock if true indicates the memory block is in program
	 * memory.
	 */
	Dump(Sink & outputsink, bool progmemblock = false)
	: sink(&outputsink)
	, progmem(progmemblock)
	{}

	/**
	 * Destructor.
	 */
	~Dump() {}

	/**
	 * Dump a memory block in printable hex digits to the Sink.
	 * @param data points to the memory block to be dumped.
	 * @param size is the size of the memory block in bytes.
	 * @return a reference to this Console object.
	 */
	size_t operator() (const void * data, size_t size);

protected:

	Sink * sink;
	bool progmem;

};

}
}
}

#endif /* _COM_DIAG_SINK_H_ */
