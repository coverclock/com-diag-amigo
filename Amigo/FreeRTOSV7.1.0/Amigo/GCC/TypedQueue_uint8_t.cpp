/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This file instantiates a template explicitly for when the GCC C++ option
 * -fno-implicit-templates is used.
 */

#include "com/diag/amigo/TypedQueue.h"

// Used by A2D, Serial, SPI.
template class com::diag::amigo::TypedQueue<uint8_t>;
