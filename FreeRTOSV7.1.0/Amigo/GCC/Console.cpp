/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * These are the functions that provide C-linkage access to Console for C
 * translation units. They are in a separate translation unit because they
 * don't need to be linked in unless they are referenced, and because they
 * have no architectural dependencies.
 */

#include "com/diag/amigo/target/Console.h"

CXXCAPI void amigo_console_start() {
	com::diag::amigo::Console::instance().start();
}

CXXCAPI void amigo_console_stop() {
	com::diag::amigo::Console::instance().stop();
}

CXXCAPI void amigo_console_write_char(uint8_t ch) {
	com::diag::amigo::Console::instance().write(ch);
}

CXXCAPI void amigo_console_write_string(const char * string) {
	com::diag::amigo::Console::instance().write(string);
}

CXXCAPI void amigo_console_write_string_P(PGM_P string) {
	com::diag::amigo::Console::instance().write_P(string);
}

CXXCAPI void amigo_console_write_data(const void * data, size_t size) {
	com::diag::amigo::Console::instance().write(data, size);
}

CXXCAPI void amigo_console_write_data_P(PGM_VOID_P data, size_t size) {
	com::diag::amigo::Console::instance().write_P(data, size);
}

CXXCAPI void amigo_console_dump(const void * data, size_t size) {
	com::diag::amigo::Console::instance().dump(data, size);
}

CXXCAPI void amigo_console_dump_P(PGM_VOID_P data, size_t size) {
	com::diag::amigo::Console::instance().dump_P(data, size);
}

CXXCAPI void amigo_console_flush() {
	com::diag::amigo::Console::instance().flush();
}

CXXCAPI void amigo_console_trace(char ch) {
	com::diag::amigo::Console::instance().start().write(ch).flush().stop();
}

