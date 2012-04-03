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
#include "com/diag/amigo/target/harvard.h"

#if defined(__cplusplus)

namespace com {
namespace diag {
namespace amigo {

/**
 * Console implements a special debugging interface to the USART0 serial port
 * on a megaAVR device. It does not rely on any underlying FreeRTOS software,
 * it uses busy waiting instead of interrupts, and it is hard coded to talk
 * directly to the memory mapped I/O registers of the USART. This makes it
 * inappropriate for real-time applications, but extremely useful for low
 * level debugging. The API encourages the use of method chaining, which makes
 * writing debugging code much easier. Typically I would put the following
 * example all on one line.
 *
 * Console::instance()
 * .start()
 * .write("TASK=0x")
 * .dump(&task, sizeof(task))
 * .write("\r\n")
 * .flush()
 * .stop();
 *
 * There is also a CXXC API that allows Console to be used from C code. In the
 * following example, the default Console instance is used implicitly.
 *
 * amigo_console_start();
 * amigo_console_write_string("TASK=0x");
 * amigo_console_dump(&task, sizeof(task));
 * amigo_console_write_string("\r\n");
 * amigo_console_flush();
 * amigo_console_stop();
 */
class Console
{

public:

	/**
	 * This is the default serial rate in bits per second.
	 */
	static const uint32_t RATE = 115200UL;

	/**
	 * This returns a reference to a pre-built Console object.
	 */
	static Console & instance();

	/**
	 * Constructor.
	 */
	explicit Console();

	/**
	 * Destructor.
	 */
	virtual ~Console();

	/**
	 * Initialize USART0 to the specified baud rate, one start bit, eight data
	 * bits, no parity bit, and one stop bit (8n1). The prior contents of the
	 * USART0 memory mapped I/O registers are saved and restored by the stop()
	 * method in the hope that an interrupt driven Serial device driver might
	 * not notice that Console has taken over the serial port temporarily.
	 * @param rate is the desired serial rate in bits per second.
	 * @return a reference to this Console object.
	 */
	Console & start(uint32_t rate = RATE);

	/**
	 * Deinitialize USART0 by restoring the memory mapped I/O registers saved
	 * by start();
	 * @return a reference to this Console object.
	 */
	Console & stop();

	/**
	 * Display a character on USART0.
	 * @param ch is a character to be displayed.
	 * @return a reference to this Console object.
	 */
	Console & write(uint8_t ch);

	/**
	 * Display a nul-terminated C string in data space.
	 * @param string points to the string to be displayed.
	 * @return a reference to this Console object.
	 */
	Console & write(const char * string);

	/**
	 * Display a nul-terminated C string in program space.
	 * @param string points to the string to be displayed.
	 * @return a reference to this Console object.
	 */
	Console & write_P(PGM_P string);

	/**
	 * Display a fixed data block in data space.
	 * @param data points to the data block to be displayed.
	 * @param size is the size of the data block in bytes.
	 * @return a reference to this Console object.
	 */
	Console & write(const void * data, size_t size);

	/**
	 * Display a fixed data block in program space.
	 * @param data points to the data block to be displayed.
	 * @param size is the size of the data block in bytes.
	 * @return a reference to this Console object.
	 */
	Console & write_P(PGM_VOID_P data, size_t size);

	/**
	 * Dump a fixed data block in data space in printable hex digits.
	 * @param data points to the data block to be dumped.
	 * @param size is the size of the data block in bytes.
	 * @return a reference to this Console object.
	 */
	Console & dump(const void * data, size_t size);

	/**
	 * Dump a fixed data block in program space in printable hex digits.
	 * @param data points to the data block to be dumped.
	 * @param size is the size of the data block in bytes.
	 * @return a reference to this Console object.
	 */
	Console & dump_P(PGM_VOID_P data, size_t size);

	/**
	 * Wait until it is likely that all requested data has been displayed.
	 * Deinitializing USART0 before all the data has been displayed can
	 * result in the last character or two being lost. This method should
	 * be called prior to calling stop(). stop() does not automatically perform
	 * a flush() since there may be circumstances in which the delay in flush()
	 * is undesirable or unnecessary.
	 * @return a reference to this Console object.
	 */
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

/**
 * Initialize USART0 to 115200 bits per second, one start bit, eight data
 * bits, no parity bit, and one stop bit (8n1). The prior contents of the
 * USART0 memory mapped I/O registers are saved and restored by the stop()
 * method in the hope that an interrupt driven Serial device driver might
 * not notice that Console has taken over the serial port temporarily.
 */
CXXCAPI void amigo_console_start();

/**
 * Deinitialize USART0 by restoring the memory mapped I/O registers saved
 * by start();
 */
CXXCAPI void amigo_console_stop(void);

/**
 * Display a character on USART0.
 * @param ch is a character to be displayed.
 */
CXXCAPI void amigo_console_write_char(uint8_t ch);

/**
 * Display a nul-terminated C string in data space.
 * @param string points to the string to be displayed.
 */
CXXCAPI void amigo_console_write_string(const char * string);

/**
 * Display a nul-terminated C string in program space.
 * @param string points to the string to be displayed.
 */
CXXCAPI void amigo_console_write_string_P(PGM_P string);

/**
 * Display a fixed data block in data space.
 * @param data points to the data block to be displayed.
 * @param size is the size of the data block in bytes.
 */
CXXCAPI void amigo_console_write_data(const void * data, size_t size);

/**
 * Display a fixed data block in program space.
 * @param data points to the data block to be displayed.
 * @param size is the size of the data block in bytes.
 */
CXXCAPI void amigo_console_write_data_P(PGM_VOID_P data, size_t size);

/**
 * Dump a fixed data block in data space in printable hex digits.
 * @param data points to the data block to be dumped.
 * @param size is the size of the data block in bytes.
 */
CXXCAPI void amigo_console_dump(const void * data, size_t size);

/**
 * Dump a fixed data block in program space in printable hex digits.
 * @param data points to the data block to be dumped.
 * @param size is the size of the data block in bytes.
 */
CXXCAPI void amigo_console_dump_P(PGM_VOID_P data, size_t size);

/**
 * Wait until it is likely that all requested data has been displayed.
 * Deinitializing USART0 before all the data has been displayed can
 * result in the last character or two being lost. This method should
 * be called prior to calling stop(). stop() does not automatically perform
 * a flush() since there may be circumstances in which the delay in flush()
 * is undesirable or unnecessary.
 */
CXXCAPI void amigo_console_flush(void);

#endif /* _COM_DIAG_AMIGO_MEGAAVR_CONSOLE_H_ */
