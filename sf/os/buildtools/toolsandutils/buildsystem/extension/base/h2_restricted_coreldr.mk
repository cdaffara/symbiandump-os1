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
# Code execute address set in the linker file
# 
#

# To guarantee there is a slash at the end of EPOCROOT in case there is not.
# This is needed to ensure compatibility with SBSv1. 
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


## If any of these macros are changed, then execute "abld clean coreldr" from this directory
## Use this macro if it is required to use the MMU
## if the MMU is not require either comment it out or set it FALSE
USE_MMU := TRUE
  
## This macro enables benchmarking code. Comment out or set FALSE if not required
#WRITE_TIMINGS := TRUE

## This macro causes the page tables to be output. Comment out or set FALSE if not required
## If this option is selected then the MMU code will be enabled
#DUMP_PAGE_TABLES := TRUE


## Make sure all 3 macros are either TRUE or FALSE
# Enforce USE_MMU if page table is to be dumped
ifeq "$(DUMP_PAGE_TABLES)" "TRUE"
        USE_MMU := TRUE
else
        DUMP_PAGE_TABLES := FALSE
endif

ifneq "$(USE_MMU)" "TRUE"
        USE_MMU := FALSE
endif

ifneq "$(WRITE_TIMINGS)" "TRUE"
        WRITE_TIMINGS := FALSE
endif

#Set the directories
GENSRCDIR := $(EXTENSION_ROOT)/../../../kernelhwsrv/kerneltest/e32utils/nandboot/coreldr
XSRSRCDIR := $(EXTENSION_ROOT)/../../../kernelhwsrv/kerneltest/e32utils/nandboot/coreldr/unistore2
SPECSRCDIR := $(EXTENSION_ROOT)/../omaph2bsp/h2/nandboot/coreldr
PLATSRCDIR := $(EXTENSION_ROOT)/../omaph2bsp/shared/bootstrap

VARIANTINC := $(EXTENSION_ROOT)/../omaph2bsp/h2/inc
ARMDIR := $(INC_PATH)/kernel/arm

GENINC1 := $(INC_PATH) /epoc32/include
GENDRIVERINC := $(INC_PATH)/drivers
GENINC2 := $(INC_PATH)/drivers/unistore2
H2BLDDIR := $(EXTENSION_ROOT)/../omaph2bsp/h2
GENINCPATH:= $(GENSRCDIR) $(SPECSRCDIR) $(XSRSRCDIR) $(VARIANTINC) $(GENINC1) $(GENDRIVERINC) $(GENINC2) $(H2BLDDIR) $(PLATSRCDIR)

# Set the source/include/target directories
GSRCDIR = ../../../unref/orphan/cedgen/shared/nandboot
VINCDIR = ../../../unref/orphan/cedgen/h2/inc
GINCDIR = ../../../unref/orphan/cedgen/shared/inc

# Build directory (EPOCBLD too long)
#BUILDLOC = $(EPOCROOT)epoc32/build/omap/h2/specific/unistore2/nandboot/coreldr/$(PLATFORM_PATH)/$(CFG_PATH)
BUILDLOC = $(EPOCBLD)/$(PLATFORM_PATH)/$(CFG_PATH)

# Set the target name
TARGETDIR := $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)
TARGET = $(TARGETDIR)/h2_un2_coreldr.bin
TMPTARGET = $(BUILDLOC)/coreldr.tmp

#Rules
vpath %.s . $(SPECSRCDIR) $(SRCDIR) $(GENSRCDIR)
vpath %.inc . $(SPECSRCDIR)  $(VARIANTINC) $(EPOCINCDIR) $(BLDDIR) $(ARMDIR) $(PLATSRCDIR) $(GENSRCDIR) $(H2BLDDIR)
vpath %.ginc . $(BUILDLOC)

INCLUDES := nand_macro.inc

VHEADERS := nanddevice.h 
BUILTINCLUDES := nanddevice.inc
BUILTINCLUDES2 := nand_plat.inc
BLDINCLUDES := config.inc
PLATINCLUDES := 16xx_common.inc general.inc
GENINCLUDES := armcpudefs.inc

