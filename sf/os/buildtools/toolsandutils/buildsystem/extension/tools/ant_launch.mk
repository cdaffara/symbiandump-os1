# Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk
EXTENSION_ROOT:=$(lastword $(subst :, ,$(EXTENSION_ROOT)))
BUILDDIR = $(EPOCROOT)epoc32/build$(EXTENSION_ROOT)/$(PLATFORM_PATH)/$(CFG_PATH)
RELDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
LIBDIR = $(ANT_HOME)/bin/ant -lib $(EPOCROOT)epoc32/tools/java
BLDFILE = -buildfile $(EXTENSION_ROOT)/${build_xml}

BUILD = $(BUILDDIR)/${notdir ${build_xml}}_${target}
HAS_BUILT = ${wildcard ${BUILD}}

DO_NOTHING :
	@echo Nothing to do

$(BUILDDIR):
	-$(MKDIR) $(call slash2generic,$$@)

SAVESPACE : BLD	
MAKMAKE FREEZE LIB CLEANLIB RESOURCE FINAL : DO_NOTHING


## Find configuration
ifeq ($(CFG),REL)
RELEASE = REL
endif
ifeq ($(CFG),UREL)
RELEASE = UREL
endif

## Build target
ifeq ($(CFG),$(RELEASE))
ifeq ($(strip $(HAS_BUILT)), )

BLD : $(EXTENSION_ROOT)/${build_xml} $(BUILDDIR)
	$(LIBDIR) -logger com.symbian.ant.ScanLogger $(BLDFILE) -Depoc.build=$(BUILDDIR) -Depoc.rel=$(RELDIR) -Dtools.rel=$(EPOCROOT)tools ${args} ${target}
	echo $(BUILDDIR)/${build_xml} ${target} > ${BUILD}
else
BLD : ${BUILD}
	@echo The java component ${target} has already been built. Clean to rebuild.
endif
else
BLD : DO_NOTHING
endif


## Allow for a abld reallyclean
ifeq ($(CFG),$(RELEASE))
ifneq ($(strip $(HAS_BUILT)), )

CLEAN : ${BUILD}
	$(LIBDIR) -logger com.symbian.ant.ScanLogger $(BLDFILE) -Depoc.build=$(BUILDDIR) -Depoc.rel=$(RELDIR) -Dtools.rel=$(EPOCROOT)tools clean 
	${RM} ${BUILD}
else
CLEAN : DO_NOTHING
endif
else
CLEAN : DO_NOTHING
endif


## Allow for a abld -what
RELEASABLES :
	$(LIBDIR) -logger com.symbian.ant.AbldWhatLogger -quiet $(BLDFILE) -Depoc.rel=$(RELDIR) -Dtools.rel=$(EPOCROOT)tools what
