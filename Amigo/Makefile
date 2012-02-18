################################################################################
# Copyright 2012 by the Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h.
# Chip Overclock <coverclock@diag.com>
# http://www.diag.com/navigation/downloads/Amigo
################################################################################

PROJECT=amigo
DIRECTORY=Amigo

MAJOR=0
MINOR=0
BUILD=0

SVN_URL=svn://graphite/$(PROJECT)/trunk/$(DIRECTORY)
HTTP_URL=http://www.diag.com/navigation/downloads/$(DIRECTORY).html

DISTRIBUTION_DIR=$(HOME)/Desktop/Platinum/Applications
APPLICATION_DIR=/Applications/Arduino.app/Contents/Resources/Java/Libraries

TMP=/tmp

################################################################################

default:	all

PHONY+=dist
ARTIFACTS+=$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz

dist $(ARCHIVE)-$(MAJOR).$(MINOR).$(BUILD).tgz:
	TARDIR=$(shell mktemp -d /tmp/$(ARCHIVE).XXXXXXXXXX); \
	svn export $(SVN_URL) $$TARDIR/$(ARCHIVE)-$(MAJOR).$(MINOR).$(BUILD); \
	tar -C $$TARDIR -cvzf - $(ARCHIVE)-$(MAJOR).$(MINOR).$(BUILD) > $(ARCHIVE)-$(MAJOR).$(MINOR).$(BUILD).tgz; \
	rm -rf $$TARDIR

################################################################################

PHONY+=all

all:	$(DELIVERABLES)

PHONY+=clean

clean:
	rm -rf $(ARTIFACTS)

################################################################################

.PHONY:	$(PHONY)
