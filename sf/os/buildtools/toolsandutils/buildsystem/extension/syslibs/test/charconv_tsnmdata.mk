# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	TARGET_DIRECTORY = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/resource/charconv
else
	TARGET_DIRECTORY = $(EPOCROOT)epoc32/data/z/resource/charconv
endif

TARGET_FILES = $(TARGET_DIRECTORY)/tsnm.snm

SOURCE_DIRECTORY = $(EXTENSION_ROOT)/../test/data/main

TOOLS = \
	$(EPOCROOT)epoc32/tools/PARSER.PM \
	$(EPOCROOT)epoc32/tools/WRITER.PM \
	$(EPOCROOT)epoc32/tools/snmtool.pl

$(TARGET_DIRECTORY) :
	$(call createdir,"$@")

$(TARGET_FILES) : $(SOURCE_DIRECTORY)/tsnm.txt $(TOOLS)
	perl $(EPOCROOT)epoc32/tools/snmtool.pl $(SOURCE_DIRECTORY)/tsnm.txt $@
DO_NOTHING :
	@echo do nothing
	
#
# The targets invoked by bld...
#

MAKMAKE : DO_NOTHING

BLD : $(TARGET_DIRECTORY) $(TARGET_FILES)

SAVESPACE : BLD

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

RESOURCE : DO_NOTHING

CLEAN : 
	-$(ERASE) $(TARGET_FILES)

RELEASABLES : 
	@echo $(TARGET_FILES)

FINAL : DO_NOTHING
