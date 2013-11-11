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
	EPOCDATADIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
else
	EPOCDATADIR=$(EPOCROOT)epoc32/data
endif

TARGETDIR = $(EPOCDATADIR)/z/resource
SOURCEDIR = $(EXTENSION_ROOT)/../data/certstore

FILE = swicertstore.dat

$(TARGETDIR) : 
	$(call createdir, "$@")

COPYFILES : $(FILE)
	
	$(call forcecopy,$(SOURCEDIR)/$^,$(TARGETDIR)/$^)

$(FILE) :

DO_NOTHING:
	@echo do nothing

#
# The targets invoked by bld...
#

BLD :	$(TARGETDIR) COPYFILES

CLEAN :
	$(call forceremove,$(TARGETDIR)/swicertstore.dat)

RELEASABLES :
	@echo $(TARGETDIR)/swicertstore.dat

MAKMAKE : DO_NOTHING

SAVESPACE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FREEZE : DO_NOTHING

RESOURCE : DO_NOTHING

FINAL : DO_NOTHING

ROMFILE : DO_NOTHING
