#ifndef _COM_DIAG_AMIGO_MEGAAVR_CONSOLE_H_
#define _COM_DIAG_AMIGO_MEGAAVR_CONSOLE_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/cxxcapi.h"
#include <avr/pgmspace.h>

#if defined(__cplusplus)

namespace com {
namespace diag {
namespace amigo {

class Console
{

public:

	static const uint32_t RATE = 115200UL;

	static Console & instance();

	explicit Console();

	virtual ~Console();

	Console & start(uint32_t rate = RATE);

	Console & stop();

	Console & write(uint8_t ch);

	Console & write(const char * string);

	Console & write_P(PGM_P string);

	Console & write(const void * data, size_t size);

	Console & write_P(PGM_VOID_P data, size_t size);

	Console & flush();

private:

	uint8_t ubrrl;
	uint8_t ubrrh;
	uint8_t ucsra;
	uint8_t ucsrb;
	uint8_t ucsrc;

	void emit(uint8_t ch);

};

}
}
}

#endif

CXXCAPI void amigo_console_start(uint32_t rate);

CXXCAPI void amigo_console_stop(void);

CXXCAPI void amigo_console_write_char(uint8_t ch);

CXXCAPI void amigo_console_write_string(const char * string);

CXXCAPI void amigo_console_write_string_P(PGM_P string);

CXXCAPI void amigo_console_write_data(const void * data, size_t size);

CXXCAPI void amigo_console_write_data_P(PGM_VOID_P data, size_t size);

CXXCAPI void amigo_console_flush(void);

#endif /* _COM_DIAG_AMIGO_MEGAAVR_CONSOLE_H_ */
