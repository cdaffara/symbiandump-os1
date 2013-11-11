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
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
CENTREPSRVSRCDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
CENTREPSRVTGTDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/sys/bin

FILE = centralrepositorysrv.exe

$(CENTREPSRVTGTDIR) :
	$(call createdir, "$(CENTREPSRVTGTDIR)")

COPYFILE : $(FILE)
	$(call forcecopy,$(CENTREPSRVSRCDIR)/$^,$(CENTREPSRVTGTDIR))

$(FILE):
endif

DO_NOTHING :
	@echo do nothing

#
# The targets invoked by bld...
#

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
BLD : $(CENTREPSRVTGTDIR) $(CENTREPSRVSRCDIR) COPYFILE

CLEAN : $(FILE)
	$(call forceremove,$(CENTREPSRVTGTDIR)/$^)

RELEASABLES : $(FILE)
	@echo $(CENTREPSRVTGTDIR)/$^

else
BLD : DO_NOTHING

CLEAN : DO_NOTHING

RELEASABLES : DO_NOTHING
endif

MAKMAKE : DO_NOTHING

SAVESPACE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FREEZE : DO_NOTHING

RESOURCE : DO_NOTHING

FINAL : DO_NOTHING

ROMFILE : DO_NOTHING
