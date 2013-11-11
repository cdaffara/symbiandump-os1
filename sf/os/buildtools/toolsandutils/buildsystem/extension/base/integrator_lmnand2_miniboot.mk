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


## Set the source directory
BLDINFDIR := ../../../unref/orphan/cedgen/base/integrator/.
SRCDIR = $(EXTENSION_ROOT)/nandboot/miniboot
SPECSRCDIR = $(EXTENSION_ROOT)/../integratormotherboard/nandboot/miniboot
TARGETDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)
EPOCINCDIR = $(EPOCROOT)epoc32/include/integrator/logic/lmnand2

#Rules
vpath %.s . $(SPECSRCDIR) $(SRCDIR)
vpath %.inc . $(SRCDIR) $(SPECSRCDIR) $(EPOCINCDIR)
vpath %.ginc . $(BUILDLOC)


# Build directory (EPOCBLD too long)
#BUILDLOC = $(EPOCROOT)epoc32/build/integrator/logic/lmnand2/nandboot/miniboot/$(PLATFORM_PATH)/$(CFG_PATH)
BUILDLOC = $(EPOCBLD)/$(PLATFORM_PATH)/$(CFG_PATH)

## Set the target name
INCLUDES := nand.inc
SPECINCLUDES := arm.inc
EPOCINCLUDES := nand_fbr_offset.inc

SRC := miniboot.s arm.s
TARGET = $(TARGETDIR)/_lmnand2_miniboot.bin
TMPTARGET = $(BUILDLOC)/miniboot.exe

# Set the code and data base addresses 
LINKBASE = 0x0000000

ARMASM_OUT := $(shell armasm 2>&1)
ARMASM_OUT_4 := $(word 4,$(ARMASM_OUT))
# Use GCC toolchain if no other is available
TOOLVER := GCC
RVCTSTR := $(strip $(findstring RVCT, $(ARMASM_OUT_4)))
ifeq "$(RVCTSTR)" "RVCT"
        TOOLVER := RVCT
        OP := --
        OB := o
endif 

ifeq "$(TOOLVER)" "RVCT"
        ASM_MACROS += USE_CXSF

        ASM := armasm
        LINK := armlink
        FROMELF := fromelf
        OBJEXT := o
        INCEXT := inc

        ASMINCPATHCMD := $(foreach dir,$(SRCDIR),$(join -I ,$(dir))) $(foreach dir,$(EPOCINCDIR),$(join -I ,$(dir)))
        SPECASMINCPATHCMD := $(foreach dir,$(SPECSRCDIR),$(join -I ,$(dir)))
        ASM_MACRO_CMD := $(foreach macro,$(ASM_MACROS),$(OP)predefine "$(macro) SETL {TRUE}")
        AFLAGS := -g $(OP)keep $(ASM_MACRO_CMD) $(ASMINCPATHCMD) $(SPECASMINCPATHCMD)
        
        SYMOPT := $(OP)symdefs
        ASMTYP := ARMASM
        LINKFILE :=
        define do_asm
                $(ASM) $(AFLAGS) -$(OB) $@ $(OP)LIST $(join $(basename $@),.lst) $<
        endef
        
        LFLAGS := $(OP)ro-base $(LINKBASE) $(OP)entry $(LINKBASE) $(OP)map

        define do_link
                $(LINK) $(LFLAGS) -$(OB) $@ $(filter %.$(OBJEXT),$^)
        endef

        define do_strip
                $(FROMELF) $(OP)bin $(OP)output $@ $<
        endef
endif


#GCC build options
ifeq "$(TOOLVER)" "GCC"
        ASM := as
        AFLAGS := -mapcs-32 -R -n -I$(BUILDLOC)

        LINKFLAGS = -n -x --section-alignment 4 --file-alignment 2 -no-whole-archive
        OBJEXT := o
        INCEXT := ginc
        LINKFILE = $(SRCDIR)/miniboot.lnk
        define do_includes
                perl $(EPOCROOT)epoc32/tools/armasm2as.pl $< $@
        endef
        define do_asm
                perl $(EPOCROOT)epoc32/tools/armasm2as.pl $< $(join $(basename $@),.s) 
                $(AS) $(AFLAGS) -o $@ $(join $(basename $@),.s)
        endef
        define do_link
                ld -o "$(TMPTARGET)"  --start $(FULLOBJECTS) --script=$(LINKFILE)
        endef
        define do_strip
                strip -O binary -o "$(TARGET)" "$(TMPTARGET)"
                $(ERASE) $(call slash2generic,"$(TMPTARGET)") 
	#	$(ERASE) "$(TMPTARGET)"
                echo Built $(TARGET)
        endef
        PROCESS_INCLUDES := 1
endif


OBJECTS := $(foreach f,$(SRC),$(basename $(f)).$(OBJEXT))

FULLOBJECTS := $(addprefix $(BUILDLOC)/,$(OBJECTS))


#Include processing


ifdef PROCESS_INCLUDES

GCCSRC := $(addprefix $(BUILDLOC)/,$(SRC))

FULLINCLUDES := $(foreach f,$(INCLUDES),$(basename $(f)).$(INCEXT))
FULLINCLUDES := $(addprefix $(BUILDLOC)/,$(FULLINCLUDES))

SPECFULLINCLUDES := $(foreach f,$(SPECINCLUDES),$(basename $(f)).$(INCEXT))
SPECFULLINCLUDES := $(addprefix $(BUILDLOC)/,$(SPECFULLINCLUDES))

EPOCFULLINCLUDES := $(foreach f,$(EPOCINCLUDES),$(basename $(f)).$(INCEXT))
EPOCFULLINCLUDES := $(addprefix $(BUILDLOC)/,$(EPOCFULLINCLUDES))

#Creation of headers
$(FULLINCLUDES) : $(BUILDLOC)/%.$(INCEXT) : %.inc
	$(do_includes)
$(SPECFULLINCLUDES) : $(BUILDLOC)/%.$(INCEXT) : %.inc
	$(do_includes)
$(EPOCFULLINCLUDES) : $(BUILDLOC)/%.$(INCEXT) : %.inc
	$(do_includes)

else

#Armasm sytax specifc asm rule goes here
FULLINCLUDES := $(addprefix $(SRCDIR)/,$(INCLUDES))
SPECFULLINCLUDES:= $(addprefix $(SPECSRCDIR)/,$(SPECINCLUDES))
EPOCFULLINCLUDES:= $(addprefix $(EPOCINCDIR)/,$(EPOCINCLUDES))

endif
 

#Strip
$(TARGET) : $(TMPTARGET)
	$(do_strip)

#Link	
$(TMPTARGET) : $(FULLOBJECTS)
	$(do_link)

#Objects
$(FULLOBJECTS): $(BUILDLOC)/%.$(OBJEXT): %.s $(FULLINCLUDES) $(SPECFULLINCLUDES)  $(EPOCFULLINCLUDES)
	$(do_asm)

# make directories
$(TARGETDIR):
	$(call ifnotexistd,"$(TARGETDIR)")

$(BUILDLOC):
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
#	-$(ERASE) "$(TARGET)"
#	-$(ERASE) "$(BUILDLOC)/*.*"

