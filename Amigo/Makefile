################################################################################
# Copyright 2012 by the Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h (basically LGPL 2.1)
# Chip Overclock <coverclock@diag.com>
# http://www.diag.com/navigation/downloads/Amigo
#
# USUAL DISCLAIMERS
#
#	BECAUSE THE SOFTWARE IS LICENSED FREE OF CHARGE, THERE IS NO
#	WARRANTY FOR THE SOFTWARE, TO THE EXTENT PERMITTED BY APPLICABLE
#	LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
#	HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS" WITHOUT
#	WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#	BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
#	AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO THE
#	QUALITY AND PERFORMANCE OF THE SOFTWARE IS WITH YOU.  SHOULD THE
#	SOFTWARE PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY
#	SERVICING, REPAIR OR CORRECTION.
#	
#	IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN
#	WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY
#	MODIFY AND/OR REDISTRIBUTE THE SOFTWARE AS PERMITTED ABOVE,
#	BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL,
#	INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR
#	INABILITY TO USE THE SOFTWARE (INCLUDING BUT NOT LIMITED TO LOSS
#	OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED
#	BY YOU OR THIRD PARTIES OR A FAILURE OF THE LIBRARY TO OPERATE
#	WITH ANY OTHER SOFTWARE), EVEN IF SUCH HOLDER OR OTHER PARTY
#	HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
#
# TESTED CONFIGURATIONS
#
#	BUILD_TARGET		FreetronicsEtherMega2560
#	BUILD_HOST			Darwin (a.k.a. Mac OS X 10.6.8)
#	BUILD_PLATFORM		UnitTest
#
#	BUILD_TARGET		FreetronicsEtherMega2560
#	BUILD_HOST			Linux (a.k.a. Ubuntu 10.04)
#	BUILD_PLATFORM		UnitTest
#
#	BUILD_TARGET		ArduinoMegaADK (with Ethernet Shield)
#	BUILD_HOST			Darwin (a.k.a. Mac OS X 10.6.8)
#	BUILD_PLATFORM		UnitTest
#
#	BUILD_TARGET		FreetronicsEtherTen
#	BUILD_HOST			Darwin (a.k.a. Mac OS X 10.6.8)
#	BUILD_PLATFORM		UnitTest (with multitasking but extremely cut down)
#
# TYPICAL MAKE TARGETS
#
#	clean			- remove artifacts
#	depend			- generate dependencies
#	all				- generate artifacts and deliverables (this is the default)
#	upload			- upload into target using tool chain avrdude utility
#	screen			- connect to target serial port using Mac screen utility
#	connect			- connect to target Ethernet port using Linux netcat utility
#
# ANCILLARY MAKE TARGETS
#
#	full			- generate artifacts, deliverables, and collateral
#	pristine		- remove deliverables and collateral
#	documentation	- generate documentation using Doxygen
#	flash			- reload bootloader using AVR ISP
#
# EXPERIMENTAL MAKE TARGETS
#
#	experimental-bootloader	- build modified STK500V2 bootloader for ATmega2560
#	experimental-flash		- load modified STK500V2 bootloader onto ATmega2560
#	experimental-patch		- make patch file of STK500V2 bootloader changes
#	experimental-apply		- apply patch file to unmodified STK500V2 bootloader
#
# COMPILE TIME OPTIONS
#
#	-DCOM_DIAG_AMIGO_PANIC_USES_ENABLE
#
#	This compiles amigo_panic() to use amigo_watchdog_enable() to restart
#	the microcontroller. This may be enabled if and only if you are running
#	a bootloader that correctly disables the hardware watchdog timer in its
#	early run-time initialization. Using this feature otherwise will brick
#	you board, requiring an AVR ISP hardware tool to recover. (This feature has
#	been tested and verified to work correctly with a modified bootloader. A
#	modified stk500v2 bootloader for the ATmega2560 microcontroller is included
#	in this software distribution, but you'll need an AVR ISP programming tool
#	to install it.)
#
#	-DCOM_DIAG_AMIGO_PANIC_USES_RESTART
#
#	This compiles amigo_panic() to use amigo_watchdog_restart() to restart
#	the software on the microcontroller. This really isn't a good solution
#	and seldom works correctly without some effort on the part of the
#	application prior to calling it.
#
#	-DCOM_DIAG_AMIGO_UNEXPECTED_USES_DEFAULT
#
#	This compiles amigo_unexpected() to use a single interrupt service routine
#	to handle all unexpected interrupts. This saves some flash space, but means
#	you can never know through what vector you received the unexpected interrupt
#	(and hence what driver is misbehaving). The alternative is to create an
#	interrupt service routine for each unused interrupt vector, which the code
#	does automatically.
#
#	-DCOM_DIAG_AMIGO_USES_PREDEFINED_SSIZE_T
#
#	This causes "com/diag/amigo/types.h" not to define its own ssize_t type.
#	Although ssize_t is normally defined on POSIX systems, the AVR libc and
#	GNU header files do not define it, so normally Amigo does so.
#
#	-DCOM_DIAG_AMIGO_WATCHDOG_RESTART_USES_CALL
#
#	This compiles amigo_watchdog_restart() to use a function call to transfer
#	control to the reset vector instead of just jumping to the vector address.
#	There probably is little reason ever to prefer this to the default behavior.
#
# IMPORTANT SAFETY TIP
#
#	Amigo has been built and tested with the following GCC tool chains and AVR
#	C libaries.
#
#		HOST			PACKAGE				GCC			LIBC
#		Mac OS X		AVR CrossPack		4.5.1		1.8.0
#		Windows 7		AVR Studio 5.1		4.5.1		1.7.1
#		Ubuntu 10.04	WrightFlyer Debian	4.5.1		1.8.0
#
#	The standard Ubuntu AVR packages available via Synaptics are too old (GCC
#	4.3.4, LIBC 1.6.7) and WILL NOT WORK!
#
#	Amigo has been built and tested tested with the following hardware.
#
#		VENDOR			BOARD			CONTROLLER
#		Freetronics		EtherMega2560	ATmega2560
#		Arduino			Mega ADK		ATmega2560
#		Freetronics		EtherTen		ATmega328p
#
#	The ATmega328p is so resource constrained that while I was able to get
#	FreeRTOS running on it, I had to cut back the unit test suite to the bare
#	minimum necessary just to insure that a task was actually running. I don't
#	consider the 328p to be a practical target on which to use Amigo. You will
#	spend all of your precious time trying to save one byte of SRAM while
#	fiddling with the relative sizes of the heap, the stacks, and the data
#	segment.
#
################################################################################

