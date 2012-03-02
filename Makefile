################################################################################
# Copyright 2012 by the Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# http://www.diag.com/navigation/downloads/Amigo
################################################################################

PROJECT=amigo
DIRECTORY=Amigo

MAJOR=0
MINOR=3
BUILD=0

#BUILD_TARGET=Uno
BUILD_TARGET=EtherMega2560
BUILD_HOST=$(shell uname -s)
BUILD_PLATFORM=MegaBlink

SERIAL=/dev/tty.usbmodem26421
BAUD=115200

# Darwin identifies my desktop where I run graphical tools and interactive jobs.
# Linux identifies my multicore server where I run big background jobs.

ifeq ($(BUILD_HOST), Darwin)
TMP_DIR=/tmp
ROOT_DIR=$(HOME)/Desktop/Silver
PROJECT_DIR=$(ROOT_DIR)/projects/$(PROJECT)
FREERTOS_DIR=$(PROJECT_DIR)/FreeRTOSV7.1.0
APPLICATION_DIR=/Applications
ARDUINO_DIR=$(APPLICATION_DIR)/Arduino.app
TOOLS_DIR=$(ARDUINO_DIR)/Contents/Resources/Java/hardware/tools
TOOLCHAIN_BIN=$(TOOLS_DIR)/$(ARCH)/bin
AVRDUDE_CONF=$(TOOLS_DIR)/$(ARCH)/etc/avrdude.conf
endif

ifeq ($(BUILD_HOST), Linux)
TMP_DIR=/tmp
ROOT_DIR=$(HOME)
PROJECT_DIR=$(ROOT_DIR)/projects/$(PROJECT)
FREERTOS_DIR=$(PROJECT_DIR)/FreeRTOSV7.1.0
TOOLCHAIN_BIN=/usr/bin
AVRDUDE_CONF=$(WORKING_DIR)/avrdude.conf
endif

################################################################################
# CONFIGURATION
################################################################################

HTTP_URL=http://www.diag.com/navigation/downloads/$(DIRECTORY).html
FTP_URL=http://www.diag.com/ftp/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz
SVN_URL=svn://graphite/$(PROJECT)/trunk/$(DIRECTORY)

WORKING_DIR:=$(shell pwd)

DATESTAMP=$(shell date +'%Y%m%d')
TIMESTAMP=$(shell date -u +'%Y%m%d%H%M%S%N%Z')

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

OFILES+=$(addsuffix .o,$(basename $(CFILES)))

LIBRARIES+=-lm

ARCHFLAGS=-mmcu=$(CONTROLLER) -mrelax
#CPPFLAGS=-DF_CPU=$(FREQUENCY) -DARDUINO=$(ARDUINO) $(INCLUDES)
CPPFLAGS=-DF_CPU=$(FREQUENCY) $(INCLUDES)
#CFLAGS=-g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -std=c99
CFLAGS=-g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections
LDFLAGS=-Os -Wl,--gc-sections
OBJCOPYEEPFLAGS=-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 
OBJCOPYHEXFLAGS=-O ihex -R .eeprom

ARTIFACTS+=$(OFILES)
ARTIFACTS+=$(BUILD_PLATFORM).elf
ARTIFACTS+=$(BUILD_PLATFORM).map
ARTIFACTS+=$(BUILD_PLATFORM).eep
ARTIFACTS+=$(BUILD_PLATFORM).hex

DELIVERABLES+=$(BUILD_PLATFORM).hex

$(BUILD_PLATFORM).elf:	$(OFILES)
	$(CROSS_COMPILE)$(CC) $(ARCHFLAGS) $(LDFLAGS) -o $@ $(OFILES) $(OBJECTS) $(ARCHIVES) $(LIBRARIES)

################################################################################
# DEPENCENDIES
################################################################################

PHONY+=depend
ARTIFACTS+=$(BUILD_HOST).mk

# For some reason echo in make on the Mac (Darwin) doesn't understand the -n
# option even though its man page documents it. And echo in make on the server
# (Linux) doesn't understand \c. The paths in the dependencies file are
# different too, so we can't use the Mac dependencies file on Linux and vice
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
	
PHONY+=path implicit interrogate upload backup terminal

path:
	@echo export PATH=$(PATH):$(TOOLCHAIN_BIN)

implicit:
	$(CROSS_COMPILE)$(CXX) -dM -E -mmcu=$(CONTROLLER) - < /dev/null

backup:
	( cd $(FREERTOS_DIR)/..; DIRNAME="`basename $(FREERTOS_DIR)`"; echo $$DIRNAME; tar cvzf - $$DIRNAME > $$DIRNAME-$(TIMESTAMP).tgz )

# These targets only exist when running on my Mac because that's to what the
# Arduino board connects via USB.

ifeq ($(BUILD_HOST), Darwin)

interrogate:
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(PROGRAMMER) -Pusb -b$(BAUD) -t
	
upload:	$(BUILD_PLATFORM).hex
	stty -f $(SERIAL) hupcl
	$(AVRDUDE) -C$(AVRDUDE_CONF) -v -p$(CONTROLLER) -c$(CONFIG) -P$(SERIAL) -b$(BAUD) -D -Uflash:w:$<:i

terminal:
	stty -f $(SERIAL) hupcl
	screen -L $(SERIAL) $(BAUD)

endif

################################################################################
# PATTERNS
################################################################################

%.txt:	%.cpp
	$(CROSS_COMPILE)$(CXX) $(ARCHFLAGS) -E $(CPPFLAGS) -c $< > $*.txt

%.o:	%.cpp
	$(CROSS_COMPILE)$(CXX) $(ARCHFLAGS) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%.txt:	%.c
	$(CROSS_COMPILE)$(CC) $(ARCHFLAGS) -E $(CPPFLAGS) -c $< > $*.txt

%.o:	%.c
	$(CROSS_COMPILE)$(CC) $(ARCHFLAGS) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.eep:	%.elf
	$(CROSS_COMPILE)$(OBJCOPY) $(OBJCOPYEEPFLAGS) $< $@

%.hex:	%.elf
	$(CROSS_COMPILE)$(OBJCOPY) $(OBJCOPYHEXFLAGS) $< $@

%.map:	%.elf
	$(CROSS_COMPILE)$(NM) -n -o -a $< > $@

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
