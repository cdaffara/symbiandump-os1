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
# Build Upgrade SIS file
# 
#

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


# Select appropriate directory and ensure it exists

TARGETDIR=$(EPOCROOT)epoc32/data/z/private/$(UID_DIR)
ifeq ($(PLATFORM),WINS) 
        TARGETDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/private/$(UID_DIR)
else
ifeq ($(PLATFORM),WINSCW)
        TARGETDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/private/$(UID_DIR)
endif
endif

$(TARGETDIR) :
	@$(call createdir,"$(TARGETDIR)") 

# Build stub SIS file

SISFILE= $(TARGETDIR)/$(SISNAME).sis

$(SISFILE) : $(EXTENSION_ROOT)/$(SRCDIR)/$(SISNAME).pkg
	$(EPOCROOT)epoc32/tools/makesis -d$(EXTENSION_ROOT)/$(SRCDIR) $? $@


do_nothing :
# do_nothing

# The targets invoked by abld

MAKMAKE : do_nothing

RESOURCE : $(TARGETDIR) $(SISFILE)

SAVESPACE : BLD

BLD : do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

FINAL : do_nothing

CLEAN :
	-$(ERASE) $(SISFILE)

RELEASABLES :
	@echo $(SISFILE)
