################################################################################
# Copyright 2012 by the Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# http://www.diag.com/navigation/downloads/Amigo
################################################################################

PROJECT=amigo
NAME=Amigo

MAJOR=0
MINOR=6
BUILD=0

HTTP_URL=http://www.diag.com/navigation/downloads/$(NAME).html
FTP_URL=http://www.diag.com/ftp/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz
SVN_URL=svn://graphite/$(PROJECT)/trunk/$(NAME)

#BUILD_TARGET=Uno
BUILD_TARGET=EtherMega2560
BUILD_HOST=$(shell uname -s)
BUILD_PLATFORM=MegaBlink

# This option uses the tool chain provided with Arduino on my desktop.
ifeq ($(BUILD_HOST), Unused)
TMP_DIR=/tmp
FREERTOS_DIR=FreeRTOSV7.1.0
ARDUINO_DIR=/Applications/Arduino.app/Contents/Resources/Java
BOOTLOADER_DIR=$(ARDUINO_DIR)/hardware/Arduino/bootloaders
TOOLS_DIR=$(ARDUINO_DIR)/hardware/tools/avr
AVRDUDE_CONF=$(TOOLS_DIR)/etc/avrdude.conf
TOOLCHAIN_BIN=$(TOOLS_DIR)/bin
TOOLCHAIN_DIR=$(TOOLS_DIR)/avr/include
AVRSTUDIO_DIR=Amigo/Debug
endif

# This option uses the tool chain provided with the AVR CrossPack on my desktop.
ifeq ($(BUILD_HOST), Darwin)
TMP_DIR=/tmp
FREERTOS_DIR=FreeRTOSV7.1.0
ARDUINO_DIR=/Applications/Arduino.app/Contents/Resources/Java
BOOTLOADER_DIR=$(ARDUINO_DIR)/hardware/Arduino/bootloaders
CROSSPACK_DIR=/usr/local/CrossPack-AVR
AVRDUDE_CONF=$(CROSSPACK_DIR)/etc/avrdude.conf
TOOLCHAIN_BIN=$(CROSSPACK_DIR)/bin
TOOLCHAIN_DIR=$(CROSSPACK_DIR)/avr/include
AVRSTUDIO_DIR=Amigo/Debug
endif

# This option uses the tool chain provided via AVR packages installed on my server.
ifeq ($(BUILD_HOST), Linux)
TMP_DIR=/tmp
FREERTOS_DIR=FreeRTOSV7.1.0
ARDUINO_DIR=$(HOME)/src/arduino-1.0-linux
BOOTLOADER_DIR=$(ARDUINO_DIR)/hardware/arduino/bootloaders
AVRDUDE_CONF=/etc/avrdude.conf
TOOLCHAIN_BIN=/usr/bin
TOOLCHAIN_DIR=/usr/lib/avr/include
AVRSTUDIO_DIR=Amigo/Debug
endif

#SERIAL=/dev/tty.usbmodem26421
SERIAL=/dev/tty.usbmodem411
BAUD=115200

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
BOARD=Uno_$(TOOLCHAIN)
CONFIG=arduino
PROGRAMMER=avrispmkII
ISP=stk500v2
PART=m328p
BOOTLOADER_HEX=$(BOOTLOADER_DIR)/optiboot/optiboot_atmega328.hex
EFUSE=0x05
HFUSE=0xDE# ?0xD6 EESAVE?
LFUSE=0xFF
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
BOARD=EtherMega2560_$(TOOLCHAIN)
CONFIG=stk500v2
PROGRAMMER=avrispmkII
ISP=stk500v2
PART=m2560
BOOTLOADER_HEX=$(BOOTLOADER_DIR)/stk500v2/stk500boot_v2_mega2560.hex
EFUSE=0xFD
HFUSE=0xD8
LFUSE=0xFF
endif

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
# BUILD
################################################################################

