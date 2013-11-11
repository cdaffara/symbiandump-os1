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


XINCDIR := $(INC_PATH)/omap_hrp/h4
XINCDIR2 := $(INC_PATH)/omap_hrp/assp/omap24xx

MAKMAKE : all

FREEZE :

LIB : all

CLEANLIB :

RESOURCE :

FINAL :

BLD SAVESPACE : all

RELEASABLES :
	@echo $(XINCDIR)/nand_fbr_offset.inc
	@echo $(XINCDIR2)/omap24xxconst.inc

CLEAN :
	-$(ERASE) $(call slash2generic,$(XINCDIR)/nand_fbr_offset.inc) 
#	-$(ERASE) $(XINCDIR)/nand_fbr_offset.inc
	@echo $(XINCDIR2)/omap24xxconst.inc

all: $(XINCDIR2)/omap24xxconst.inc $(XINCDIR)/nand_fbr_offset.inc 

$(XINCDIR)/nand_fbr_offset.inc : $(XINCDIR)/nand_fbr_offset.h
	perl $(EPOCROOT)epoc32/tools/h2inc.pl $(XINCDIR)/nand_fbr_offset.h $(XINCDIR)/nand_fbr_offset.inc ARMASM

$(XINCDIR2)/omap24xxconst.inc : $(XINCDIR2)/omap24xxconst.h
	perl $(EPOCROOT)epoc32/tools/h2inc.pl $(XINCDIR2)/omap24xxconst.h $(XINCDIR2)/omap24xxconst.inc ARMASM
