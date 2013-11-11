# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# This postbuild script is only called when a Test Build is done.  It deletes the KDDI/AU versions of the shiftjis and j5 plugin RSC file, so
# that only the Docomo versions remain.  This removed ambiguity over which one will load during tests - a seperate version of the KDDI/AU
# plugins is created for test build, with a unique UID number so they may coexist for test purposes.
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

# Only remove files on emulator build - for hardware, iby file selects correct plugin.
ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	TARGETDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/resource/plugins

	TARGET_FILES = \
	$(TARGETDIR)/shiftjis_kddiau.rsc \
	$(TARGETDIR)/j5_kddiau.rsc \
	$(TARGETDIR)/eucjp_packed_2.rsc \
	$(TARGETDIR)/iso2022jp_2.rsc \
	$(TARGETDIR)/iso2022jp1_2.rsc \
	$(TARGETDIR)/j5_kddiau_2.rsc \
	$(TARGETDIR)/jis_2.rsc\
	$(TARGETDIR)/shiftjis_kddiau_2.rsc
	
	TARGET_FILES:=$(subst /,\,$(TARGET_FILES))
endif

DO_NOTHING :
	@echo do nothing

#
# The targets invoked by bld...
#

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
BLD :	
	@echo Below rsc files will be deleted to remove ambiguity in testing:
	@echo $(TARGET_FILES)
	-$(ERASE) $(TARGET_FILES)	
else
BLD : DO_NOTHING
endif

RELEASABLES : DO_NOTHING

MAKMAKE : DO_NOTHING

CLEAN : DO_NOTHING

SAVESPACE : DO_NOTHING

RESOURCE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : DO_NOTHING
