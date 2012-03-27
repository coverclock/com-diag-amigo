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

namespace com {
namespace diag {
namespace amigo {

class Sink
{

public:

	virtual size_t write(uint8_t ch) = 0;

	virtual void flush() = 0;

    virtual size_t write(const void * datum, size_t size);

    size_t write(const char * string) { return write(string, strlen(string)); }

};

}
}
}

#endif /* _COM_DIAG_SINK_H_ */
