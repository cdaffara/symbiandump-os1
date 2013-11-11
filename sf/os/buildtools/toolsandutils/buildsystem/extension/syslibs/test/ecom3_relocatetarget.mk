# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Different from ecom3_postbuild.mk in that on armv5 the RAMONLYTARGETDIR
# is not \epoc32\data\z\ramonly. It is \epoc32\release\armv5\<CFG>\z\ramonly.
# This template has to preserve the udeb/urel targets.
# Params:
# SOURCES - list of .exe and .dll files to relocate
# TARGET - not used
# OPTION TARGETDIR - mandatory, it is "ramonly" for ecom testing.
# OPTION TARGETBASE - optional, overrides \epoc32\release\<platform>\<cfg>\z 
# OPTION SOURCEDIR -  optional, overrides \epoc32\release\<platform>\<cfg> 
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

# $(/) is actually back slash in Windows environment. Since bld.inf are written
# with forward slashes and $(CP) is "copy", this substitution is important.
TARGETDIR := $(subst /,$(/),$(TARGETDIR))

ifdef TARGETBASE
TARGETBASE := $(subst PLATFORM,$(PLATFORM_PATH),$(TARGETBASE))
TARGETBASE := $(subst CFG,$(CFG_PATH),$(TARGETBASE))
TARGETBASE := $(subst /,$(/),$(TARGETBASE))
DESTDIR:=$(TARGETBASE)/$(TARGETDIR)
else
DESTDIR:=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/$(TARGETDIR)
endif

ifdef SOURCEDIR
SOURCEDIR := $(subst PLATFORM,$(PLATFORM_PATH),$(SOURCEDIR))
SOURCEDIR := $(subst CFG,$(CFG_PATH),$(SOURCEDIR))
SOURCEDIR := $(subst /,$(/),$(SOURCEDIR))
else
SOURCEDIR := $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
endif

TARGET_COPY := $(foreach f,$(SOURCES),$(DESTDIR)/$(f) )

$(DESTDIR) :
	$(call createdir,"$@")

$(TARGET_COPY) :
	$(CP) $(call slash2generic, $(SOURCEDIR)/$(notdir $@) $@)
	-$(ERASE) $(call slash2generic, $(SOURCEDIR)/$(notdir $@))

#
# The targets invoked by abld...
#
BLD : $(DESTDIR) $(TARGET_COPY)

MAKMAKE SAVESPACE FREEZE LIB CLEANLIB RESOURCE FINAL :
	@echo do nothing

CLEAN : 
	-$(ERASE) $(TARGET_COPY)

RELEASABLES : 
	@echo $(TARGET_COPY)

