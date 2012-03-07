################################################################################
# Copyright 2012 by the Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# http://www.diag.com/navigation/downloads/Amigo
################################################################################

PROJECT=amigo
DIRECTORY=Amigo

#BUILD_TARGET=Uno
BUILD_TARGET=EtherMega2560
BUILD_HOST=$(shell uname -s)
BUILD_PLATFORM=MegaBlink

#SERIAL=/dev/tty.usbmodem26421
SERIAL=/dev/tty.usbmodem411
BAUD=115200

# Darwin identifies my Mac Mini desktop.
# Linux identifies my Dell quadcore server.

ifeq ($(BUILD_HOST), Darwin)
TMP_DIR=/tmp
APPLICATION_DIR=/Applications
ROOT_DIR=$(HOME)/Desktop/Silver
PROJECT_DIR=$(ROOT_DIR)/projects/$(PROJECT)
endif

ifeq ($(BUILD_HOST), Linux)
TMP_DIR=/tmp
ROOT_DIR=$(HOME)
PROJECT_DIR=$(ROOT_DIR)/projects/$(PROJECT)
endif

################################################################################
# CONFIGURATION
################################################################################

MAJOR=0
MINOR=4
BUILD=0

HTTP_URL=http://www.diag.com/navigation/downloads/$(DIRECTORY).html
FTP_URL=http://www.diag.com/ftp/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz
SVN_URL=svn://graphite/$(PROJECT)/trunk/$(DIRECTORY)

WORKING_DIR:=$(shell pwd)

DATESTAMP=$(shell date +'%Y%m%d')
TIMESTAMP=$(shell date -u +'%Y%m%d%H%M%S%N%Z')

ifeq ($(BUILD_TARGET),Uno)
ARCH=avr
CROSS_COMPILE=$(ARCH)-
FAMILY=avr5
CONTROLLER=atmega328p
FREQUENCY=16000000L
ARDUINO=100
CORE=arduino
VARIANT=standard
TARGET=__AVR_ATmega328P__
PORTABLE=ATmega328P
TOOLCHAIN=GCC
DEMO=Uno_$(TOOLCHAIN)
CONFIG=arduino
PROGRAMMER=avrispmkII
PART=m328p
endif

ifeq ($(BUILD_TARGET),EtherMega2560)
ARCH=avr
CROSS_COMPILE=$(ARCH)-
FAMILY=avr6
CONTROLLER=atmega2560
FREQUENCY=16000000L
ARDUINO=100
CORE=arduino
VARIANT=mega
TARGET=__AVR_ATmega2560__
PORTABLE=ATmega2560
TOOLCHAIN=GCC
DEMO=EtherMega2560_$(TOOLCHAIN)
CONFIG=stk500v2
PROGRAMMER=avrispmkII
PART=m2560
endif

ifeq ($(BUILD_HOST), Darwin)
FREERTOS_DIR=$(PROJECT_DIR)/FreeRTOSV7.1.0
TOOLS_DIR=$(APPLICATION_DIR)/Arduino.app/Contents/Resources/Java/hardware/tools
TOOLCHAIN_BIN=$(TOOLS_DIR)/$(ARCH)/bin
AVRDUDE_CONF=$(TOOLS_DIR)/$(ARCH)/etc/avrdude.conf
endif

ifeq ($(BUILD_HOST), Linux)
FREERTOS_DIR=$(PROJECT_DIR)/FreeRTOSV7.1.0
TOOLCHAIN_BIN=/usr/bin
AVRDUDE_CONF=$(WORKING_DIR)/avrdude.conf
endif

CC=gcc
CXX=g++
LD=gcc
NM=nm
OBJCOPY=objcopy
AVRDUDE=avrdude

################################################################################
# DEFAULT
################################################################################

PHONY+=default

default:	all

################################################################################
# BUILD
################################################################################

