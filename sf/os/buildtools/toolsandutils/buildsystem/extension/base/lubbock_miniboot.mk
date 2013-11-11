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


# Set the source directory
SRCDIR = $(EXTENSION_ROOT)/nandboot/miniboot
TARGETDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)
EPOCINCDIR = $(EPOCROOT)epoc32/include/lubbock/nand

#Rules
vpath %.s $(SRCDIR)
vpath %.inc $(SRCDIR) $(EPOCINCDIR)
vpath %.ginc $(BUILDLOC)

# Build directory
BUILDLOC = $(EPOCBLD)$(call sbsadd,/base/lubbock)

## Set the target name
INCLUDES := nand.inc lubbock.inc
EPOCINCLUDES := nand_fbr_offset.inc
SRC := miniboot.s lubbock.s
TARGET=$(TARGETDIR)/lubbock_miniboot.bin
TMPTARGET = $(BUILDLOC)/miniboot.exe

# Set the code and data base addresses 
# Since miniboot is PIC this is just to placate the linker
LINKBASE = 0x0000000

ARMASM_OUT := $(shell armasm 2>&1)
ARMASM_OUT_4 := $(word 4,$(ARMASM_OUT))
# Use GCC toolchain if no other is available
# NB! Need to keep updating/extending allowable range of RVCT versions!!
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
        ASMINCFBRPATH := $(foreach fir,$(EPOCINCDIR),$(join -I ,$(fir)))
        ASM_MACRO_CMD := $(foreach macro,$(ASM_MACROS),$(OP)predefine "$(macro) SETL {TRUE}")
        AFLAGS := -g $(OP)keep $(ASM_MACRO_CMD) $(ASMINCPATHCMD) $(ASMINCFBRPATH)
        
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
define do_headers
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
        echo Built $(TARGET)
endef
        PROCESS_INCLUDES := 1
endif


OBJECTS := $(foreach f,$(SRC),$(basename $(f)).$(OBJEXT))

FULLOBJECTS := $(addprefix $(BUILDLOC)/,$(OBJECTS))


#Include processing
FULLINCLUDES := $(addprefix $(SRCDIR)/,$(INCLUDES))
#Object processing
FULLSRC := $(addprefix $(SRCDIR)/,$(SRC))

ifdef PROCESS_INCLUDES
GCCSRC := $(addprefix $(EPOCBLD)/,$(SRC))
FULLINCLUDES := $(foreach f,$(FULLINCLUDES),$(basename $(f)).$(INCEXT))

EPOCFULLINCLUDES := $(foreach f,$(EPOCINCLUDES),$(basename $(f)).$(INCEXT))
EPOCFULLINCLUDES := $(addprefix $(BUILDLOC)/,$(EPOCFULLINCLUDES))


#Creation of headers
$(FULLINCLUDES) : $(SRCDIR)/%.$(INCEXT) : %.inc
	$(do_headers)
$(EPOCFULLINCLUDES) : $(BUILDLOC)/%.$(INCEXT) : %.inc
	$(do_headers)

else

#Armasm sytax specifc asm rule goes here

EPOCFULLINCLUDES:= $(addprefix $(EPOCINCDIR)/,$(EPOCINCLUDES))
endif


#Strip
$(call abldquote,$(TARGET)) : $(call abldquote,$(TMPTARGET)) $(call pipe,$(TARGETDIR) $(BUILDLOC)) 
	$(do_strip)

#Link	
$(call abldquote,$(TMPTARGET)) : $(FULLOBJECTS)
	$(do_link)

#Objects
$(FULLOBJECTS) : $(BUILDLOC)/%.$(OBJEXT) : %.s $(FULLINCLUDES) $(EPOCFULLINCLUDES) $(call pipe,$(BUILDLOC))
	$(do_asm)

# make directories
$(call abldquote,$(TARGETDIR)) :
	@$(call ifnotexistd,"$(TARGETDIR)")
	
$(call abldquote,$(BUILDLOC)) :
	@$(call ifnotexistd,"$(BUILDLOC)")
	
$(EPOCBLD) :
	@$(call ifnotexistd,"$(EPOCBLD)")


MAKMAKE :
	echo Nothing to do
	echo $(BUILDLOC)

FREEZE :
	echo Nothing to do
	echo $(BUILDLOC)

LIB :
	echo Nothing to do
	echo $(BUILDLOC)

CLEANLIB :
	echo Nothing to do
	echo $(BUILDLOC)

RESOURCE :
	echo Nothing to do
	echo $(BUILDLOC)

FINAL :
	echo Nothing to do
	echo $(BUILDLOC)



BLD SAVESPACE : $(call abldquote,$(TARGETDIR)) $(EPOCBLD) $(call abldquote,$(TARGET)) $(call sbsadd,$(BUILDLOC))

RELEASABLES :
	@echo "$(TARGET)"

CLEAN :
	-$(ERASE) $(call slash2generic,"$(TARGET)") 
	-$(ERASE) $(call slash2generic,"$(EPOCBLD)/*.*") 
#	-$(ERASE) "$(TARGET)"
#	-$(ERASE) "$(EPOCBLD)/*.*"

