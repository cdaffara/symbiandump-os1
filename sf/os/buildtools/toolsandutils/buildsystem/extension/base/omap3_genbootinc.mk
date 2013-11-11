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
# Generate assembler inc files from header files
# 
#

# To guarantee there is a slash at the end of EPOCROOT in case there is not.
# This is needed to ensure compatibility with SBSv1. 
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

# This will need to change when the directory gets renamed
XINCDIR := $(INC_PATH)/tiomap3/variant/common
XINCDIR2 := $(INC_PATH)/tiomap3/assp/common


MAKMAKE : all

FREEZE :

LIB : all

CLEANLIB :

RESOURCE :

FINAL :

BLD SAVESPACE : all

RELEASABLES :
	@echo $(XINCDIR)/nand_fbr_offset.inc
	@echo $(XINCDIR2)/customrestartreasons.inc

CLEAN :
	-$(ERASE) $(call slash2generic,$(XINCDIR)/nand_fbr_offset.inc) 
#	-$(ERASE) $(XINCDIR)/nand_fbr_offset.inc
	@echo $(XINCDIR2)/customrestartreasons.inc

all: $(XINCDIR2)/customrestartreasons.inc $(XINCDIR)/nand_fbr_offset.inc 

$(XINCDIR)/nand_fbr_offset.inc : $(XINCDIR)/nand_fbr_offset.h
	perl $(EPOCROOT)epoc32/tools/h2inc.pl $(XINCDIR)/nand_fbr_offset.h $(XINCDIR)/nand_fbr_offset.inc ARMASM

$(XINCDIR2)/customrestartreasons.inc : $(XINCDIR2)/customrestartreasons.h
	perl $(EPOCROOT)epoc32/tools/h2inc.pl $(XINCDIR2)/customrestartreasons.h $(XINCDIR2)/customrestartreasons.inc ARMASM
