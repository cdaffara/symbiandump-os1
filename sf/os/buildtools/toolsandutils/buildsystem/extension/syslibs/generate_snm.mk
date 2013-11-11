# Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# basic_snm.mk
# 
#

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


ifeq ($(PLATFORM),WINS) 
        TARGET_DIRECTORY := $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/resource/charconv
else 
ifeq ($(PLATFORM),WINSCW)
        TARGET_DIRECTORY := $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/resource/charconv
else
        TARGET_DIRECTORY := $(EPOCROOT)epoc32/data/z/resource/charconv
endif
endif

SOURCE_DIRECTORY=$(EXTENSION_ROOT)

TARGET_FILE=\
        $(TARGET_DIRECTORY)/$(TARGET)

TOOLS=\
        $(EPOCROOT)epoc32/tools/PARSER.pm \
        $(EPOCROOT)epoc32/tools/WRITER.pm \
        $(EPOCROOT)epoc32/tools/snmtool.pl

$(TARGET_DIRECTORY) :
	@$(call createdir,"$@")

$(TARGET_DIRECTORY)/basic.snm : $(SOURCE_DIRECTORY)/$(SOURCES) $(TOOLS)
	perl $(EPOCROOT)epoc32/tools/snmtool.pl $(SOURCE_DIRECTORY)/$(SOURCES) $@

DO_NOTHING :
	@echo do nothing

# the targets below are the public ones

MAKMAKE : DO_NOTHING

BLD : $(TARGET_DIRECTORY) $(TARGET_FILE)

SAVESPACE : BLD

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

RESOURCE : DO_NOTHING

CLEAN : 
	-$(ERASE) $(TARGET_FILE)

RELEASABLES : 
	@echo $(TARGET_FILE)

FINAL : DO_NOTHING

