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

ifeq ($(OS), "Windows_NT")
	RMDIR := @rmdir 2>>nul
	@echo $(OS))
else
	RMDIR := @rmdir
endif
ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	TARGETDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/xmlramonly
	SOURCEDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
	SOURCEDIR2 = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/resource/plugins
else
	TARGETDIR = $(EPOCROOT)epoc32/data/z/xmlramonly
	SOURCEDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
	SOURCEDIR2 = $(EPOCROOT)epoc32/data/z/resource/plugins
endif

# Ensure we have a clean canvas - this also avoids us having to specify
# switches to commands that are not recognised across platforms.
CLEAN_DEST :
	$(call createdir,$(TARGETDIR))

$(TARGETDIR) : 
	$(call createdir,"$@")

COPYFILES : CLEAN_DEST $(TARGETDIR)
	$(CP) $(call slash2generic,$(SOURCEDIR)/tu_xmlramparserplugins.dll  $(TARGETDIR)/tu_xmlramparserplugins.dll)
	$(CP) $(call slash2generic,$(SOURCEDIR2)/tu_xmlramparserplugins.RSC  $(TARGETDIR)/tu_xmlramparserplugins.rsc)
	-$(ERASE) $(call slash2generic,$(SOURCEDIR)/tu_xmlramparserplugins.dll)
	-$(ERASE) $(call slash2generic,$(SOURCEDIR2)/tu_xmlramparserplugins.rsc)

DO_NOTHING:
	@echo do nothing

#
# The targets invoked by bld...
#

MAKMAKE : DO_NOTHING

RESOURCE : DO_NOTHING

CLEAN :
	$(call createdir,$(TARGETDIR))

ifeq ($(PLATFORM), GCCXML)
BLD : DO_NOTHING
else
BLD : COPYFILES
endif

SAVESPACE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

RESOURCE : DO_NOTHING

FINAL : DO_NOTHING

RELEASABLES :
	@echo $(TARGETDIR)/tu_xmlramparserplugins.dll
	@echo $(TARGETDIR)/tu_xmlramparserplugins.rsc
