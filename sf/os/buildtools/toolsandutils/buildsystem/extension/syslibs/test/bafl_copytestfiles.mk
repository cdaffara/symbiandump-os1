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
# Copy test files to test sortbytable functionality.
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
EPOCDATADIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
else
EPOCDATADIR=$(EPOCROOT)epoc32/data
endif

TARGETDIR=$(EPOCDATADIR)/z/system/documents/baflsortbytabletest
SOURCEDIR=$(EXTENSION_ROOT)/../tsrc
	
COPYFILES :
	$(CP) $(call slash2generic,$(SOURCEDIR)/ADDCERT.RSC $(TARGETDIR)/addcert.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/HELP.RSC $(TARGETDIR)/help.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/MSGEDITOR.RSC $(TARGETDIR)/msgeditor.rsc)
	$(CP) $(call slash2generic,$(SOURCEDIR)/SMLPROGRESS.RSC $(TARGETDIR)/smlprogress.rsc)

$(TARGETDIR) :
	$(call createdir,"$@")
	
DO_NOTHING :
	@echo do nothing
	
#
# The targets invoked by bld...
#

BLD : $(TARGETDIR) COPYFILES

CLEAN :  
	-$(ERASE) $(call slash2generic,$(TARGETDIR)/addcert.rsc)
	-$(ERASE) $(call slash2generic,$(TARGETDIR)/help.rsc)
	-$(ERASE) $(call slash2generic,$(TARGETDIR)/msgeditor.rsc)
	-$(ERASE) $(call slash2generic,$(TARGETDIR)/smlprogress.rsc)
	
RELEASABLES :
	@echo $(TARGETDIR)/addcert.rsc
	@echo $(TARGETDIR)/help.rsc
	@echo $(TARGETDIR)/msgeditor.rsc
	@echo $(TARGETDIR)/smlprogress.rsc
 
MAKMAKE : DO_NOTHING

SAVESPACE : DO_NOTHING

RESOURCE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : DO_NOTHING
