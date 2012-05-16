#ifndef _COM_DIAG_AMIGO_MEGAAVR_WATCHDOG_H_
#define _COM_DIAG_AMIGO_MEGAAVR_WATCHDOG_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 *
 * IMPORTANT SAFETY TIPS
 *
 * The functions in this translation unit are more experimental that what
 * I normally foist off onto the public. I recommend NOT using them. They are
 * definitely a work in progress, and the code below should be considered
 * "notes".
 *
 * I don't believe there's actually any way to safely use the AVR hardware
 * watch dog timer (WDT) with the standard Arduino bootloader. If the system
 * is reset via the WDT, the WDT is still enabled, but with the shortest
 * prescaler (zero), when it comes out of Watch Dog Reset. A prescaler of zero
 * allows for about 16ms before the WDT fires again; it must be disabled within
 * this time to prevent another watch dog reset from occurring. But the Arduino
 * bootloader has a time out of about a second (depending on the model) before
 * it transfers control to the application. This results in a "rolling reset".
 * I haven't been able to interrupt this even by power cycling the board (?!);
 * I had to resort to reflashing the board using the AVRISP mkII in-system
 * programmer device. Searching the web after this debacle, I found several
 * folks have figured this out before I did. The fix is to modify the Arduino
 * bootloader to disable the WDT very early in its own initialization.
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include "com/diag/amigo/cxxcapi.h"
#include "com/diag/amigo/types.h"
#include "com/diag/amigo/target/Uninterruptible.h"

/**
 * Disable the watch dog timer. NOT ACTUALLY USEFUL.
 * @return the prior value of the MCUSR indicating the prior RESET reason.
 */
CXXCINLINE uint8_t amigo_watchdog_disable(void) {
	com::diag::amigo::Uninterruptible uninterruptible;
	// This code snippet actually has to be done in the Arduino bootloader,
	// if it is being used, to be effective. Doing it in the application, which
	// is only run by the standard bootloader after a long (relatively speaking)
	// timeout, is way too late. It may be too late in whatever passes for
	// "main" in the boot loader. The AVR libc FAQ suggests this snippet which
	// puts the code in one of the early init sections in the run-time.
	//
	// #include <avr/wdt.h>
	// void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
	// void wdt_init(void) { wdt_reset(); MCUSR = 0; wdt_disable(); return; }
	//
	// Note that that still has to be in the bootloader; putting it in the
	// application is too late. Also note that if it is placed in the
	// bootloader, then the MCUSR value read by the application is useless
	// because the bootloader has already cleared it.
	wdt_reset();
	uint8_t reason = MCUSR;
	MCUSR = 0;
	wdt_disable();
	return reason;
}

/**
 * Transfer control to the system reset vector. This doesn't use the
 * hardware watch dog timer. The disadvantage to using this approach is that it
 * doesn't reset any of the hardware, leaving all the SoC controllers in states
 * that may be inconsistent with a system reset. DOES NOT WORK.
 */
CXXCINLINE void amigo_watchdog_restart(void) {
	com::diag::amigo::Uninterruptible uninterruptible;
#if defined(COM_DIAG_AMIGO_WATCHDOG_RESTART_USES_CALL)
	// I really really really thought this would work. The assembly code
	// code generated by the compiler looks right to me. But if it is
	// jumping to the reset vector, the system isn't restarting the way
	// I expect.
	(*static_cast<void(*)(void)>(0))();
#elif defined(__AVR_MEGA__) && __AVR_MEGA__
	// The assembly code generated by the compiler for this also looks right
	// to me. But it's not happening for this either. However this has the
	// benefit of requiring zero additional stack space.
	asm volatile ("jmp __vectors"); // GNU as treats all undefined symbols as .extern.
#else
	asm volatile ("rjmp __vectors"); // GNU as treats all undefined symbols as .extern.
#endif
}

/**
 * Enable the hardware watch dog timer. If not periodically patted, the hardware
 * watch dog will reset the target. This is a good way to deliberately reset
 * the target. The underlying implementation currently sets the watch dog
 * timeout to about eight seconds which is the maximum. IF YOU ARE NOT USING
 * A BOOTLOADER THAT DISABLES THE WATCHDOG TIMER, THIS WILL BRICK YOUR BOARD
 * BY SENDING IT INTO CONTINUOUS RESETS REQUIRING AN AVR ISP HARDWARE PROGRAMMER
 * TO RECOVER. THE STANDARD ARDUINO BOOTLOADERS DO NOT DISABLE THE WATCHDOG
 * TIMER.
 */
CXXCINLINE void amigo_watchdog_enable(void) {
	com::diag::amigo::Uninterruptible uninterruptible;
	// Note that the AVR libc function expects all four prescaler bits to be
	// contiguous, like they are portrayed in the data sheet table, not split
	// one and three as they actually are in the control register.
	//
	// Reference: Atmel, 8-bit Atmel Microcontroller, ATmega2560,
	//            2549N-AVR-05/11, 12.5.2, p. 67
	//
	// This nominally gives us about eight seconds before the WDT reset. That
	// gives the ATmega2560 to reset, the bootloader to time out and restart
	// our application, and the main program to disable the WDT.
	wdt_enable(WDTO_8S);
}

/**
 * Pat the watch dog timer by resetting the timer. NOT ACTUALLY USEFUL.
 */
CXXCINLINE void amigo_watchdog_reset(void) {
	com::diag::amigo::Uninterruptible uninterruptible;
	wdt_reset();
}

#if defined(__cplusplus)

namespace com {
namespace diag {
namespace amigo {
namespace watchdog {

/**
 * Disable the watch dog timer. NOT ACTUALLY USEFUL.
 * @return the prior value of the MCUSR indicating the prior RESET reason.
 */
inline uint8_t disable() {
	return amigo_watchdog_disable();
}

/**
 * Transfer control to the system reset vector. This doesn't actually use the
 * hardware watch dog timer. The disadvantage to using this approach is that it
 * doesn't reset any of the hardware, leaving all the SoC controllers in states
 * that may be inconsistent with a system reset. DOES NOT WORK.
 */
inline void restart() {
	amigo_watchdog_restart();
}

/**
 * Enable the hardware watch dog timer. If not periodically patted, the hardware
 * watch dog will reset the target. This is a good way to deliberately reset
 * the target. The underlying implementation currently sets the watch dog
 * timeout to about eight seconds which is the maximum. IF YOU ARE NOT USING
 * A BOOTLOADER THAT DISABLES THE WATCHDOG TIMER, THIS WILL BRICK YOUR BOARD
 * BY SENDING IT INTO CONTINUOUS RESETS REQUIRING AN AVR ISP HARDWARE PROGRAMMER
 * TO RECOVER. THE STANDARD ARDUINO BOOTLOADERS DO NOT DISABLE THE WATCHDOG
 * TIMER.
 */
inline void enable() {
	amigo_watchdog_enable();
}

/**
 * Pat the watch dog timer by resetting the timer. NOT ACTUALLY USEFUL.
 */
inline void reset() {
	amigo_watchdog_reset();
}

}
}
}
}

#endif

#endif /* _COM_DIAG_AMIGO_MEGAAVR_WATCHDOG_H_ */