ifeq ($(BUILD_PLATFORM), MegaBlink)
CFILES+=$(FREERTOS_DIR)/Demo/$(BOARD)/$(BUILD_PLATFORM)/main.c
CFILES+=$(FREERTOS_DIR)/Demo/$(BOARD)/lib_digitalAnalog/digitalAnalog.c
CFILES+=$(FREERTOS_DIR)/Demo/$(BOARD)/lib_serial/lib_serial.c
CFILES+=$(FREERTOS_DIR)/Source/portable/MemMang/heap_2.c
HDIRECTORIES+=$(FREERTOS_DIR)/Demo/$(BOARD)/include
endif

ifeq ($(BUILD_PLATFORM), Blink)
CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(BOARD)/$(BUILD_PLATFORM)/main.cpp
CXXFILES+=$(FREERTOS_DIR)/$(NAME)/$(BOARD)/$(BUILD_PLATFORM)/heap.cpp
CFILES+=$(FREERTOS_DIR)/Source/portable/MemMang/heap_2.c
HDIRECTORIES+=$(FREERTOS_DIR)/$(NAME)/$(BOARD)/include
endif

CFILES+=$(FREERTOS_DIR)/Source/portable/$(TOOLCHAIN)/$(PORTABLE)/port.c
CFILES+=$(FREERTOS_DIR)/Source/croutine.c
CFILES+=$(FREERTOS_DIR)/Source/list.c
CFILES+=$(FREERTOS_DIR)/Source/queue.c
CFILES+=$(FREERTOS_DIR)/Source/tasks.c
CFILES+=$(FREERTOS_DIR)/Source/timers.c

HDIRECTORIES+=$(FREERTOS_DIR)/Source/portable/$(TOOLCHAIN)/$(PORTABLE)
HDIRECTORIES+=$(FREERTOS_DIR)/Source/include

INCLUDES+=$(addprefix -I,$(HDIRECTORIES))
LIBRARIES+=-lm

OPT=s
CARCH=-mmcu=$(CONTROLLER)
CDIALECT=-std=gnu99 -fno-exceptions -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums# -mrelax
CXXDIALECT=-fno-exceptions -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums# -mrelax
CDEBUG=-g
CWARN=-Wall
CTUNING=-fno-exceptions -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums# -mrelax
CPPFLAGS=-DF_CPU=$(FREQUENCY) -DARDUINO=$(ARDUINO) $(INCLUDES)
CFLAGS=$(CARCH) $(CDIALECT) $(CDEBUG) -O$(OPT) $(CWARN) $(CEXTRA)
CXXFLAGS=$(CARCH) $(CXXDIALECT) $(CDEBUG) -O$(OPT) $(CWARN) $(CXXEXTRA)
LDFLAGS=$(CARCH) -O$(OPT) -Wl,--gc-sections
NMFLAGS=-n -o -a -A
OBJDUMPFLAGS=-x -G -t -r
SIZEFLAGS=-C --mcu=$(CONTROLLER)
OBJDISASSEMBLYFLAGS=-d
OBJCOPYEEPFLAGS=-O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 
OBJCOPYHEXFLAGS=-O ihex -R .eeprom

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

################################################################################
# DEPENDENCIES
################################################################################

PHONY+=depend
ARTIFACTS+=dependencies.mk

depend:
	cp /dev/null dependencies.mk
	for F in $(CFILES); do \
		D=`dirname $$F`; \
		B=`basename -s .c $$F`; \
		$(CXX) $(CPPFLAGS) -MM -MT $$D/$$B.o -MG $$F >> dependencies.mk; \
	done
	for F in $(CXXFILES); do \
		D=`dirname $$F`; \
		B=`basename -s .cpp $$F`; \
		$(CXX) $(CPPFLAGS) -MM -MT $$D/$$B.o -MG $$F >> dependencies.mk; \
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

PHONY+=parameters verify backup

parameters:
	@echo BUILD_TARGET=$(BUILD_TARGET)
	@echo BUILD_HOST=$(BUILD_HOST)
	@echo BUILD_PLATFORM=$(BUILD_PLATFORM)

