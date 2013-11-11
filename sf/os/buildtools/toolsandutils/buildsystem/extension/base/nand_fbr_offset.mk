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
#

# To guarantee there is a slash at the end of EPOCROOT in case there is not.
# This is needed to ensure compatibility with SBSv1. 
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


# Build directory (EPOCBLD too long)
BUILDLOC = $(EXTENSION_ROOT)/drivers

## Set the target name
SRC := $(SRCDIR)/nand_fbr_offset.h
TARGET = $(TARGETDIR)/nand_fbr_offset.inc



#Include processing
FULLINCLUDES := $(addprefix $(SRCDIR)/,$(INCLUDES))
#Object processing
FULLSRC := $(addprefix $(SRCDIR)/,$(SRC))

ifdef PROCESS_INCLUDES
GCCSRC := $(addprefix $(BUILDLOC)/,$(SRC))
FULLINCLUDES := $(foreach f,$(FULLINCLUDES),$(basename $(f)).$(INCEXT))
#Creation of headers
$(FULLINCLUDES) : $(SRCDIR)/%.$(INCEXT) : %.inc
	$(do_headers)
else
#Armasm sytax specifc asm rule goes here
endif


#Rules
#vpath %.inc . $(SRCDIR) 


MAKMAKE :
	perl $(EPOCROOT)epoc32/tools/h2inc.pl $(SRC) $(TARGET) ARMASM
#	echo Nothing to do

FREEZE :
	echo Nothing to do

LIB :
	echo Nothing to do

CLEANLIB :
	echo Nothing to do

RESOURCE :
	echo Nothing to do

FINAL :
	echo Nothing to do



BLD SAVESPACE : 
	echo Nothing to do

RELEASABLES :
	@echo "$(TARGET)"

CLEAN :
	-$(ERASE) $(call slash2generic,"$(TARGET)") 
	-$(ERASE) $(call slash2generic,"$(BUILDLOC)/$(TARGET)") 
#	-$(ERASE) "$(TARGET)")
#	-$(ERASE) "$(BUILDLOC)/$(TARGET)")