ASMSOURCE := coreldrasm.s
GENCPPSOURCE := coreldr.cpp inflate.cpp 
XSRCPPSOURCE := coreldrxsr.cpp
ifeq "$(USE_MMU)" "TRUE"
        GENASMSOURCE := coreldrmmu.s # only link in the MMU stuff if required
endif

HEADERS := inflate.h coreldr.h
SPECHEADERS := nand_plat.h

## Address at which binary is loaded and started from
LINKBASE = 0x20000C00



ARMASM_OUT := $(shell armasm 2>&1)
ARMASM_OUT_4 := $(word 4,$(ARMASM_OUT))

# Select the toolchain: ARM RVCT, then GCC 

# Use GCC toolchain if no other is available
TOOLVER := GCC
RVCTSTR := $(strip $(findstring RVCT, $(ARMASM_OUT_4)))
ifeq "$(RVCTSTR)" "RVCT"
        TOOLVER := RVCT
        OP := --
        OB := o
endif

# Build up logical TRUE defines
ifeq "$(USE_MMU)" "TRUE"
        ASM_TRUE_MACROS += USE_MMU
endif 

ifeq "$(WRITE_TIMINGS)" "TRUE"
        ASM_TRUE_MACROS += WRITE_TIMINGS
endif 

ifeq "$(DUMP_PAGE_TABLES)" "TRUE"
        ASM_TRUE_MACROS += DUMP_PAGE_TABLES
endif 

# Build up logical FALSE defines
ifeq "$(USE_MMU)" "FALSE"
        ASM_FALSE_MACROS += USE_MMU
endif 

ifeq "$(WRITE_TIMINGS)" "FALSE"
        ASM_FALSE_MACROS += WRITE_TIMINGS
endif 

ifeq "$(DUMP_PAGE_TABLES)" "FALSE"
        ASM_FALSE_MACROS += DUMP_PAGE_TABLES
endif 

#Arm RVCT tools
ifeq "$(TOOLVER)" "RVCT"
        ASM_TRUE_MACROS += USE_CXSF
        ASM := armasm
        LINK := armlink
        FROMELF := fromelf
        CPP := armcc

        OBJEXT := o
        INCEXT := inc

        ARMCCFLAGS := --arm -c -Otime --cpp
        ARMCCFLAGS := $(ARMCCFLAGS) $(foreach dir,$(GENINCPATH),$(join -I, $(dir)))
        ARMCCFLAGS := $(ARMCCFLAGS) -DEKA2
        ARMCCFLAGS := $(ARMCCFLAGS) -DSYMBIAN_SUPPORT_UNISTORE2


        ARMCCFLAGS := $(ARMCCFLAGS) --preinclude $(EPOCROOT)epoc32/include/rvct/rvct.h

        ifeq "$(CFG)" "UDEB"
        ARMCCFLAGS := $(ARMCCFLAGS) -D_DEBUG
        endif
        
        ASM_TRUE_MACRO_CMD := $(foreach macro,$(ASM_TRUE_MACROS),$(OP)predefine "$(macro) SETL {TRUE}")
        ASM_FALSE_MACRO_CMD := $(foreach macro,$(ASM_FALSE_MACROS),$(OP)predefine "$(macro) SETL {FALSE}")
        ASM_LINKBASE_MACRO := $(OP)predefine "_LINKBASE_ SETA $(LINKBASE)"

        AFLAGS := -g $(OP)keep $(ASM_TRUE_MACRO_CMD) $(ASM_FALSE_MACRO_CMD) $(ASM_LINKBASE_MACRO) -I$(BUILDLOC) $(foreach dir,$(GENINCPATH),$(join -I, $(dir)))
        LFLAGS := $(OP)entry BootEntry $(OP)ro-base $(LINKBASE) $(OP)FIRST BootEntry $(OP)map        
        SYMOPT := $(OP)symdefs
        ASMTYP := ARMASM
        LINKFILE :=

        define do_compile
                $(CPP) $(ARMCCFLAGS) $< -o $@
        endef
        define do_h2inc
                perl $(EPOCROOT)epoc32/tools/h2inc.pl $< $@ ARMASM
        endef
        define do_asm
                $(ASM) $(AFLAGS) -$(OB) $@ $(OP)LIST $(join $(basename $@),.lst) $<
        endef
        define do_link
                $(LINK) $(LFLAGS) -$(OB) $@ $(FULLOBJECTS)
        endef
        define do_strip
                $(FROMELF) $(OP)bin $(OP)output $@ $<
        endef
