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

# Get standard shell definitions
include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

LETTERS:=a b c d e f g h i j k l m n o p q r s t u v w x y z A B C D E F G H I J K L M N O P Q R S T U V W X Y Z

# Raptor includes the drive letter on paths - this makefile can't handle the drive letter
# If we don't do this we get "multiple target patterns" errors in sbsv2
TMPROOT:=$(strip $(foreach let,$(LETTERS),$(if $(findstring $(let):,$(EPOCROOT)),$(subst $(let):,,$(EPOCROOT)))))
ifneq ($(TMPROOT),)
	EPOCROOT:=$(TMPROOT)
endif
TMPROOT:=$(strip $(foreach let,$(LETTERS),$(if $(findstring $(let):,$(EXTENSION_ROOT)),$(subst $(let):,,$(EXTENSION_ROOT)))))
ifneq ($(TMPROOT),)
	EXTENSION_ROOT:=$(TMPROOT)
endif


BUILDDIR = $(EPOCROOT)epoc32/build/$(EXTENSION_ROOT)/$(PLATFORM_PATH)
RELDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)
LIBDIR = $(ANT_HOME)/bin/ant -lib $(EPOCROOT)epoc32/tools/java
BLDFILE = -buildfile $(EXTENSION_ROOT)/build.xml

BUILD = $(BUILDDIR)/${notdir ${build_xml}}_${target}
HAS_BUILT = ${wildcard ${BUILD}}

DO_NOTHING :
	@echo Nothing to do

SAVESPACE : BLD	
MAKMAKE FREEZE CLEANLIB RESOURCE FINAL : DO_NOTHING

BLD: DO_NOTHING

LIB:
	@$(call createdir,"$(BUILDDIR)")
	$(LIBDIR) -logger com.symbian.ant.ScanLogger $(BLDFILE) -Depoc.build=$(BUILDDIR) -Depoc.rel=$(RELDIR) -Dtools.rel=$(EPOCROOT)tools ${args} ${target}
	echo $(BUILDDIR)/${build_xml} ${target} > ${BUILD}	

## Allow for a abld reallyclean
CLEAN :
	$(LIBDIR) -logger com.symbian.ant.ScanLogger $(BLDFILE) -Depoc.build=$(BUILDDIR) -Depoc.rel=$(RELDIR) -Dtools.rel=$(EPOCROOT)tools clean 

## Allow for a abld -what
RELEASABLES : 
	$(LIBDIR) -logger com.symbian.ant.AbldWhatLogger -quiet $(BLDFILE) -Depoc.rel=$(RELDIR) -Dtools.rel=$(EPOCROOT)tools what