PROJECT=amigo
TITLE=Amigo
NAME=Amigo

MAJOR=4
MINOR=10
FIX=1

HTTP_URL=http://www.diag.com/navigation/downloads/$(TITLE).html
FTP_URL=ftp://www.diag.com/ftp/$(PROJECT)-$(MAJOR).$(MINOR).$(FIX).tgz
SVN_REPO=svn://graphite/$(PROJECT)
SVN_URL=$(SVN_REPO)/trunk/$(TITLE)
GIT_URL=https://github.com/coverclock/com-diag-$(PROJECT).git

################################################################################
# PARAMETERS
################################################################################

#BUILD_TARGET=FreetronicsEtherTen
#BUILD_TARGET=ArduinoMegaADK
BUILD_TARGET=FreetronicsEtherMega2560
BUILD_HOST=$(shell uname -s)
BUILD_PLATFORM=UnitTest

# What works here depends on what USB port you plug the Arduino cable into.
# Your mileage will absolutely vary.
#SERIAL=/dev/tty.usbmodem26421
#SERIAL=/dev/tty.usbmodem411
#SERIAL=/dev/tty.usbmodem441
SERIAL=/dev/tty.usbmodem621
BAUD=115200
FORMAT=cs8

# These values are used by the Unit Test suite. Amigo doesn't currently support
# DHCP (although that is a very good idea) so we have to configure it with
# a static IPV4 address. "make connect" will try to talk to the target at this
# address using the netcat utility (if you have it) over port 23 (TELNET)
# for the Socket server unit test. You can use TELNET as well, although it sends
# a lot of terminal-setup stuff that the Unit Test won't understand. The target
# will try to talk to the specified web server during the Socket client unit
# test. Amigo currently doesn't support DNS lookups (although that is a very
# good idea) so we have to configure it with the IPV4 address of the web server.
TARGET_MACADDRESS=90:a2:da:0d:03:4c
TARGET_IPADDRESS=192.168.1.253
TARGET_IPGATEWAY=192.168.1.1
TARGET_IPSUBNET=255.255.255.0
TARGET_WEBSERVER=$(shell eval "host arduino.cc | head -1 | cut -d\  -f 4")

