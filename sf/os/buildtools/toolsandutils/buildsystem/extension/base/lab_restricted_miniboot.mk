# lab_restricted_miniboot.mk
#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/
include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

#Set the directories
SRCDIR := $(EXTENSION_ROOT)/../../lab_restricted/unistore2/nandboot/miniboot_largeblk

# Build directory (EPOCBLD too long)
BUILDLOC = $(EPOCROOT)epoc32/build/lab_restricted/unistore2/nandboot/miniboot/$(PLATFORM_PATH)

# Set the target name
TARGETDIR := $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)
TARGET = $(TARGETDIR)$/lab_miniboot.bin
TMPTARGET = $(BUILDLOC)$/lab_miniboot.elf
TMPTARGET2 = $(BUILDLOC)$/lab_miniboot.bin

# Set the Load Address for the miniboot
# This is currently set to the beginning of SRAM
LINKBASE=0x40200000

#Rules
vpath %.s . $(SRCDIR)

ASMSOURCE := miniboot_largeblk.s

ARMASM_OUT := $(shell armasm 2>&1)
ARMASM_OUT_4 := $(word 4,$(ARMASM_OUT))

# Select the toolchain: ARM RVCT, then GCC
#TOOLVER := RVCT
OP := --
OB := o

#Arm RVCT tools
ASM_TRUE_MACROS += USE_CXSF
ASM := armasm
LINK := armlink
FROMELF := fromelf

OBJEXT := o

ASM_TRUE_MACRO_CMD := $(foreach macro,$(ASM_TRUE_MACROS),$(OP)predefine "$(macro) SETL {TRUE}")
ASM_FALSE_MACRO_CMD := $(foreach macro,$(ASM_FALSE_MACROS),$(OP)predefine "$(macro) SETL {FALSE}")

AFLAGS := -g $(OP)keep $(ASM_TRUE_MACRO_CMD) $(ASM_FALSE_MACRO_CMD) -I$(BUILDLOC)
LFLAGS := $(OP)ro-base $(LINKBASE) $(OP)entry BootEntry $(OP)FIRST BootEntry $(OP)map
SYMOPT := $(OP)symdefs
ASMTYP := ARMASM

define do_asm
	$(ASM) $(AFLAGS) -$(OB) $@ $(OP)LIST $(join $(basename $@),.lst) $<
endef

define do_link
	$(LINK) $(LFLAGS) -$(OB) $@ $(FULLOBJECTS)
endef

define do_strip
	$(FROMELF) $(OP)bin $(OP)output $@ $<
endef

define do_omapsig
	perl -S $(EPOCROOT)epoc32/tools/omapsig.pl $(LINKBASE) $< $@
endef

ASMOBJECTS := $(foreach f,$(ASMSOURCE),$(basename $(f)).$(OBJEXT))
FULLASMOBJECTS := $(addprefix $(BUILDLOC)/,$(ASMOBJECTS))

FULLOBJECTS := $(FULLASMOBJECTS)


#Link
$(TMPTARGET) : $(FULLOBJECTS)
	$(do_link)

#strip
$(TMPTARGET2) : $(TMPTARGET)
	$(do_strip)

#omapsig
$(TARGET) : $(TMPTARGET2)
	$(do_omapsig)

#Asm objects

$(FULLASMOBJECTS) : $(BUILDLOC)/%.$(OBJEXT) : $(SRCDIR)/$(ASMSOURCE) $(FULLINCLUDES) $(FULLBUILTINCLUDES) $(FULLBUILTINCLUDES2) $(FULLBLDINCLUDES) $(FULLGENINCLUDES) $(FULLPLATINCLUDES)
	$(do_asm)

# make the work directories
$(TARGETDIR) :
	$(call ifnotexistd,"$(TARGETDIR)")
   
$(BUILDLOC) :
	$(call ifnotexistd,"$(BUILDLOC)")
	


MAKMAKE :
	echo Nothing to do

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

BLD SAVESPACE : $(TARGETDIR) $(BUILDLOC) $(TARGET)

RELEASABLES :
	@echo "$(TARGET)"

CLEAN :
	-$(ERASE) $(call slash2generic,"$(TARGET)")
	-$(ERASE) $(call slash2generic,"$(BUILDLOC)/*.*")

