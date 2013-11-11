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
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/
include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

## This macro enables benchmarking code. Comment out or set FALSE if not required
# WRITE_TIMINGS := TRUE

ifneq "$(WRITE_TIMINGS)" "TRUE"
        WRITE_TIMINGS := FALSE
endif

#Set the directories
GENSRCDIR := $(EXTENSION_ROOT)/../../../kernelhwsrv/kernel/eka/drivers/unistore2/srca/xsr/util/ONBL1
SPECSRCDIR := $(EXTENSION_ROOT)/nandboot/miniboot_onenand

# Build directory (EPOCBLD too long)
BUILDLOC = $(EPOCROOT)epoc32/build/omap_hrp/h4_restricted/unistore2/nandboot/miniboot/$(PLATFORM_PATH)

# Set the target name
TARGETDIR := $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)
TARGET = $(TARGETDIR)$/h4hrp_un2_on_miniboot.bin
TMPTARGET = $(BUILDLOC)$/h4hrp_un2_on_miniboot.elf

#Rules
vpath %.s . $(GENSRCDIR) $(SPECSRCDIR) 

GENASMSOURCE := onbl1.s
ASMSOURCE := miniboot_onenand.s

ARMASM_OUT := $(shell armasm 2>&1)
ARMASM_OUT_4 := $(word 4,$(ARMASM_OUT))

# Select the toolchain: ARM RVCT, then GCC
#TOOLVER := RVCT
OP := --
OB := o


ifeq "$(WRITE_TIMINGS)" "TRUE"
	ASM_TRUE_MACROS += WRITE_TIMINGS
endif 

ifeq "$(WRITE_TIMINGS)" "FALSE"
	ASM_FALSE_MACROS += WRITE_TIMINGS
endif 

#Arm RVCT tools
ASM_TRUE_MACROS += USE_CXSF
ASM := armasm
LINK := armlink
FROMELF := fromelf

OBJEXT := o

ASM_TRUE_MACRO_CMD := $(foreach macro,$(ASM_TRUE_MACROS),$(OP)predefine "$(macro) SETL {TRUE}")
ASM_FALSE_MACRO_CMD := $(foreach macro,$(ASM_FALSE_MACROS),$(OP)predefine "$(macro) SETL {FALSE}")

AFLAGS := -g $(OP)keep $(ASM_TRUE_MACRO_CMD) $(ASM_FALSE_MACRO_CMD) -I$(BUILDLOC)
LFLAGS := $(OP)entry BootEntry $(OP)FIRST BootEntry $(OP)map
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

ASMOBJECTS := $(foreach f,$(ASMSOURCE),$(basename $(f)).$(OBJEXT))
FULLASMOBJECTS := $(addprefix $(BUILDLOC)/,$(ASMOBJECTS))

GENASMOBJECTS := $(foreach f,$(GENASMSOURCE),$(basename $(f)).$(OBJEXT))
FULLGENASMOBJECTS := $(addprefix $(BUILDLOC)/,$(GENASMOBJECTS))

FULLOBJECTS := $(FULLASMOBJECTS) $(FULLGENASMOBJECTS)


#Link
$(TMPTARGET) : $(FULLOBJECTS)
	$(do_link)

#strip
$(TARGET) : $(TMPTARGET)
	$(do_strip)

#Asm objects
$(FULLGENASMOBJECTS) : $(BUILDLOC)/%.$(OBJEXT) : $(GENSRCDIR)/$(GENASMSOURCE) $(FULLINCLUDES) $(FULLBUILTINCLUDES) $(FULLBUILTINCLUDES2) $(FULLBUILTINCLUDES3) $(FULLDRIVERINCLUDES) $(FULLARMINCLUDES) $(FULLBLDINCLUDES) $(FULLGENINCLUDES) $(FULLPLATINCLUDES)
	$(do_asm)

$(FULLASMOBJECTS) : $(BUILDLOC)/%.$(OBJEXT) : $(SPECSRCDIR)/$(ASMSOURCE) $(FULLINCLUDES) $(FULLBUILTINCLUDES) $(FULLBUILTINCLUDES2) $(FULLBLDINCLUDES) $(FULLGENINCLUDES) $(FULLPLATINCLUDES)
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