################################################################################
# HOST
################################################################################

# This option uses the tool chain from the AVR CrossPack for my desktop Mac.
# http://www.obdev.at/downloads/crosspack/CrossPack-AVR-20120217.dmg
ifeq ($(BUILD_HOST), Darwin)
TMP_DIR=/tmp
FREERTOS_DIR=FreeRTOSV7.1.0
ARDUINO_DIR=/Applications/Arduino.app/Contents/Resources/Java
BOOTLOADER_DIR=$(ARDUINO_DIR)/hardware/Arduino/bootloaders
TOOLCHAIN_ROOT=/usr/local/CrossPack-AVR
AVRDUDE_CONF=$(TOOLCHAIN_ROOT)/etc/avrdude.conf
TOOLCHAIN_BIN=$(TOOLCHAIN_ROOT)/bin
TOOLCHAIN_DIR=$(TOOLCHAIN_ROOT)/avr/include
AVRSTUDIO_DIR=Amigo/Debug
SIZEFORMAT=-C --mcu=$(CONTROLLER)
endif

# This option uses the tool chain from the WrightFlyer debian package for Linux/GNU.
# http://www.wrightflyer.co.uk/avr-gcc/avr-gcc-4.5.1-avrfreaks-2011-dec-29-u10.04.i386.deb
# dpkg -i avr-gcc-4.5.1-avrfreaks-2011-dec-29-u10.04.i386.deb
ifeq ($(BUILD_HOST), Linux)
TMP_DIR=/tmp
FREERTOS_DIR=FreeRTOSV7.1.0
ARDUINO_DIR=$(HOME)/src/arduino-1.0-linux
BOOTLOADER_DIR=$(ARDUINO_DIR)/hardware/arduino/bootloaders
TOOLCHAIN_ROOT=/usr/local/avr
AVRDUDE_CONF=$(TOOLCHAIN_ROOT)/etc/avrdude.conf
TOOLCHAIN_BIN=$(TOOLCHAIN_ROOT)/bin
TOOLCHAIN_DIR=$(TOOLCHAIN_ROOT)/avr/include
AVRSTUDIO_DIR=Amigo/Debug
SIZEFORMAT=-C --mcu=$(CONTROLLER)
endif

################################################################################
# TARGET
################################################################################

ifeq ($(BUILD_TARGET),FreetronicsEtherMega2560)
ARCH=avr
RELAX=-mrelax
CROSS_COMPILE=$(ARCH)-
FAMILY=avr6
CONTROLLER=atmega2560
FREQUENCY=16000000L
ARDUINO=100
CORE=arduino
VARIANT=mega
TARGET=megaAVR
TOOLCHAIN=GCC
BOARD=$(BUILD_TARGET)
CONFIG=stk500v2
PROGRAMMER=avrispmkII
ISP=stk500v2
PART=m2560
BOOTLOADER_HEX=$(BOOTLOADER_DIR)/stk500v2/stk500boot_v2_mega2560.hex
EFUSE=0xFD
HFUSE=0xD8
LFUSE=0xFF
endif

ifeq ($(BUILD_TARGET),ArduinoMegaADK)
ARCH=avr
RELAX=-mrelax
CROSS_COMPILE=$(ARCH)-
FAMILY=avr6
CONTROLLER=atmega2560
FREQUENCY=16000000L
ARDUINO=100
CORE=arduino
VARIANT=mega
TARGET=megaAVR
TOOLCHAIN=GCC
BOARD=$(BUILD_TARGET)
CONFIG=stk500v2
PROGRAMMER=avrispmkII
ISP=stk500v2
PART=m2560
BOOTLOADER_HEX=$(BOOTLOADER_DIR)/stk500v2/stk500boot_v2_mega2560.hex
EFUSE=0xFD
HFUSE=0xD8
LFUSE=0xFF
endif