ifeq ($(BUILD_PLATFORM), MegaBlink)
CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/MegaBlink/main.c
CFILES+=$(FREERTOS_DIR)/Source/portable/MemMang/heap_2.c
CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_digitalAnalog/digitalAnalog.c
CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_serial/lib_serial.c
endif

CFILES+=$(FREERTOS_DIR)/Source/croutine.c
CFILES+=$(FREERTOS_DIR)/Source/list.c
CFILES+=$(FREERTOS_DIR)/Source/queue.c
CFILES+=$(FREERTOS_DIR)/Source/tasks.c
CFILES+=$(FREERTOS_DIR)/Source/timers.c
CFILES+=$(FREERTOS_DIR)/Source/portable/$(TOOLCHAIN)/$(PORTABLE)/port.c

#INCLUDES+=-I$(HARDWARE_DIR)/arduino/cores/$(CORE)
#INCLUDES+=-I$(HARDWARE_DIR)/arduino/variants/$(VARIANT)

HDIRECTORIES+=$(FREERTOS_DIR)/Source/include
HDIRECTORIES+=$(FREERTOS_DIR)/Source/portable/$(TOOLCHAIN)/$(PORTABLE)
HDIRECTORIES+=$(FREERTOS_DIR)/Demo/$(DEMO)/include

INCLUDES+=$(addprefix -I,$(HDIRECTORIES))

#CFILES+=$(FREERTOS_DIR)/Source/portable/MemMang/heap_1.c
#CFILES+=$(FREERTOS_DIR)/Source/portable/MemMang/heap_3.c
#CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_crc/crc8.c
#CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_ext_ram/ext_ram.c
#CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_fatf/cc932.c
#CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_fatf/ccsbcs.c
#CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_fatf/diskio.c
#CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_fatf/ff.c
#CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_i2c/i2cMultiMaster.c
#CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_rtc/rtc.c
#CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_servo/servoPWM.c
#CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_spi/spi.c

LIBRARIES+=-lm

ARCHFLAGS=-mmcu=$(CONTROLLER) -mrelax
#CPPFLAGS=-DF_CPU=$(FREQUENCY) -DARDUINO=$(ARDUINO) $(INCLUDES)
CPPFLAGS=-DF_CPU=$(FREQUENCY) $(INCLUDES)
#CFLAGS=-g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -std=c99
CFLAGS=-g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections
LDFLAGS=-Os -Wl,--gc-sections
OBJCOPYEEPFLAGS=-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 
OBJCOPYHEXFLAGS=-O ihex -R .eeprom

OFILES+=$(addsuffix .o,$(basename $(CFILES) $(CXXFILES)))

EFILES+=$(addsuffix .e,$(basename $(CFILES) $(CXXFILES)))

SFILES+=$(addsuffix .s,$(basename $(CFILES) $(CXXFILES)))

ARTIFACTS+=$(OFILES)
ARTIFACTS+=$(SFILES)
ARTIFACTS+=$(EFILES)
ARTIFACTS+=$(BUILD_PLATFORM).elf
ARTIFACTS+=$(BUILD_PLATFORM).map
ARTIFACTS+=$(BUILD_PLATFORM).eep
ARTIFACTS+=$(BUILD_PLATFORM).hex

DELIVERABLES+=$(BUILD_PLATFORM).hex
DELIVERABLES+=$(BUILD_PLATFORM).map
	
PHONY+=preprocess

preprocess:	$(EFILES)

PHONY+=preassemble

preassemble:	$(SFILES)

$(BUILD_PLATFORM).elf:	$(OFILES)
	$(CROSS_COMPILE)$(CC) $(ARCHFLAGS) $(LDFLAGS) -o $@ $(OFILES) $(OBJECTS) $(ARCHIVES) $(LIBRARIES)

################################################################################
# DEPENDENCIES
################################################################################

PHONY+=depend
ARTIFACTS+=$(BUILD_HOST).mk

# For some reason echo in make on the Mac (Darwin) doesn't understand the -n
# option even though its man page documents it. And echo in make on the Dell
# (Linux) doesn't understand \c. The paths in the dependencies file are
# different too, so we can't use the Mac dependencies file on the Dell and vice
# versa. Nothing is ever simple.

