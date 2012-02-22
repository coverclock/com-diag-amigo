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

COMPILEFOR=uno
#COMPILEFOR=mega2560

COMPILEON=$(shell uname -s)

################################################################################
# CONFIGURATION
################################################################################

TMP_DIR=/tmp
PROJECT_DIR=$(HOME)/projects/$(PROJECT)
FREERTOS_DIR=$(PROJECT_DIR)/FreeRTOSV7.1.0
APPLICATION_DIR=/Applications/Arduino.app
PROCESSING_DIR=$(APPLICATION_DIR)/Contents/Resources/Java
LIBRARIES_DIR=$(PROCESSING_DIR)/Libraries
HARDWARE_DIR=$(PROCESSING_DIR)/hardware

ifeq ($(COMPILEON), Darwin)
TOOLCHAIN_DIR=$(HARDWARE_DIR)/tools/$(ARCH)/bin
endif

ifeq ($(COMPILEON), Linux)
TOOLCHAIN_DIR=/usr/bin
endif

ifeq ($(COMPILEFOR),uno)
ARCH=avr
FAMILY=avr5
CONTROLLER=atmega328p
FREQUENCY=16000000L
ARDUINO=100
CORE=arduino
VARIANT=standard
TARGET=__AVR_ATmega328P__
PORTABLE=ATmega328P
TOOLCHAIN=GCC
DEMO=Arduino_GCC
endif

ifeq ($(COMPILEFOR),mega2560)
ARCH=avr
FAMILY=avr6
CONTROLLER=atmega2560
FREQUENCY=16000000L
ARDUINO=100
CORE=arduino
VARIANT=mega
TARGET=__AVR_ATmega2560__
PORTABLE=ATmega2560
TOOLCHAIN=GCC
DEMO=Arduino_GCC
endif

CROSS_COMPILE=$(ARCH)-

CC=gcc
CXX=g++
LD=gcc
OBJCOPY=objcopy

#INCLUDES+=-I$(HARDWARE_DIR)/arduino/cores/$(CORE)
#INCLUDES+=-I$(HARDWARE_DIR)/arduino/variants/$(VARIANT)

################################################################################
# DEFAULT
################################################################################

PHONY+=default

default:	all

################################################################################
# BUILD
################################################################################

HDIRECTORIES+=$(FREERTOS_DIR)/Source/include
HDIRECTORIES+=$(FREERTOS_DIR)/Source/portable/$(TOOLCHAIN)/$(PORTABLE)
HDIRECTORIES+=$(FREERTOS_DIR)/Demo/$(DEMO)/include

CDIRECTORIES+=$(FREERTOS_DIR)/Source
CDIRECTORIES+=$(FREERTOS_DIR)/Source/portable/MemMang
CDIRECTORIES+=$(FREERTOS_DIR)/Source/portable/$(TOOLCHAIN)/$(PORTABLE)
CDIRECTORIES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_crc
CDIRECTORIES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_digitalAnalog
CDIRECTORIES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_fatf
CDIRECTORIES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_i2c
CDIRECTORIES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_serial
CDIRECTORIES+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_spi

INCLUDES=$(addprefix -I,$(HDIRECTORIES))

FILTER+=$(FREERTOS_DIR)/Demo/$(DEMO)/lib_fatf/cc932.c

CFILES=$(filter-out $(FILTER),$(wildcard $(addsuffix /*.c,$(CDIRECTORIES))))

OFILES=$(addsuffix .o,$(basename $(CFILES)))

LIBRARIES+=-lm

#CPPFLAGS=-DF_CPU=$(FREQUENCY) -DARDUINO=$(ARDUINO) $(INCLUDES)
CPPFLAGS=-DF_CPU=$(FREQUENCY) -D$(TARGET)=1 $(INCLUDES)
CFLAGS=-g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(CONTROLLER) -std=c99
LDFLAGS=-Os -Wl,--gc-sections -mmcu=$(CONTROLLER)
OBJCOPYHEXFLAGS=-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 
OBJCOPYEEPFLAGS=-O ihex -R .eeprom

foo:	$(OFILES)

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
# PATTERNS
################################################################################

%.txt:	%.cpp
	$(CROSS_COMPILE)$(CXX) -E $(CPPFLAGS) -c $< > $*.txt

%.o:	%.cpp
	$(CROSS_COMPILE)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%.txt:	%.c
	$(CROSS_COMPILE)$(CC) -E $(CPPFLAGS) -c $< > $*.txt

%.o:	%.c
	$(CROSS_COMPILE)$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.elf:	%.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -o $@ $< $(OBJECTS) $(ARCHIVES) $(LIBRARIES)

%.eep:	%.elf
	$(CROSS_COMPILE)$(OBJCOPY) $(OBJCOPYEEPFLAGS) $< $@

%.hex:	%.elf
	$(CROSS_COMPILE)$(OBJCOPY) $(OBJCOPYHEXFLAGS) $< $@

%:	%_unstripped
	$(CROSS_COMPILE)$(STRIP) -o $@ $<

################################################################################
# BUILD
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
