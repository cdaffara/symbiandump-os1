# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# ne1_tb_restricted_coreldr.mk
# #  NB! LINKBASE :  Code execute address also set coreldr.lnk file 
# #  MUST REFLECT CORELOADER ADDRESS RELOCATION IN BOTH FILES!!
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
  
# This macro enables benchmarking code. Comment out or set FALSE if not required
#WRITE_TIMINGS := TRUE
# Timer is not implemented for Naviengine. Do not enable!


## This macro causes the page tables to be output. Comment out or set FALSE if not required
## If this option is selected then the MMU code will be enabled
# DUMP_PAGE_TABLES := TRUE
# Page tables cannot be dumped on Naviengine due to the NANDController used. Do not enable!

# This macro enables the MMU enabled version of the coreldr to call RestartAuxiliaryCores
# before it runs the core image.  This function should be defined in the variant, to 
# restart the auxiliary cores, which it should have paused on startup.
RUNS_WITH_SMP := TRUE

# Macro used to enable support for Shadowed Memory Regions feature in loader
# See also varaint\config.inc to enable in bootstrap
SUPPORTS_SMR := TRUE

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

ifneq "$(SUPPORTS_SMR)" "TRUE"
	SUPPORTS_SMR := FALSE
endif

#Set the directories; based on calling bld.inf
GENSRCDIR := $(EXTENSION_ROOT)/../../../kernelhwsrv/kerneltest/e32utils/nandboot/coreldr
#Generic drivers
BLSRCDIR := $(EXTENSION_ROOT)/../../../kernelhwsrv/kernel/eka/drivers/unistore2/srca/XSR/UTIL/ONBL2
XSRSRCDIR1 := $(EXTENSION_ROOT)/../../../kernelhwsrv/kernel/eka/drivers/unistore2/srca/XSR/OAM/OSLess
XSRSRCDIR2 := $(EXTENSION_ROOT)/../../../kernelhwsrv/kernel/eka/drivers/unistore2/srca/XSR/LLD/DNandO
#Coreloader ASM...
SPECSRCDIR := $(EXTENSION_ROOT)/../navienginebsp/ne1_tb/nandboot
#PAM - Platform specific Version
SPECXSRSRCDIR := $(EXTENSION_ROOT)/pam
SPECXSRSRCDIR1 := $(EXTENSION_ROOT)/lld

#Platform Variant includes
VARIANTINC := $(INC_PATH)/ne1_tb
VARIANTINC2 := $(EXTENSION_ROOT)/../navienginebsp/ne1_tb/bootstrap
VARIANTINC3 := $(EXTENSION_ROOT)/../navienginebsp/ne1_tb/nand  $(EXTENSION_ROOT)/../navienginebsp/naviengine_assp  $(INC_PATH)/assp/naviengine/$(MEMMODEL)

#Generic driver file includes
GENINC1 := $(INC_PATH) $(EPOCROOT)epoc32/include
GENDRIVERINC := $(INC_PATH)/drivers
GENINC2 := $(INC_PATH)/drivers/unistore2 $(INC_PATH)/memmodel/epoc $(INC_PATH)/kernel  $(INC_PATH)/kernel/arm $(INC_PATH)/nkern $(INC_PATH)/nkern/arm
GENXSRINC := $(EXTENSION_ROOT)/../../../kernelhwsrv/kernel/eka/drivers/unistore2/srca/XSR/INC
GENINCPATH:= $(GENSRCDIR) $(SPECSRCDIR) $(XSRSRCDIR1) $(XSRSRCDIR2) $(SPECXSRSRCDIR) $(SPECXSRSRCDIR1) $(VARIANTINC) $(VARIANTINC2) $(VARIANTINC3) $(GENINC1) $(GENDRIVERINC) $(GENINC2) $(GENINC3) $(GENINC4) $(GENXSRINC)


# Set the source/include/target directories

#epoc32 folder for exported header files
EPOCINCDIR = $(INC_PATH)/ne1_tb

# Build directory (EPOCBLD too long)
BUILDLOC = $(EPOCROOT)epoc32/build/bsp/hwip_nec_naviengine/ne1_tb_restricted/unistore2/nandboot/coreldr/$(PLATFORM_PATH)

# Set the target name
TARGETDIR := $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)
TARGET = $(TARGETDIR)$/$(NAME).bin
TMPTARGET = $(BUILDLOC)$/$(NAME).elf

#Rules
vpath %.s . $(SPECSRCDIR) $(SRCDIR)
vpath %.inc . $(SPECSRCDIR) $(EPOCINCDIR)
vpath %.ginc . $(BUILDLOC)