depend:
	cp /dev/null $(BUILD_HOST).mk
ifeq ($(BUILD_HOST), Darwin)
	for F in $(CFILES); do \
		D=`dirname $$F`; \
		echo "$$D/\c" >> dependencies.mk; \
		$(CXX) $(CPPFLAGS) -MM -MG $$F >> $(BUILD_HOST).mk; \
	done
endif
ifeq ($(BUILD_HOST), Linux)
	for F in $(CFILES); do \
		D=`dirname $$F`; \
		echo -n "$$D/" >> dependencies.mk; \
		$(CXX) $(CPPFLAGS) -MM -MG $$F >> $(BUILD_HOST).mk; \
	done
endif

-include $(BUILD_HOST).mk

################################################################################
# DISTRIBUTION
################################################################################

PHONY+=dist
ARTIFACTS+=$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz

dist $(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz:
	TARDIR=$(shell mktemp -d $(TMP_DIR)/$(PROJECT).XXXXXXXXXX); \
	svn export $(SVN_URL) $$TARDIR/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD); \
	tar -C $$TARDIR -cvzf - $(PROJECT)-$(MAJOR).$(MINOR).$(BUILD) > $(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz; \
	rm -rf $$TARDIR

################################################################################
# DOCUMENTATION
################################################################################

DOC_DIR=doc
BROWSER=firefox

PHONY+=documentation browse refman manpages
DELIVERABLES+=$(DOC_DIR)

documentation:
	mkdir -p $(DOC_DIR)/latex $(DOC_DIR)/man $(DOC_DIR)/pdf
	sed -e "s/\\\$$Name.*\\\$$/$(MAJOR).$(MINOR).$(BUILD)/" < doxygen.cf > doxygen-local.cf
	doxygen doxygen-local.cf
	( cd $(DOC_DIR)/latex; $(MAKE) refman.pdf; cp refman.pdf ../pdf )
	cat $(DOC_DIR)/man/man3/*.3 | groff -man -Tps - > $(DOC_DIR)/pdf/manpages.ps
	ps2pdf $(DOC_DIR)/pdf/manpages.ps $(DOC_DIR)/pdf/manpages.pdf

browse:
	$(BROWSER) file:doc/html/index.html

refman:
	$(BROWSER) file:doc/pdf/refman.pdf

manpages:
	$(BROWSER) file:doc/pdf/manpages.pdf

################################################################################
# UTILITIES
################################################################################
	
PHONY+=interrogate upload terminal

# Using back quotes like `make path` on the command line sets the PATH variable
# in your interactive shell.

path:
	@echo export PATH=$(PATH):$(TOOLCHAIN_BIN)

implicit:
	$(CROSS_COMPILE)$(CXX) -dM -E -mmcu=$(CONTROLLER) - < /dev/null

backup:
	( cd $(FREERTOS_DIR)/..; DIRNAME="`basename $(FREERTOS_DIR)`"; echo $$DIRNAME; tar cvzf - $$DIRNAME > $$DIRNAME-$(TIMESTAMP).tgz )

# These targets only exist when running on my Mac desktop because that's to what
# the Arduino boards connects via USB. The Dell server is two floors down!

ifeq ($(BUILD_HOST), Darwin)
	
PHONY+=interrogate upload terminal

# As far as I can tell, the Arduino bootloaders on both the Uno and the Mega
# only pretend to implement the avrdude commands to query the signature bytes
# from the EEPROM. But in fact the bootloaders return hardcoded values, not the
# actual values from the EEPROM. If queried for other EEPROM values like the
# fuses, they return hardcoded zeros. If you want to play with the EEPROM,
# you'll need an In-System Programmer (ISP) device like the Atmel AVRISP mkII,
# which is what I use.

# This uses the AVRISP mkII.
interrogate:
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(PROGRAMMER) -Pusb -b$(BAUD) -t

# This uses the bootloader.
upload:	$(BUILD_PLATFORM).hex
	stty -f $(SERIAL) hupcl
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(CONFIG) -P$(SERIAL) -b$(BAUD) -D -Uflash:w:$<:i

terminal:
	stty -f $(SERIAL) sane
	stty -f $(SERIAL) hupcl
	# control-A control-\ y to exit.
	screen -L $(SERIAL) $(BAUD)
	stty -f $(SERIAL) sane

endif

################################################################################
# PATTERNS
################################################################################

%.e:	%.cpp
	$(CROSS_COMPILE)$(CXX) $(ARCHFLAGS) -E $(CPPFLAGS) -c $< > $*.e

%.s:	%.cpp
	$(CROSS_COMPILE)$(CXX) $(ARCHFLAGS) -S $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%.o:	%.cpp
	$(CROSS_COMPILE)$(CXX) $(ARCHFLAGS) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%.e:	%.c
	$(CROSS_COMPILE)$(CC) $(ARCHFLAGS) -E $(CPPFLAGS) -c $< > $*.e

%.s:	%.c
	$(CROSS_COMPILE)$(CC) $(ARCHFLAGS) -S $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.o:	%.c
	$(CROSS_COMPILE)$(CC) $(ARCHFLAGS) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.eep:	%.elf
	$(CROSS_COMPILE)$(OBJCOPY) $(OBJCOPYEEPFLAGS) $< $@

%.hex:	%.elf
	$(CROSS_COMPILE)$(OBJCOPY) $(OBJCOPYHEXFLAGS) $< $@

%.map:	%.elf
	$(CROSS_COMPILE)$(NM) -n -o -a -A $< > $@

%:	%_unstripped
	$(CROSS_COMPILE)$(STRIP) -o $@ $<

################################################################################
# TARGETS
################################################################################

PHONY+=all

all:	$(DELIVERABLES)

PHONY+=clean

clean:
	rm -rf $(ARTIFACTS)

################################################################################
# END
################################################################################

.PHONY:	$(PHONY)

# Cribbed from verbose output of Arduino build for the Uno to use as a working reference design.
# avr-g++ -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=100 -Iarduino -Istandard PWM.cpp -oPWM.cpp.o 
# avr-gcc -Os -Wl,--gc-sections -mmcu=atmega328p -o PWM.cpp.elf PWM.cpp.o core.a -Lbuild373539219298904089.tmp -lm 
# avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 PWM.cpp.elf PWM.cpp.eep 
# avr-objcopy -O ihex -R .eeprom PWM.cpp.elf PWM.cpp.hex 
# avrdude -Cavrdude.conf -v -v -v -v -patmega328p -carduino -P/dev/tty.usbmodem26431 -b115200 -D -Uflash:w:PWM.cpp.hex:i 

# Cribbed from verbose output of Arduino build for the Mega 2560 to use as a working reference design.
# avr-g++ -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=100 -Iarduino -Imega Empty.cpp -oEmpty.cpp.o 
# avr-gcc -Os -Wl,--gc-sections -mmcu=atmega2560 -o Empty.cpp.elf Empty.cpp.o core.a -Lbuild7537680787165628439.tmp -lm 
# avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 Empty.cpp.elf Empty.cpp.eep 
# avr-objcopy -O ihex -R .eeprom Empty.cpp.elf Empty.cpp.hex 
# avrdude -Cavrdude.conf -v -v -v -v -patmega2560 -cstk500v2 -P/dev/tty.usbmodem26421 -b115200 -D -Uflash:w:Empty.cpp.hex:i

# These directories were of interest when using the GNU GCC AVR tool chain.
# /usr/lib/avr/include
# /usr/lib/gcc/avr/4.3.4/include
# /usr/lib/gcc/avr/4.3.4/include-fixed
# /usr/lib/gcc/avr/4.3.4/install-tools/include
# /usr/lib/ldscripts