endif


#GCC build options
ifeq "$(TOOLVER)" "GCC"
        ASM := as
        AFLAGS := -mapcs-32 -R -n -I$(BUILDLOC)

        ASM_TRUE_MACRO_CMD := $(foreach macro,$(ASM_TRUE_MACROS),--defsym $(macro)=1)
        ASM_FALSE_MACRO_CMD := $(foreach macro,$(ASM_FALSE_MACROS),--defsym $(macro)=0)
        ASM_LINKBASE_MACRO := --defsym _LINKBASE_=$(LINKBASE)

        LINKFLAGS = -n --section-alignment 4 --file-alignment 2 -no-whole-archive
        GCCFLAGS=-march=armv4 -nostdinc -pipe -c -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas
        GCCFLAGS := $(GCCFLAGS) $(foreach dir,$(GENINCPATH),$(join -I, $(dir)))
        GCCDEFS = -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARM4__ -DEKA2 -DSYMBIAN_SUPPORT_UNISTORE2
        ifeq "$(CFG)" "UDEB"
        GCC = gcc -x c++ -g -O2 $(GCCFLAGS) -D_DEBUG -D_UNICODE $(GCCDEFS)
        else
        GCC = gcc -x c++ -s -fomit-frame-pointer -O2 $(GCCFLAGS) -DNDEBUG -D_UNICODE $(GCCDEFS)
        endif

        LINKFILE = $(SPECSRCDIR)/coreldr.lnk
        OBJEXT := o
        INCEXT := ginc

        PROCESS_INCLUDES := 1
        define do_compile
                $(GCC) -o $@ $<
        endef
        define do_h2inc
                perl $(EPOCROOT)epoc32/tools/h2inc.pl $< $@ AS
                perl $(EPOCROOT)epoc32/tools/armasm2as.pl $@ $(join $(basename $@),.ginc)
        endef 
        define do_includes
                perl $(EPOCROOT)epoc32/tools/armasm2as.pl $< $@ 
        endef
        define do_asm
                perl $(EPOCROOT)epoc32/tools/armasm2as.pl $< $(join $(basename $@),.s)
                $(AS) $(AFLAGS) $(ASM_TRUE_MACRO_CMD) $(ASM_FALSE_MACRO_CMD) $(ASM_LINKBASE_MACRO) -o $@ $(join $(basename $@),.s)
        endef
        define do_strip
                strip -O binary -o "$(TARGET)" "$(TMPTARGET)"
        
		$(ERASE) $(call slash2generic,"$(TMPTARGET)") 
        #	$(ERASE) "$(TMPTARGET)"
		echo Built $(TARGET)
        endef
        define do_link
                ld -o "$(TMPTARGET)"  --start $(FULLOBJECTS) --script=$(LINKFILE)
        endef
endif


#CPP source processing
FULLCPPSOURCE := $(addprefix $(GENSRCDIR)/,$(GENCPPSOURCE))

#Header processing
FULLHEADERS := $(addprefix $(GENSRCDIR)/,$(HEADERS))
FULLSPECHEADERS := $(addprefix $(VARIANTINC)/,$(SPECHEADERS))

FULLVHEADERS := $(addprefix $(GENDRIVERINC)/,$(VHEADERS))
FULLBUILTINCLUDES := $(addprefix $(BUILDLOC)/,$(BUILTINCLUDES))
$(FULLBUILTINCLUDES) : $(FULLVHEADERS)
	$(do_h2inc)

FULLVHEADERS2 := $(addprefix $(VARIANTINC)/,$(SPECHEADERS))
FULLBUILTINCLUDES2 := $(addprefix $(BUILDLOC)/,$(BUILTINCLUDES2))
$(FULLBUILTINCLUDES2) : $(FULLVHEADERS2) 
	$(do_h2inc)