ifeq ($(BUILD_TARGET),FreetronicsEtherTen)
ARCH=avr
RELAX=
CROSS_COMPILE=$(ARCH)-
FAMILY=avr5
CONTROLLER=atmega328p
FREQUENCY=16000000L
ARDUINO=100
CORE=arduino
VARIANT=standard
TARGET=megaAVR
TOOLCHAIN=GCC
BOARD=$(BUILD_TARGET)
CONFIG=arduino
PROGRAMMER=avrispmkII
ISP=stk500v2
PART=m328p
BOOTLOADER_HEX=$(BOOTLOADER_DIR)/optiboot/optiboot_atmega328.hex
EFUSE=0x05
HFUSE=0xDE# ?0xD6 EESAVE?
LFUSE=0xFF
endif

################################################################################
# TOOLS
################################################################################

AVRDUDE=avrdude
CC=gcc
CXX=g++
LD=gcc
NM=nm
OBJCOPY=objcopy
OBJDUMP=objdump
SIZE=size

################################################################################
# DEFAULT
################################################################################

PHONY+=default

default:	all

################################################################################
# PLATFORM
################################################################################

# Amigo FreeRTOS-specific files
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/BinarySemaphore.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/CountingSemaphore.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/MutexSemaphore.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/overflow.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/Queue.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/Task.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/Timer.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/TypedQueue_uint16_t.cpp# for A2D when -fno-implicit-templates
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/TypedQueue_uint8_t.cpp# for A2D, Serial, SPI when -fno-implicit-templates

# Amigo megaAVR-specific files
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/$(TARGET)/A2D.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/$(TARGET)/Console.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/$(TARGET)/GPIO.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/$(TARGET)/Morse.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/$(TARGET)/Serial.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/$(TARGET)/SPI.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/$(TARGET)/PWM.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/$(TARGET)/unexpected.cpp

# Amigo W5100-specific files
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/W5100/Socket.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/W5100/W5100.cpp

# Amigo files
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/Console.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/Dump.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/fatal.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/heap.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/IPV4Address.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/MACAddress.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/Print.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/SerialSink.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/SerialSource.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/Sink.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/Socket.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/Source.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/unused.cpp
AMIGO_CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(TOOLCHAIN)/virtual.cpp

AMIGO_HDIRECTORIES+=$(FREERTOS_DIR)/include

