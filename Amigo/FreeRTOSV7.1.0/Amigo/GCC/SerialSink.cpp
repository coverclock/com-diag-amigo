/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/SerialSink.h"
#include "com/diag/amigo/arch/Serial.h"

namespace com {
namespace diag {
namespace amigo {

SerialSink::~SerialSink() {}

size_t SerialSink::write(uint8_t ch) {
	return serial->write(ch);
}

void SerialSink::flush() {
	serial->flush();
}

}
}
}