verify:	$(CFILES) $(CXXFILES) $(HDIRECTORIES)

backup:
	DIRNAME="$(shell basename $(shell pwd))"; \
	tar -C .. -cvzf - $$DIRNAME > ../$$DIRNAME-$(shell date -u +'%Y%m%d%H%M%S%N%Z').tgz

PHONY+=preprocess preassemble

preprocess:	$(EFILES)

preassemble:	$(SFILES)
	
PHONY+=path implicit backup bundle

# Using back quotes like `make path` on the command line sets the PATH variable
# in your interactive shell.

path:
	@echo export PATH=$(PATH):$(TOOLCHAIN_BIN)

# This is because I'm curious.

implicit:
	$(CROSS_COMPILE)$(CXX) -dM -E -mmcu=$(CONTROLLER) - < /dev/null

# These targets only exist when running on my Mac desktop because that's to what
# the Arduino boards connects via USB. The Dell server is two floors down!

ifeq ($(BUILD_HOST), Darwin)
	
PHONY+=interrogate screen flashapplication flashbootloader flashdebug

# As far as I can tell, the Arduino bootloaders on both the Uno and the Mega
# only pretend to implement the avrdude commands to query the signature bytes
# from the EEPROM. But in fact the bootloaders return hardcoded values, not the
# actual values from the chip. If queried for other chip values like the
# fuses, they return hardcoded zeros. If you want to play with the chip,
# you'll need an In-System Programmer (ISP) device like the Atmel AVRISP mkII,
# which is what I use.

# This uses the AVRISP mkII to start an interactive session.
interrogate:
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(PROGRAMMER) -Pusb -t

screen:
	stty sane
	stty -f $(SERIAL) hupcl
	# control-A control-\ y to exit.
	screen -L $(SERIAL) $(BAUD)
	stty sane

# This uses the bootloader to load the application.
flashapplication:	$(BUILD_PLATFORM).hex
	stty -f $(SERIAL) hupcl
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(CONFIG) -P$(SERIAL) -b$(BAUD) -D -Uflash:w:$<:i

# This uses the bootloader to load the debug image produced by AVR Studio 5.1.
flashdebug:	$(AVRSTUDIO_DIR)/$(NAME).hex
	stty -f $(SERIAL) hupcl
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(CONFIG) -P$(SERIAL) -b$(BAUD) -D -Uflash:w:$(AVRSTUDIO_DIR)/$(NAME).hex:i
	
# This uses the AVRISP mkII to
# unlock the bootloader,
# initialize all fuses to Arduino defaults,
# reflash the bootloader, and
# lock the bootloader.
flashbootloader:	$(BOOTLOADER_HEX)
	$(AVRDUDE) -C$(AVRDUDE_CONF) -p$(PART) -c$(ISP) -Pusb -e -Ulock:w:0x3F:m -Uefuse:w:$(EFUSE):m -Uhfuse:w:$(HFUSE):m -Ulfuse:w:$(LFUSE):m
	$(AVRDUDE) -C$(AVRDUDE_CONF) -p$(PART) -c$(ISP) -Pusb -Uflash:w:$(BOOTLOADER_HEX):i -Ulock:w:0x0F:m 

ifeq ($(BUILD_TARGET),EtherMega2560)

PHONY+=enablejtag

# This uses the AVRISP mkII to enable JTAG (0x40) in the HFUSE on the ATmega2560.
enablejtag:
	$(AVRDUDE) -v -C$(AVRDUDE_CONF) -p$(PART) -c$(ISP) -Pusb -Uhfuse:w:0x98:m
	
endif

endif

################################################################################
# PATTERNS
################################################################################

%.e:	%.cpp
	$(CROSS_COMPILE)$(CXX)-E $(CPPFLAGS) -c $< > $*.e

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
# TARGETS
################################################################################

PHONY+=all

all:	$(DELIVERABLES)

PHONY+=full

full:	$(DELIVERABLES) $(COLLATERAL)

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
