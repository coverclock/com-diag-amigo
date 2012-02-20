################################################################################
# Copyright 2012 by the Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# http://www.diag.com/navigation/downloads/Amigo
################################################################################

PROJECT=amigo
DIRECTORY=Amigo

MAJOR=0
MINOR=1
BUILD=0

SVN_URL=svn://graphite/$(PROJECT)/trunk/$(DIRECTORY)
HTTP_URL=http://www.diag.com/navigation/downloads/$(DIRECTORY).html
FTP_URL=http://www.diag.com/ftp/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz

APPLICATION_DIR=/Applications/Arduino.app

TMP=/tmp

COMPILEFOR=uno
#COMPILEFOR=mega2560

################################################################################
# CONFIGURATION
################################################################################

ifeq ($(COMPILEFOR),uno)
ARCH=avr
FAMILY=avr5
CONTROLLER=atmega328p
FREQUENCY=16000000L
ARDUINO=100
CORE=arduino
VARIANT=standard
endif

ifeq ($(COMPILEFOR),mega2560)
ARCH=avr
FAMILY=avr6
CONTROLLER=atmega2560
FREQUENCY=16000000L
ARDUINO=100
CORE=arduino
VARIANT=mega
endif

PROCESSING_DIR=$(APPLICATION_DIR)/Contents/Resources/Java
LIBRARIES_DIR=$(PROCESSING_DIR)/Libraries
HARDWARE_DIR=$(PROCESSING_DIR)/hardware
TOOLCHAIN_DIR=$(HARDWARE_DIR)/tools/$(ARCH)/bin

CC=gcc
CXX=g++
LD=gcc
OBJCOPY=objcopy

CROSS_COMPILE=$(ARCH)-

INCLUDES+=-I$(HARDWARE_DIR)/arduino/cores/$(CORE)
INCLUDES+=-I$(HARDWARE_DIR)/arduino/variants/$(VARIANT)

OBJECTS=

ARCHIVES=

LIBRARIES+=-lm

CPPFLAGS=-DF_CPU=$(FREQUENCY) -DARDUINO=$(ARDUINO) $(INCLUDES)
CFLAGS=-g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(CONTROLLER)
LDFLAGS=-Os -Wl,--gc-sections -mmcu=$(CONTROLLER)
OBJCOPYHEXFLAGS=-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 
OBJCOPYEEPFLAGS=-O ihex -R .eeprom

################################################################################
# DEFAULT
################################################################################

PHONY+=default

default:	all

################################################################################
# DEPENCENDIES
################################################################################

PHONY+=depend
ARTIFACTS+=dependencies.mk

DEPENDS:=${shell find . -type f \( -name '*.c' -o -name '*.cpp' \) -print}

depend:
	cp /dev/null dependencies.mk
	for F in $(DEPENDS); do \
		D=`dirname $$F | sed "s/^\.\///"`; \
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
	TARDIR=$(shell mktemp -d /tmp/$(PROJECT).XXXXXXXXXX); \
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
