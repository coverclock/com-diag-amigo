#ifndef _COM_DIAG_SOURCESINK_H_
#define _COM_DIAG_SOURCESINK_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/Source.h"
#include "com/diag/amigo/Sink.h"

namespace com {
namespace diag {
namespace amigo {

class SourceSink {

	virtual Source & source() = 0;

	virtual Sink & sink() = 0;

};

}
}
}

#endif /* _COM_DIAG_SOURCESINK_H_ */