PREREQUISITES+=$(FREERTOS_DIR)/include/com/diag/amigo/target
PREREQUISITES+=$(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h

# FreeRTOS files
FREERTOS_CFILES+=$(FREERTOS_DIR)/Source/portable/$(TOOLCHAIN)/$(TARGET)/port.c
FREERTOS_CFILES+=$(FREERTOS_DIR)/Source/croutine.c
FREERTOS_CFILES+=$(FREERTOS_DIR)/Source/list.c
FREERTOS_CFILES+=$(FREERTOS_DIR)/Source/queue.c
FREERTOS_CFILES+=$(FREERTOS_DIR)/Source/tasks.c
FREERTOS_CFILES+=$(FREERTOS_DIR)/Source/timers.c

FREERTOS_HDIRECTORIES+=$(FREERTOS_DIR)/Source/portable/$(TOOLCHAIN)/$(TARGET)
FREERTOS_HDIRECTORIES+=$(FREERTOS_DIR)/Source/include

ifeq ($(BUILD_PLATFORM), UnitTest)
CFILES+=$(FREERTOS_CFILES)
CFILES+=$(FREERTOS_DIR)/Source/portable/MemMang/heap_2.c
CXXFILES+=$(AMIGO_CXXFILES)
CXXFILES+=$(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/main.cpp
HDIRECTORIES+=$(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)
HDIRECTORIES+=$(AMIGO_HDIRECTORIES)
HDIRECTORIES+=$(FREERTOS_HDIRECTORIES)
endif

INCLUDES+=$(addprefix -I,$(HDIRECTORIES))

LIBRARIES+=-lm

################################################################################
# OPTIONS
################################################################################

OPT=s
CARCH=-mmcu=$(CONTROLLER)
DIALECT=-fno-exceptions -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CDIALECT=-std=gnu99 $(DIALECT)
CXXDIALECT=-fno-rtti -fno-implicit-templates $(DIALECT)
CDEBUG=-g
CWARN=-Wall
CPPFLAGS=$(CARCH) -DF_CPU=$(FREQUENCY) -DARDUINO=$(ARDUINO) $(INCLUDES)
CFLAGS=$(CDIALECT) $(CDEBUG) -O$(OPT) $(CWARN) $(CEXTRA)
CXXFLAGS=$(CXXDIALECT) $(CDEBUG) -O$(OPT) $(CWARN) $(CXXEXTRA)
LDFLAGS=$(CARCH) $(RELAX) -O$(OPT) -Wl,--gc-sections
NMFLAGS=-n -o -a -A
OBJDUMPFLAGS=-x -G -t -r
SIZEFLAGS=$(SIZEFORMAT)
OBJDISASSEMBLYFLAGS=-d
OBJCOPYEEPFLAGS=-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 
OBJCOPYHEXFLAGS=-O ihex -R .eeprom

################################################################################
# BUILD
################################################################################

OFILES+=$(addsuffix .o,$(basename $(CFILES) $(CXXFILES)))

EFILES+=$(addsuffix .e,$(basename $(CFILES) $(CXXFILES)))

SFILES+=$(addsuffix .s,$(basename $(CFILES) $(CXXFILES)))

ARTIFACTS+=$(OFILES)
ARTIFACTS+=$(SFILES)
ARTIFACTS+=$(EFILES)

ARTIFACTS+=$(BUILD_PLATFORM).elf# ELF output from linker
ARTIFACTS+=$(BUILD_PLATFORM).hex# Intel hex file for bootloading
ARTIFACTS+=$(BUILD_PLATFORM).eep# EEPROM file
ARTIFACTS+=$(BUILD_PLATFORM).map# ELF link map
ARTIFACTS+=$(BUILD_PLATFORM).dmp# AVR dump
ARTIFACTS+=$(BUILD_PLATFORM).dis# AVR disassembly
ARTIFACTS+=$(BUILD_PLATFORM).siz# AVR size

DELIVERABLES+=$(BUILD_PLATFORM).hex

COLLATERAL+=$(BUILD_PLATFORM).map
COLLATERAL+=$(BUILD_PLATFORM).dmp
COLLATERAL+=$(BUILD_PLATFORM).siz

$(BUILD_PLATFORM).elf:	$(OFILES)
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -o $@ $(OFILES) $(OBJECTS) $(ARCHIVES) $(LIBRARIES)

$(FREERTOS_DIR)/include/com/diag/amigo/target:	$(FREERTOS_DIR)/include/com/diag/amigo/$(TARGET)
	( cd $(FREERTOS_DIR)/include/com/diag/amigo; ln -f -s $(TARGET) target )

PHONY+=parameters

parameters $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h:
	cp /dev/null $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h
	echo "#define COM_DIAG_AMIGO_VINTAGE \"$(MAJOR).$(MINOR).$(FIX)\"" >> $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h
	echo "#define COM_DIAG_AMIGO_TARGET \"$(BUILD_TARGET)\"" >> $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h
	echo "#define COM_DIAG_AMIGO_HOST \"$(BUILD_HOST)\"" >> $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h
	echo "#define COM_DIAG_AMIGO_PLATFORM \"$(BUILD_PLATFORM)\"" >> $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h
	echo "#define COM_DIAG_AMIGO_MACADDRESS \"$(TARGET_MACADDRESS)\"" >> $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h
	echo "#define COM_DIAG_AMIGO_IPADDRESS \"$(TARGET_IPADDRESS)\"" >> $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h
	echo "#define COM_DIAG_AMIGO_IPSUBNET \"$(TARGET_IPSUBNET)\"" >> $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h
	echo "#define COM_DIAG_AMIGO_IPGATEWAY \"$(TARGET_IPGATEWAY)\"" >> $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h
	echo "#define COM_DIAG_AMIGO_WEBSERVER \"$(TARGET_WEBSERVER)\"" >> $(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h
	
ARTIFACTS+=$(FREERTOS_DIR)/Demo/$(TOOLCHAIN)/$(BOARD)/$(BUILD_PLATFORM)/unittest.h

################################################################################
# DEPENDENCIES
################################################################################

PHONY+=depend
ARTIFACTS+=$(BUILD_PLATFORM).mk

depend:	$(PREREQUISITES)
	cp /dev/null $(BUILD_PLATFORM).mk
	for F in $(CFILES); do \
		D=`dirname $$F`; \
		B=`basename $$F | sed 's/\..*$$//'`; \
		$(CROSS_COMPILE)$(CC) $(CPPFLAGS) -MM -MT $$D/$$B.o -MG $$F >> $(BUILD_PLATFORM).mk; \
	done
	for F in $(CXXFILES); do \
		D=`dirname $$F`; \
		B=`basename $$F | sed 's/\..*$$//'`; \
		$(CROSS_COMPILE)$(CXX) $(CPPFLAGS) -MM -MT $$D/$$B.o -MG $$F >> $(BUILD_PLATFORM).mk; \
	done

-include $(BUILD_PLATFORM).mk

################################################################################
# DISTRIBUTION
################################################################################

PHONY+=dist
ARTIFACTS+=$(PROJECT)-$(MAJOR).$(MINOR).$(FIX).tgz

dist $(PROJECT)-$(MAJOR).$(MINOR).$(FIX).tgz:
	TARDIR=$(shell mktemp -d $(TMP_DIR)/$(PROJECT).XXXXXXXXXX); \
	svn export $(SVN_URL) $$TARDIR/$(PROJECT)-$(MAJOR).$(MINOR).$(FIX); \
	tar -C $$TARDIR -cvzf - $(PROJECT)-$(MAJOR).$(MINOR).$(FIX) > $(PROJECT)-$(MAJOR).$(MINOR).$(FIX).tgz; \
	rm -rf $$TARDIR

PHONY+=commit dcommit push init

commit:
	git commit .

dcommit:
	git svn dcommit

push:
	git push origin master

init:
	git svn show-ignore > .gitignore
	git add .gitignore
	git remote add origin $(GIT_URL)

################################################################################
# DOCUMENTATION
################################################################################

DOC_DIR=doc
SED=sed
DOXYGEN=doxygen
TROFF=groff
PS2PDF=ps2pdf
BROWSER=firefox

PHONY+=documentation browse refman manpages

PREREQUISITES+=$(DOC_DIR)

documentation:
	mkdir -p $(DOC_DIR)/latex $(DOC_DIR)/man $(DOC_DIR)/pdf
	$(SED) -e "s/\\\$$Name.*\\\$$/$(MAJOR).$(MINOR).$(FIX)/" < doxygen.cf > doxygen-local.cf
	$(DOXYGEN) doxygen-local.cf
	( cd $(DOC_DIR)/latex; $(MAKE) refman.pdf; cp refman.pdf ../pdf )
	cat $(DOC_DIR)/man/man3/*.3 | $(TROFF) -man -Tps - > $(DOC_DIR)/pdf/manpages.ps
	$(PS2PDF) $(DOC_DIR)/pdf/manpages.ps $(DOC_DIR)/pdf/manpages.pdf

browse:
	$(BROWSER) file:doc/html/index.html

refman:
	$(BROWSER) file:doc/pdf/refman.pdf

manpages:
	$(BROWSER) file:doc/pdf/manpages.pdf

################################################################################
# UTILITIES
################################################################################

PHONY+=parameters verify backup bundle path implicit

verify:	$(CFILES) $(CXXFILES) $(HDIRECTORIES)

backup:
	DIRNAME="$(shell basename $(shell pwd))"; \
	tar -C .. -cvzf - $$DIRNAME > ../$$DIRNAME-$(shell date -u +'%Y%m%d%H%M%S%N%Z').tgz
	
bundle:
	DIRNAME="$(shell basename $(shell pwd))"; \
	tar -C .. --exclude .svn -cvzf - $$DIRNAME/$(FREERTOS_DIR) > ../$$DIRNAME-$(shell date -u +'%Y%m%d').tgz

# Using back quotes like `make path` on the command line sets the PATH variable
# in your interactive shell.

path:
	@echo export PATH=$(PATH):$(TOOLCHAIN_BIN)

# This is because I'm curious.

implicit:
	$(CROSS_COMPILE)$(CXX) -dM -E -mmcu=$(CONTROLLER) - < /dev/null

PHONY+=preprocess preassemble

preprocess:	$(EFILES)

preassemble:	$(SFILES)

# These targets only exist when running on my Mac desktop because that's to what
# the Arduino boards connects via USB. The Linux server is two floors down!

ifeq ($(BUILD_HOST), Darwin)
	
PHONY+=interrogate screen upload flash debug

# The Arduino bootloaders on both the Uno (optiboot) and the Mega (stk500)
# only pretend to implement the avrdude commands to query the signature bytes
# from the EEPROM. But in fact the bootloaders return hardcoded values, not the
# actual values from the chip. If queried for other chip values like the
# fuses, they return hardcoded zeros. If you want to play with the chip,
# you'll need an In-System Programmer (ISP) device like the Atmel AVRISP mkII,
# which is what I use.

# This uses the AVRISP mkII to start an interactive session.
interrogate:
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(PROGRAMMER) -Pusb -t

# To exit out of screen: control-a control-\ y.
screen:
	stty sane dec
	stty -f $(SERIAL) hupcl
	# control-A control-\ y to exit.
	screen -L $(SERIAL) $(BAUD) $(FORMAT)
	stty sane dec

# This uses the bootloader to load the application.
upload:	$(BUILD_PLATFORM).hex
	stty -f $(SERIAL) hupcl
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(CONFIG) -P$(SERIAL) -b$(BAUD) -D -Uflash:w:$<:i

# This is just like upload but does not rebuild the application.
upload2:
	stty -f $(SERIAL) hupcl
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(CONFIG) -P$(SERIAL) -b$(BAUD) -D -Uflash:w:$(BUILD_PLATFORM).hex:i

# This is just like upload2 but uses the AVRISP mkII to do the upload.
upload3:
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(ISP) -Pusb -D -Uflash:w:$(BUILD_PLATFORM).hex:i

# This uses the bootloader to load the debug image produced by AVR Studio 5.1.
debug:	$(AVRSTUDIO_DIR)/$(NAME).hex
	stty -f $(SERIAL) hupcl
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(CONFIG) -P$(SERIAL) -b$(BAUD) -D -Uflash:w:$(AVRSTUDIO_DIR)/$(NAME).hex:i
	
# This uses the AVRISP mkII to unlock the bootloader, initialize all fuses to
# Arduino defaults, reflash the bootloader, and relock the bootloader.
flash:	$(BOOTLOADER_HEX)
	$(AVRDUDE) -C$(AVRDUDE_CONF) -p$(PART) -c$(ISP) -b$(BAUD) -Pusb -e -Ulock:w:0x3F:m -Uefuse:w:$(EFUSE):m -Uhfuse:w:$(HFUSE):m -Ulfuse:w:$(LFUSE):m
	$(AVRDUDE) -C$(AVRDUDE_CONF) -p$(PART) -c$(ISP) -b$(BAUD) -Pusb -Uflash:w:$(BOOTLOADER_HEX):i -Ulock:w:0x0F:m 

ifeq ($(BUILD_TARGET),FreetronicsEtherMega2560)

PHONY+=enablejtag

# This uses the AVRISP mkII to enable JTAG (0x40) in the HFUSE on the
# ATmega2560. JTAG debugging isn't supported on smaller megaAVRs like the
# ATmega328P used on the Uno.
enablejtag:
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(ISP) -Pusb -Uhfuse:w:0x98:m
	
endif

endif

connect:
	stty sane dec
	netcat $(TARGET_IPADDRESS) 23
	stty sane dec

################################################################################
# PATTERNS
################################################################################

%.e:	%.cpp
	$(CROSS_COMPILE)$(CXX) -E $(CPPFLAGS) -c $< > $*.e

%.s:	%.cpp
	$(CROSS_COMPILE)$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%.o:	%.cpp
	$(CROSS_COMPILE)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%.e:	%.c
	$(CROSS_COMPILE)$(CC) -E $(CPPFLAGS) -c $< > $*.e

%.s:	%.c
	$(CROSS_COMPILE)$(CC) -S $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.o:	%.c
	$(CROSS_COMPILE)$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.eep:	%.elf
	$(CROSS_COMPILE)$(OBJCOPY) $(OBJCOPYEEPFLAGS) $< $@

%.hex:	%.elf
	$(CROSS_COMPILE)$(OBJCOPY) $(OBJCOPYHEXFLAGS) $< $@

%.map:	%.elf
	$(CROSS_COMPILE)$(NM) $(NMFLAGS) $< > $@

%.dmp:	%.elf
	$(CROSS_COMPILE)$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@

%.dis:	%.elf
	$(CROSS_COMPILE)$(OBJDUMP) $(OBJDISASSEMBLYFLAGS) $< > $@

%.siz:	%.elf
	$(CROSS_COMPILE)$(SIZE) $(SIZEFLAGS) $< > $@

################################################################################
# EXPERIMENTAL
################################################################################

ifeq ($(CONTROLLER),atmega2560)

# This builds a modified stk500v2 bootloader for the ATmega2560 that disables
# the hardware watchdog timer very early in its run-time initialization. This
# allows the Amigo watchdog enable feature to be safely used to restart the
# system in a manner that resets the hardware and restarts the software. This
# has been tested by enabling the use of amigo_watchdog_enable() in the
# amigo_panic() function and using it to successfully and safely restart the
# application. This new bootloader must be installed using some kind of AVR ISP
# hardware programmer.

PHONY+=experimental-bootloader experimental-clean experimental-flash experimental-patch experimental-apply

EXPERIMENTAL_DIR=hardware/arduino/bootloaders

EXPERIMENTAL_HEX=$(EXPERIMENTAL_DIR)/stk500v2/stk500boot_v2_mega2560.hex

EXPERIMENTAL_PATCH=stk500boot_v2_mega2560.patch

COLLATERAL+=$(EXPERIMENTAL_HEX)

experimental-bootloader $(EXPERIMENTAL_HEX):
	make -C $(EXPERIMENTAL_DIR)/stk500v2 mega2560
	
experimental-clean:
	make -C $(EXPERIMENTAL_DIR)/stk500v2 clean
	
experimental-flash:	$(EXPERIMENTAL_HEX)
	$(AVRDUDE) -C$(AVRDUDE_CONF) -p$(PART) -c$(ISP) -b$(BAUD) -Pusb -e -Ulock:w:0x3F:m -Uefuse:w:$(EFUSE):m -Uhfuse:w:$(HFUSE):m -Ulfuse:w:$(LFUSE):m
	$(AVRDUDE) -C$(AVRDUDE_CONF) -p$(PART) -c$(ISP) -b$(BAUD) -Pusb -Uflash:w:$(EXPERIMENTAL_HEX):i -Ulock:w:0x0F:m
	
experimental-patch $(EXPERIMENTAL_PATCH):
	diff -purN -x '.svn' -x '*.hex' $(BOOTLOADER_DIR)/stk500v2 $(EXPERIMENTAL_DIR)/stk500v2 > $(EXPERIMENTAL_PATCH) || true

experimental-apply:
	( cd $(BOOTLOADER_DIR)/stk500v2; patch -b -p9 ) < $(EXPERIMENTAL_PATCH)
	
endif

################################################################################
# STANDARD
################################################################################

PHONY+=all

all:	$(PREREQUISITES) $(DELIVERABLES)

PHONY+=full

full:	all $(COLLATERAL)

PHONY+=clean

clean:
	rm -rf $(ARTIFACTS)

PHONY+=pristine

pristine:	clean
	rm -rf $(DELIVERABLES) $(COLLATERAL)

################################################################################
# END
################################################################################

.PHONY:	$(PHONY)
