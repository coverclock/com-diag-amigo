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

class Source {

public:

	explicit Source() {}

	virtual ~Source() {}

	virtual int operator() (bool peek = false) = 0;

	size_t operator() (void * buf, size_t size);

};

}
}
}

#endif /* _COM_DIAG_SOURCE_H_ */
