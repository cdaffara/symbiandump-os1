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
# h4_genbootinc.mk
# Generate assembler inc files from header files
#

# To guarantee there is a slash at the end of EPOCROOT in case there is not.
# This is needed to ensure compatibility with SBSv1. 
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


XINCDIR := $(INC_PATH)/assp/naviengine
XGENDIR := $(INC_PATH)/assp/naviengine/$(MEMMODEL)

MAKMAKE : all

FREEZE :

LIB : all

CLEANLIB :

RESOURCE :

FINAL :

BLD SAVESPACE : all

RELEASABLES :
	@echo $(XGENDIR)/naviengine.inc

CLEAN :
	-$(ERASE) $(call slash2generic,$(XGENDIR)/naviengine.inc) 
	@echo $(XGENDIR)/naviengine.inc

all: $(XGENDIR)/naviengine.inc

$(XGENDIR)/naviengine.inc : $(XINCDIR)/naviengine.h
	perl $(EPOCROOT)epoc32/tools/h2inc.pl $(XINCDIR)/naviengine.h $(XGENDIR)/naviengine.inc ARMASM

