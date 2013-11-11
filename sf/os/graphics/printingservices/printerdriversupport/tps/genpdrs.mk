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
# Build PDR files
# 
#


# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


ifeq ($(PLATFORM),WINS) 
        TARGETDIR:=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/resource/printers
else
ifeq ($(PLATFORM),WINSCW)
        TARGETDIR:=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/resource/printers
else
        TARGETDIR:=$(EPOCROOT)epoc32/data/z/resource/printers
endif
endif

$(TARGETDIR) :
	$(call createdir,"$(TARGETDIR)")

PDR=$(TARGETDIR)/$(PRINTER).pdr

$(PDR) : $(EXTENSION_ROOT)/../$(PRINTER_DIR)/$(PRINTER).PD
	$(EPOCROOT)epoc32/tools/pdrtran "$?" "$@"

do_nothing:
# do nothing

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

BLD : $(TARGETDIR) $(PDR)

SAVESPACE : BLD

RESOURCE : BLD

CLEAN :
	-$(ERASE) $(PDR)

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : do_nothing

RELEASABLES : 
	@echo $(PDR)


