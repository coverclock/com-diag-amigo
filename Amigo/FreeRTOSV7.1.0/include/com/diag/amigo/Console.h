#ifndef _COM_DIAG_AMIGO_CONSOLE_H_
#define _COM_DIAG_AMIGO_CONSOLE_H_

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

class Console
{

public:

	explicit Console();

	virtual ~Console();

	Console & start();

	Console & stop();

	Console & write(uint8_t ch);

	Console & write(const char * string);

	Console & write(const void * data, size_t size);

	Console & flush();

private:

	uint8_t begin() const;

	void end(uint8_t ucsrb) const;

	void emit(uint8_t ch) const;

	void wait() const;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_CONSOLE_H_ */
