/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "FreeRTOS.h"
#include "com/diag/amigo/target/Console.h"

extern "C" void vApplicationStackOverflowHook(void * pxTask, const char * pcTaskName);

extern "C" void vApplicationStackOverflowHook(void * pxTask, const char * pcTaskName) {
	com::diag::amigo::Console::instance().start().write_P(PSTR("\r\nSTACK: 0x")).dump(&pxTask, sizeof(pxTask)).write_P(PSTR(" \"")).write(pcTaskName).write_P(PSTR("\"!\r\n")).flush().stop();
}
