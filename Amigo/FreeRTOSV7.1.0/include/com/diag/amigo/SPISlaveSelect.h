#ifndef _COM_DIAG_AMIGO_SPISLAVESELECT_H_
#define _COM_DIAG_AMIGO_SPISLAVESELECT_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 * This code is gratefully inspired by FreeRTOS lib_spi and Arduino SPI.
 */

#include "com/diag/amigo/types.h"
#include "com/diag/amigo/target/GPIO.h"

namespace com {
namespace diag {
namespace amigo {

/**
 * SPISlaveSelect clears an active low GPIO slave select bit in its constructor
 * and sets the bit in its destructor. This allows Serial Peripheral Interface
 * (SPI) slave selection (usually abbreviated SS) to be handled via lexical
 * scoping. Its constructor uses a GPIO object and mask provided by the caller
 * because typically the application has already created these for other
 * reasons, for example because it has to configure the SS GPIO bit as an
 * output. Note that some SPI slave devices expect to be selected and deselected
 * on every individual operation, others support multiple operations between
 * selection and deselection. Your mileage will vary.
 */
class SPISlaveSelect {

public:

	SPISlaveSelect(GPIO & mygpio, uint8_t mymask)
	: gpio(&mygpio)
	, mask(mymask)
	{
		gpio->clear(mask); /* Active low. */
	}

	~SPISlaveSelect() {
		gpio->set(mask); /* Active low. */
	}

private:

	GPIO * gpio;
	uint8_t mask;

};

}
}
}

#endif /* _COM_DIAG_AMIGO_SPISLAVESELECT_H_ */
