# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# Copy logeng test dat file(s)
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	TARGETDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/system/data
	TARGETDIR2= $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/private/101f401d
else
	TARGETDIR = $(EPOCROOT)epoc32/data/z/system/data
	TARGETDIR2=$(EPOCROOT)epoc32/data/z/private/101f401d
endif

TARGET_FILES = $(TARGETDIR)/oldLogdbu.dat \
			   $(TARGETDIR)/corruptLogdbu.dat \
			   $(TARGETDIR)/corruptDamagedLogdbu.dat \
			   $(TARGETDIR)/101f401d.txt \
			   $(TARGETDIR2)/CntModel.ini \
			   $(TARGETDIR2)/SQLite__Contacts.cdb
			   

$(TARGETDIR)/oldLogdbu.dat : $(EXTENSION_ROOT)/../test/src/oldLogdbu.dat $(TARGETDIR)
	$(call forcecopy,$(EXTENSION_ROOT)/../test/src/oldLogdbu.dat,$(TARGETDIR)/oldLogdbu.dat)
$(TARGETDIR)/corruptLogdbu.dat : $(EXTENSION_ROOT)/../test/src/corruptLogdbu.dat
	$(call forcecopy,$(EXTENSION_ROOT)/../test/src/corruptLogdbu.dat,$(TARGETDIR)/corruptLogdbu.dat)
$(TARGETDIR)/corruptDamagedLogdbu.dat : $(EXTENSION_ROOT)/../test/src/corruptDamagedLogdbu.dat
	$(call forcecopy,$(EXTENSION_ROOT)/../test/src/corruptDamagedLogdbu.dat,$(TARGETDIR)/corruptDamagedLogdbu.dat)
$(TARGETDIR)/101f401d.txt : $(EXTENSION_ROOT)/../test/101f401d.txt
	$(call forcecopy,$(EXTENSION_ROOT)/../test/101f401d.txt,$(TARGETDIR)/101f401d.txt)
$(TARGETDIR2)/CntModel.ini : $(EXTENSION_ROOT)/../test/src/CntModel.ini $(TARGETDIR2)
	$(call forcecopy,$(EXTENSION_ROOT)/../test/src/CntModel.ini,$(TARGETDIR2)/CntModel.ini)
$(TARGETDIR2)/SQLite__Contacts.cdb : $(EXTENSION_ROOT)/../test/src/SQLite__Contacts.cdb $(TARGETDIR2)
	$(call forcecopy,$(EXTENSION_ROOT)/../test/src/SQLite__Contacts.cdb,$(TARGETDIR2)/SQLite__Contacts.cdb)


DO_NOTHING :
	@echo do nothing

#
# The targets invoked by bld...
#

MAKMAKE : DO_NOTHING

RESOURCE : DO_NOTHING

SAVESPACE : DO_NOTHING

BLD : $(TARGET_FILES)

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : DO_NOTHING

CLEAN :
	$(call forceremove,$(TARGET_FILES))

RELEASABLES :
	@echo $(TARGETDIR)/oldLogdbu.dat
	@echo $(TARGETDIR)/corruptLogdbu.dat
	@echo $(TARGETDIR)/corruptDamagedLogdbu.dat
	@echo $(TARGETDIR)/101f401d.txt
	@echo $(TARGETDIR2)/CntModel.ini
	@echo $(TARGETDIR2)/SQLite__Contacts.cdb
