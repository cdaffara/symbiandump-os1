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
# template.mk
# Build Word template files
# 
#
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	TARGETDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/private/10003a64
else
	# The IBY file uses the template file stored here when building ROMs
	TARGETDIR = $(EPOCROOT)epoc32/data/z/private/10003a64
endif

TEMPLATES = $(TARGETDIR)/blank

$(TARGETDIR) :
	$(call createdir, "$@")

# Well, actually just copy the prebuilt ones for now...
#  - deleting existing file first (in case it's read-only)

TEMPLATESRCDIR = $(EXTENSION_ROOT)/../utemplat

$(TEMPLATES) : $(TEMPLATESRCDIR)$/BLANK.UK $(TARGETDIR)
	$(call forceremove,$@)
	$(call forcecopy,$(TEMPLATESRCDIR)/BLANK.UK,"$@")
DO_NOTHING :
	@echo do nothing

# The targets invoked by bld...

MAKMAKE : DO_NOTHING

RESOURCE : DO_NOTHING

SAVESPACE : BLD

BLD : $(TARGETDIR) $(TEMPLATES)

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : DO_NOTHING

CLEAN :
	-$(ERASE) $(TEMPLATES)

RELEASABLES :
	@echo $(TEMPLATES)
