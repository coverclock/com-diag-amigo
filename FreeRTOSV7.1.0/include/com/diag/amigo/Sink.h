#ifndef _COM_DIAG_SINK_H_
#define _COM_DIAG_SINK_H_

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

class Sink {

public:

	explicit Sink() {}

	virtual ~Sink() {}

	virtual size_t operator() () = 0;

	virtual size_t operator() (uint8_t ch) = 0;

	size_t operator() (const void * dat, size_t size, bool progmem = false);

};

}
}
}

#endif /* _COM_DIAG_SINK_H_ */
