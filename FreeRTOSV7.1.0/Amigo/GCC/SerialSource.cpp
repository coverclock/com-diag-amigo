/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/SerialSource.h"
#include "com/diag/amigo/target/Serial.h"

namespace com {
namespace diag {
namespace amigo {

SerialSource::~SerialSource() {}

int SerialSource::available() {
	return serial->available();
}

int SerialSource::read() {
	return serial->read();
}

}
}
}