#object names
GENCPPOBJECTS := $(foreach f,$(GENCPPSOURCE),$(basename $(f)).$(OBJEXT))
FULLGENCPPOBJECTS := $(addprefix $(BUILDLOC)/,$(GENCPPOBJECTS))

XSRCPPOBJECTS := $(foreach f,$(XSRCPPSOURCE),$(basename $(f)).$(OBJEXT))
FULLXSRCPPOBJECTS := $(addprefix $(BUILDLOC)/,$(XSRCPPOBJECTS))

ASMOBJECTS := $(foreach f,$(ASMSOURCE),$(basename $(f)).$(OBJEXT))
FULLASMOBJECTS := $(addprefix $(BUILDLOC)/,$(ASMOBJECTS))

GENASMOBJECTS := $(foreach f,$(GENASMSOURCE),$(basename $(f)).$(OBJEXT))
FULLGENASMOBJECTS := $(addprefix $(BUILDLOC)/,$(GENASMOBJECTS))

FULLOBJECTS := $(FULLASMOBJECTS) $(FULLGENASMOBJECTS) $(FULLGENCPPOBJECTS) $(FULLXSRCPPOBJECTS)

ifdef PROCESS_INCLUDES

GCCSRC := $(addprefix $(BUILDLOC)/,$(SRC))

#Creation of headers
FULLINCLUDES := $(foreach f,$(INCLUDES),$(basename $(f)).$(INCEXT))
FULLINCLUDES := $(addprefix $(BUILDLOC)/,$(FULLINCLUDES))

$(FULLINCLUDES) : $(BUILDLOC)/%.$(INCEXT) : %.inc
	$(do_includes)

FULLBLDINCLUDES := $(foreach f,$(BLDINCLUDES),$(basename $(f)).$(INCEXT))
FULLBLDINCLUDES := $(addprefix $(BUILDLOC)/,$(FULLBLDINCLUDES))
$(FULLBLDINCLUDES) : $(BUILDLOC)/%.$(INCEXT) : %.inc
	$(do_includes)

FULLPLATINCLUDES := $(foreach f,$(PLATINCLUDES),$(basename $(f)).$(INCEXT))
FULLPLATINCLUDES := $(addprefix $(BUILDLOC)/,$(FULLPLATINCLUDES))
$(FULLPLATINCLUDES) : $(BUILDLOC)/%.$(INCEXT) : %.inc
	$(do_includes)

FULLGENINCLUDES := $(foreach f,$(GENINCLUDES),$(basename $(f)).$(INCEXT))
FULLGENINCLUDES := $(addprefix $(BUILDLOC)/,$(FULLGENINCLUDES))
$(FULLGENINCLUDES) : $(BUILDLOC)/%.$(INCEXT) : %.inc
	$(do_includes)

else
FULLINCLUDES:= $(addprefix $(SPECSRCDIR)/,$(INCLUDES))
FULLPLATINCLUDES:= $(addprefix $(PLATSRCDIR)/,$(PLATINCLUDES))
FULLGENINCLUDES:= $(addprefix $(GENSRCDIR)/,$(GENINCLUDES))
FULLBLDINCLUDES:= $(addprefix $(H2BLDDIR)/,$(BLDINCLUDES))

#Arm RVCT specifics here

endif


#Link 
$(TMPTARGET) : $(FULLOBJECTS)
	$(do_link)

#strip
$(TARGET) : $(TMPTARGET)
	$(do_strip)

#CPP objects
$(FULLGENCPPOBJECTS) : $(BUILDLOC)/%.$(OBJEXT) : $(GENSRCDIR)/%.cpp $(FULLHEADERS) $(FULLSPECHEADERS)
	$(do_compile)

$(FULLXSRCPPOBJECTS) : $(BUILDLOC)/%.$(OBJEXT) : $(XSRSRCDIR)/%.cpp $(FULLHEADERS) $(FULLSPECHEADERS)
	$(do_compile)


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
#	-$(ERASE) "$(TARGET)"
#	-$(ERASE) "$(BUILDLOC)/*.*"

