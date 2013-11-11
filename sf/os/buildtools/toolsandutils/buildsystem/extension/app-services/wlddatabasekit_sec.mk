# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Builds the World Database
# 
#

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


TARGETDIR=$(EPOCROOT)epoc32/data/z/resource/worldserver

ifeq ($(PLATFORM), WINS)
        TARGETDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/resource/worldserver
else
ifeq ($(PLATFORM), WINSCW)
        TARGETDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/resource/worldserver
endif
endif

$(TARGETDIR) :
	@$(call createdir,"$(TARGETDIR)")

WLDCOMP=$(EPOCROOT)epoc32/release/winc/udeb/wldcomp.exe
DATABASE=$(TARGETDIR)/$(TARGET)

# We don't build multiple languages - that's a problem
# for someone else...

$(DATABASE) :  $(EXTENSION_ROOT)/$(SOURCES) $(WLDCOMP)
	$(WLDCOMP) $(EXTENSION_ROOT)/$(SOURCES) $(DATABASE) -locale-01

do_nothing :
# do nothing

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

BLD : $(TARGETDIR) $(DATABASE)

SAVESPACE : BLD

CLEAN :
	-$(ERASE) $(DATABASE)

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

FINAL : do_nothing

RELEASABLES : 
	@echo $(DATABASE)

ROMFILE : do_nothing
