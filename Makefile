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

SVN_URL=svn://graphite/$(PROJECT)/trunk/$(DIRECTORY)
HTTP_URL=http://www.diag.com/navigation/downloads/$(DIRECTORY).html
FTP_URL=http://www.diag.com/ftp/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz

#BUILD_TARGET=Uno
BUILD_TARGET=EtherMega2560
BUILD_HOST=$(shell uname -s)
BUILD_PLATFORM=MegaBlink

WORKING_DIR:=$(shell pwd)

SERIAL=/dev/null
BAUD=115200

DATESTAMP=$(shell date +'%Y%m%d')
TIMESTAMP=$(shell date -u +'%Y%m%d%H%M%S%N%Z')

################################################################################
# CONFIGURATION
################################################################################


# Darwin identifies my desktop where I run graphical tools and interactive jobs.
# Linux identifies my multicore server where I run builds and background jobs.

ifeq ($(BUILD_HOST), Darwin)
TMP_DIR=/tmp
ROOT_DIR=$(HOME)/Desktop/Silver
PROJECT_DIR=$(ROOT_DIR)/projects/$(PROJECT)
FREERTOS_DIR=$(PROJECT_DIR)/FreeRTOSV7.1.0
TOOLCHAIN_BIN=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/$(ARCH)/bin
AVRDUDE_CONF=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/$(ARCH)/etc/avrdude.conf
endif

ifeq ($(BUILD_HOST), Linux)
TMP_DIR=/tmp
ROOT_DIR=$(HOME)
PROJECT_DIR=$(ROOT_DIR)/projects/$(PROJECT)
FREERTOS_DIR=$(PROJECT_DIR)/FreeRTOSV7.1.0
TOOLCHAIN_BIN=/usr/bin
AVRDUDE_CONF=$(WORKING_DIR)/avrdude.conf
endif

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
CONFIG=arduino
PROGRAMMER=avrispmkII
PART=m2560
endif

ifeq ($(BUILD_PLATFORM), MegaBlink)
CFILES+=$(FREERTOS_DIR)/Source/portable/MemMang/heap_2.c
CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_digitalAnalog/digitalAnalog.c
CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_serial/lib_serial.c
CFILES+=$(FREERTOS_DIR)/Demo/$(DEMO)/MegaBlink/main.c
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

CC=gcc
CXX=g++
LD=gcc
OBJCOPY=objcopy
AVRDUDE=avrdude

ARCHFLAGS=-mmcu=$(CONTROLLER) -mrelax
#CPPFLAGS=-DF_CPU=$(FREQUENCY) -DARDUINO=$(ARDUINO) $(INCLUDES)
CPPFLAGS=-DF_CPU=$(FREQUENCY) $(INCLUDES)
#CFLAGS=-g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -std=c99
CFLAGS=-g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections
LDFLAGS=-Os -Wl,--gc-sections
OBJCOPYHEXFLAGS=-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 
OBJCOPYEEPFLAGS=-O ihex -R .eeprom

################################################################################
# DEFAULT
################################################################################

PHONY+=default

default:	all

################################################################################
# BUILD
################################################################################

ARTIFACTS+=$(OFILES)
ARTIFACTS+=MegaBlink.elf
ARTIFACTS+=MegaBlink.eep
ARTIFACTS+=MegaBlink.hex

DELIVERABLES+=MegaBlink.hex

MegaBlink.elf:	$(OFILES)
	$(CROSS_COMPILE)$(CC) $(ARCHFLAGS) $(LDFLAGS) -o $@ $(OFILES) $(OBJECTS) $(ARCHIVES) $(LIBRARIES)

MegaBlink.eep:	MegaBlink.elf

MegaBlink.hex:	MegaBlink.elf

#avr-g++ -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=100 -Iarduino -Istandard PWM.cpp -oPWM.cpp.o 
#avr-gcc -Os -Wl,--gc-sections -mmcu=atmega328p -o PWM.cpp.elf PWM.cpp.o core.a -Lbuild373539219298904089.tmp -lm 
#avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 PWM.cpp.elf PWM.cpp.eep 
#avr-objcopy -O ihex -R .eeprom PWM.cpp.elf PWM.cpp.hex 
#avrdude -Cavrdude.conf -v -v -v -v -patmega328p -carduino -P/dev/tty.usbmodem26431 -b115200 -D -Uflash:w:PWM.cpp.hex:i 

################################################################################
# DEPENCENDIES
################################################################################

PHONY+=depend
ARTIFACTS+=dependencies.mk

depend:
	cp /dev/null dependencies.mk
	for F in $(CFILES); do \
		D=`dirname $$F`; \
		echo -n "$$D/" >> dependencies.mk; \
		$(CXX) $(CPPFLAGS) -MM -MG $$F >> dependencies.mk; \
	done

-include dependencies.mk

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
	
PHONY+=path implicit interrogate upload backup

path:
	@echo export PATH=$(PATH):$(TOOLCHAIN_BIN)

implicit:
	$(CROSS_COMPILE)$(CXX) -dM -E -mmcu=$(CONTROLLER) - < /dev/null

interrogate:
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(PROGRAMMER) -Pusb -b$(BAUD) -t
	
upload:	$(PLATFORM).hex
	stty -f $(SERIAL) hupcl
	$(AVRDUDE) -C$(AVRDUDE_CONF) -v -p$(CONTROLLER) -c$(CONFIG) -P$(SERIAL) -b$(BAUD) -D -Uflash:w:$<:i

backup:
	( cd $(FREERTOS_DIR)/..; DIRNAME="`basename $(FREERTOS_DIR)`"; echo $$DIRNAME; tar cvzf - $$DIRNAME > $$DIRNAME-$(TIMESTAMP).tgz )

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