# from base/e32/include/drivers/...
VHEADERS := nanddevice.h
# the following .inc files get built as part of the process
BUILTINCLUDES := nanddevice.inc config.inc
BUILTINCLUDES2 := nand_plat.inc



ASMSOURCE := coreldrasm.s
GENCPPSOURCE := inflate.cpp
BLCPPSOURCE := ONbl2.cpp
XSRCPPSOURCE1 := OSLessOAM.cpp
SPECXSRCPPSOURCE := pam.cpp
SPECXSRCPPSOURCE1 := pnl.cpp  

XSRLIB := $(EPOCROOT)epoc32/release/armv5/$(CFG)/nbl2.lib

ifeq "$(USE_MMU)" "TRUE"
	#generic version found in base/e32utils/nandboot/coreldr/...
	GENASMSOURCE := coreldrmmu.s # only link in the MMU stuff if required
endif

HEADERS := 
SPECHEADERS := nand_plat.h

## Address at which coreloader binary is loaded and then started from
#
# On NaviEngine this number is base of ram + 208MB (permitting 208MB core images)
# this number is pretty arbitrary
# if necessary as long as the corresponding change is also made to
# KCoreLoaderAddress in variant_bootstrap.inc
#
LINKBASE = 0x8D000000

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

ifeq "$(SUPPORTS_SMR)" "TRUE"
	ASM_TRUE_MACROS += SUPPORTS_SMR
endif 

ifeq "$(DUMP_PAGE_TABLES)" "TRUE"
	ASM_TRUE_MACROS += DUMP_PAGE_TABLES
endif 

ifeq "$(RUNS_WITH_SMP)" "TRUE"
	ASM_TRUE_MACROS += RUNS_WITH_SMP
endif

# Build up logical FALSE defines
ifeq "$(USE_MMU)" "FALSE"
	ASM_FALSE_MACROS += USE_MMU
endif 

ifeq "$(WRITE_TIMINGS)" "FALSE"
	ASM_FALSE_MACROS += WRITE_TIMINGS
endif
 
ifeq "$(SUPPORTS_SMR)" "FALSE"
	ASM_FALSE_MACROS += SUPPORTS_SMR
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
ARMCCFLAGS := $(ARMCCFLAGS) -DXSR_NBL2 -DREAL_TARGET -DDEFERED_CHK -DSYMBIAN_SUPPORT_UNISTORE2
  

ifeq "$(SUPPORTS_SMR)" "TRUE"
    ARMCCFLAGS := $(ARMCCFLAGS) -DSUPPORTS_SMR
endif    



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

SPECXSRCPPOBJECTS := $(foreach f,$(SPECXSRCPPSOURCE),$(basename $(f)).$(OBJEXT))
FULLSPECXSRCPPOBJECTS := $(addprefix $(BUILDLOC)/,$(SPECXSRCPPOBJECTS))

SPECXSRCPPOBJECTS1 := $(foreach f,$(SPECXSRCPPSOURCE1),$(basename $(f)).$(OBJEXT))
FULLSPECXSRCPPOBJECTS1 := $(addprefix $(BUILDLOC)/,$(SPECXSRCPPOBJECTS1))

ASMOBJECTS := $(foreach f,$(ASMSOURCE),$(basename $(f)).$(OBJEXT))
FULLASMOBJECTS := $(addprefix $(BUILDLOC)/,$(ASMOBJECTS))

GENASMOBJECTS := $(foreach f,$(GENASMSOURCE),$(basename $(f)).$(OBJEXT))
FULLGENASMOBJECTS := $(addprefix $(BUILDLOC)/,$(GENASMOBJECTS))

FULLOBJECTS := $(FULLASMOBJECTS) $(FULLGENASMOBJECTS) $(FULLGENCPPOBJECTS) $(FULLBLCPPOBJECTS) $(FULLXSRCPPOBJECTS1) $(FULLSPECXSRCPPOBJECTS) $(FULLSPECXSRCPPOBJECTS1) $(XSRLIB)

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

$(FULLSPECXSRCPPOBJECTS) : $(BUILDLOC)/%.$(OBJEXT) : $(SPECXSRSRCDIR)/%.cpp $(FULLHEADERS) $(FULLSPECHEADERS)
	$(do_compile)

$(FULLSPECXSRCPPOBJECTS1) : $(BUILDLOC)/%.$(OBJEXT) : $(SPECXSRSRCDIR1)/%.cpp $(FULLHEADERS) $(FULLSPECHEADERS)
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

