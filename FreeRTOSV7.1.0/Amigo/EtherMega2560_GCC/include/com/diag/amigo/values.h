#ifndef _COM_DIAG_AMIGO_VALUES_H_
#define _COM_DIAG_AMIGO_VALUES_H_

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

static const Ticks FOREVER = ~(static_cast<Ticks>(0));

static const Count UNLIMITED = ~(static_cast<Count>(0));

}
}
}

#endif /* _COM_DIAG_AMIGO_VALUES_H_ */
