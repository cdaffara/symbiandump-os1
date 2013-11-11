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
# Copy SQLITE3_SECURE test files
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	TARGETDIR=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/private/10286A82
else
	TARGETDIR=$(EPOCROOT)epoc32/data/z/private/10286A82

endif

SOURCEDIR = $(EXTENSION_ROOT)/../TEST/TCLSCRIPT

$(TARGETDIR):
	$(call createdir, "$@")

COPYFILES :
	$(call forcecopy,$(SOURCEDIR)/*.test,$(TARGETDIR)/)
	$(call forcecopy,$(SOURCEDIR)/tester.tcl,$(TARGETDIR)/)

	
DO_NOTHING :
	@echo do nothing

#
# The targets invoked by bld...
#

MAKMAKE : DO_NOTHING

BLD : $(TARGETDIR) $(SOURCEDIR) COPYFILES

CLEAN : 
	$(call forceremove,$(TARGETDIR)/*.test)
	$(call forceremove,$(TARGETDIR)/tester.tcl)
	
SAVESPACE : DO_NOTHING

RESOURCE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : DO_NOTHING

RELEASABLES : 
	@echo $(TARGETDIR)/*.test
	@echo $(TARGETDIR)/tester.tcl
