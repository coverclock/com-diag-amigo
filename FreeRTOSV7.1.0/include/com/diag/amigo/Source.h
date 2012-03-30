#ifndef _COM_DIAG_SOURCE_H_
#define _COM_DIAG_SOURCE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

namespace com {
namespace diag {
namespace amigo {

class Source
{

public:

	virtual int available() = 0;

	virtual int read() = 0;

};

}
}
}

#endif /* _COM_DIAG_SOURCE_H_ */
