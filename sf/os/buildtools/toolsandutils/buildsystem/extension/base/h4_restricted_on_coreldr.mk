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
# #  NB! LINKBASE :  Code execute address also set coreldr.lnk file 
# #  MUST REFLECT CORELOADER ADDRESS RELOCATION IN BOTH FILES!!
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


## If any of these macros are changed, then execute "abld clean coreldr" from this directory
## Use this macro if it is required to use the MMU
## if the MMU is not require either comment it out or set it FALSE
USE_MMU := TRUE
  
# This macro enables benchmarking code. Comment out or set FALSE if not required
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
BLSRCDIR := $(EXTENSION_ROOT)/../../../kernelhwsrv/kernel/eka/drivers/unistore2/srca/XSR/UTIL/ONBL2
XSRSRCDIR1 := $(EXTENSION_ROOT)/../../../kernelhwsrv/kernel/eka/drivers/unistore2/srca/XSR/OAM/OSLess
XSRSRCDIR2 := $(EXTENSION_ROOT)/../../../kernelhwsrv/kernel/eka/drivers/unistore2/srca/XSR/LLD/DNandO
SPECSRCDIR := $(EXTENSION_ROOT)/../omaph4bsp/h4/nandboot/coreldr_onenand
SPECXSRSRCDIR := $(EXTENSION_ROOT)/drivers/h4ons

VARIANTINC := $(INC_PATH)/omap_hrp/h4
VARIANTINC2 := $(EXTENSION_ROOT)/../omaph4bsp/shared/bootstrap
VARIANTINC3 := $(EXTENSION_ROOT)/../omaph4bsp/h4/nand  $(EXTENSION_ROOT)/../omaph4bsp/assp/shared/assp $(EXTENSION_ROOT)/../omaph4bsp/assp/omap24xx/inc $(EXTENSION_ROOT)/../omaph4bsp/assp/omap24xx/assp $(EXTENSION_ROOT)/../omaph4bsp/assp/shared/interrupt $(EXTENSION_ROOT)/../omaph4bsp/assp/omap24xx/interrupt $(EXTENSION_ROOT)/../omaph4bsp/shared/gpio $(EXTENSION_ROOT)/../omaph4bsp/shared/mcspi $(EXTENSION_ROOT)/../omaph4bsp/assp/omap24xx/gpio $(EXTENSION_ROOT)/../omaph4bsp/assp/shared $(EXTENSION_ROOT)/../omaph4bsp/shared/cirq $(EXTENSION_ROOT)/../omaph4bsp/shared/dma $(EXTENSION_ROOT)/../omaph4bsp/assp/shared/dma

GENINC1 := $(INC_PATH)
GENINC3 := $(EPOCROOT)epoc32/include
GENDRIVERINC := $(INC_PATH)/drivers
GENINC2 := $(INC_PATH)/drivers/unistore2 $(INC_PATH)/memmodel/epoc $(INC_PATH)/kernel  $(INC_PATH)/kernel/arm $(INC_PATH)/nkern $(INC_PATH)/nkern/arm
GENXSRINC := $(EXTENSION_ROOT)/../../../kernelhwsrv/kernel/eka/drivers/unistore2/srca/XSR/INC
GENINCPATH:= $(GENSRCDIR) $(SPECSRCDIR) $(XSRSRCDIR1) $(XSRSRCDIR2) $(SPECXSRSRCDIR) $(VARIANTINC) $(VARIANTINC2) $(VARIANTINC3) $(GENINC1) $(GENDRIVERINC) $(GENINC2) $(GENINC3) $(GENINC4) $(GENXSRINC) $(GENINC3)

# Set the source/include/target directories
GSRCDIR = ../../../unref/orphan/cedgen/shared/nandboot
VINCDIR = ../../../unref/orphan/cedgen/h4/inc
GINCDIR = ../../../unref/orphan/cedgen/shared/inc
EPOCINCDIR = $(INC_PATH)/omap_hrp/h4

# Build directory (EPOCBLD too long)
BUILDLOC = $(EPOCROOT)epoc32/build/omap_hrp/h4_restricted/unistore2/nandboot/coreldr/$(PLATFORM_PATH)

# Set the target name
TARGETDIR := $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)
TARGET = $(TARGETDIR)$/h4hrp_un2_on_coreldr.bin
TMPTARGET = $(BUILDLOC)$/h4hrp_un2_on_coreldr.elf

#Rules
vpath %.s . $(SPECSRCDIR) $(SRCDIR)
vpath %.inc . $(SPECSRCDIR) $(EPOCINCDIR)
vpath %.ginc . $(BUILDLOC)

VHEADERS := NandDevice.h
BUILTINCLUDES := NandDevice.inc config.inc
BUILTINCLUDES2 := nand_plat.inc

ASMSOURCE := coreldrasm_onenand.s
GENCPPSOURCE := inflate.cpp
BLCPPSOURCE := ONbl2.cpp
XSRCPPSOURCE1 := OSLessOAM.cpp
XSRCPPSOURCE2 := onld.cpp
SPECXSRCPPSOURCE := pam.cpp

XSRLIB := $(EPOCROOT)epoc32/release/armv5/$(CFG)/nbl2.lib

ifeq "$(USE_MMU)" "TRUE"
	GENASMSOURCE := coreldrmmu.s # only link in the MMU stuff if required
endif

# HEADERS := inflate.h coreldr.h
HEADERS := 
SPECHEADERS := nand_plat.h

## Address at which coreloader binary is loaded and then started from
#
# On H4 this number is base of ram + 48MB (permitting 48MB core images)
# this number is pretty arbitrary and may be raised higher into ram
# if necessary as long as the corresponding change is also made to
# KCoreLoaderAddress in variant_bootstrap.inc
#
LINKBASE = 0x83000000



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
ARMCCFLAGS := $(ARMCCFLAGS) -DXSR_NBL2 -DREAL_TARGET -DSYMBIAN_SUPPORT_UNISTORE2 

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
perl -S $(EPOCROOT)epoc32/tools/h2inc.pl $< $@ ARMASM
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

BLCPPOBJECTS := $(foreach f,$(BLCPPSOURCE),$(basename $(f)).$(OBJEXT))
FULLBLCPPOBJECTS := $(addprefix $(BUILDLOC)/,$(BLCPPOBJECTS))

XSRCPPOBJECTS1 := $(foreach f,$(XSRCPPSOURCE1),$(basename $(f)).$(OBJEXT))
FULLXSRCPPOBJECTS1 := $(addprefix $(BUILDLOC)/,$(XSRCPPOBJECTS1))

XSRCPPOBJECTS2 := $(foreach f,$(XSRCPPSOURCE2),$(basename $(f)).$(OBJEXT))
FULLXSRCPPOBJECTS2 := $(addprefix $(BUILDLOC)/,$(XSRCPPOBJECTS2))

SPECXSRCPPOBJECTS := $(foreach f,$(SPECXSRCPPSOURCE),$(basename $(f)).$(OBJEXT))
FULLSPECXSRCPPOBJECTS := $(addprefix $(BUILDLOC)/,$(SPECXSRCPPOBJECTS))

ASMOBJECTS := $(foreach f,$(ASMSOURCE),$(basename $(f)).$(OBJEXT))
FULLASMOBJECTS := $(addprefix $(BUILDLOC)/,$(ASMOBJECTS))

GENASMOBJECTS := $(foreach f,$(GENASMSOURCE),$(basename $(f)).$(OBJEXT))
FULLGENASMOBJECTS := $(addprefix $(BUILDLOC)/,$(GENASMOBJECTS))

FULLOBJECTS := $(FULLASMOBJECTS) $(FULLGENASMOBJECTS) $(FULLGENCPPOBJECTS) $(FULLBLCPPOBJECTS) $(FULLXSRCPPOBJECTS1) $(FULLXSRCPPOBJECTS2) $(FULLSPECXSRCPPOBJECTS) $(XSRLIB)

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

$(FULLBLCPPOBJECTS) : $(BUILDLOC)/%.$(OBJEXT) : $(BLSRCDIR)/%.cpp $(FULLHEADERS) $(FULLSPECHEADERS)
	$(do_compile)

$(FULLXSRCPPOBJECTS1) : $(BUILDLOC)/%.$(OBJEXT) : $(XSRSRCDIR1)/%.cpp $(FULLHEADERS) $(FULLSPECHEADERS)
	$(do_compile)

$(FULLXSRCPPOBJECTS2) : $(BUILDLOC)/%.$(OBJEXT) : $(XSRSRCDIR2)/%.cpp $(FULLHEADERS) $(FULLSPECHEADERS)
	$(do_compile)

$(FULLSPECXSRCPPOBJECTS) : $(BUILDLOC)/%.$(OBJEXT) : $(SPECXSRSRCDIR)/%.cpp $(FULLHEADERS) $(FULLSPECHEADERS)
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
	@echo BLD

RELEASABLES :
	@echo "$(TARGET)"

CLEAN :
	-$(ERASE) $(call slash2generic,"$(TARGET)")
	-$(ERASE) $(call slash2generic,"$(BUILDLOC)/*.*")

