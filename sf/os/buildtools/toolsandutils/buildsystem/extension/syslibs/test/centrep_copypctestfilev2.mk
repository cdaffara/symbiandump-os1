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
# centrep_copypctestfile.mk
# 
#

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	EPOCDATADIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)
else
	EPOCDATADIR = $(EPOCROOT)epoc32/data
endif

SECURETARGETDIR = $(EPOCDATADIR)/z/private/10202BE9

PCCENREPSOURCE = $(EXTENSION_ROOT)/../test
PCCENREPDATADIR = $(EPOCROOT)epoc32/winscw/c/private/00000000
PCCENREPTESTDIR = $(EPOCROOT)epoc32/winscw/c
PCCENREPPCTESTDIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)

$(SECURETARGETDIR) :
	$(call createdir, "$@")

$(PCCENREPDATADIR) :
	$(call createdir, "$@")

COPYFILES :
ifneq ($(findstring TOOLS2,$(PLATFORM)),TOOLS2)
	$(call forcecopy,$(PCCENREPSOURCE)/000001ff.txt,$(SECURETARGETDIR)/000001ff.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/00001fff.crev2,$(SECURETARGETDIR)/00001fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/000002ff.crev2,$(SECURETARGETDIR)/000002ff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/88888880.txt,$(SECURETARGETDIR)/88888880.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/88888881.cre,$(SECURETARGETDIR)/88888881.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00004fff.cre,$(SECURETARGETDIR)/00004fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00022222.txt,$(SECURETARGETDIR)/00022222.txt)

ifeq ($(findstring WINS,$(PLATFORM)),WINS)	
	$(call forcecopy,$(PCCENREPSOURCE)/000001ff.txt,$(PCCENREPDATADIR)/000001ff.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/00001fff.crev2,$(PCCENREPDATADIR)/00001fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/000002ff.crev2,$(PCCENREPDATADIR)/000002ff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/88888880.txt,$(PCCENREPDATADIR)/88888880.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/88888881.cre,$(PCCENREPDATADIR)/88888881.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/winscwcre.crev2,$(PCCENREPDATADIR)/ref_winscwcre.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/winscwtxt.crev2,$(PCCENREPDATADIR)/ref_winscwtxt.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/000001ff.txt,$(PCCENREPDATADIR)/copy000001ff.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/00001fff.crev2,$(PCCENREPDATADIR)/copy00001fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/000002ff.crev2,$(PCCENREPDATADIR)/copy000002ff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00004fff.cre,$(PCCENREPDATADIR)/00004fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00004fff.cre,$(PCCENREPDATADIR)/copy00004fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00022222.txt,$(PCCENREPDATADIR)/00022222.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/00022222.crev2,$(PCCENREPDATADIR)/copy00022222.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/000001ff.txt,$(PCCENREPTESTDIR)/000001ff.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/00001fff.crev2,$(PCCENREPTESTDIR)/00001fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00001fff.crev2,$(PCCENREPTESTDIR)/copy00001fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00004fff.cre,$(PCCENREPTESTDIR)/00004fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00004fff.cre,$(PCCENREPTESTDIR)/copy00004fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00022222.txt,$(PCCENREPTESTDIR)/00022222.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/00022222.crev2,$(PCCENREPTESTDIR)/copy00022222.cre)
# Shared files between the t_cenreppc (WINSCW) and testsymcenrep (TOOLS2) tests.
	$(call forcecopy,$(PCCENREPSOURCE)/common_crc.txt,$(PCCENREPTESTDIR)/common_crc.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/common_ref_00022222.cre,$(PCCENREPDATADIR)/common_ref_00022222.cre)
endif

# TOOLS2
else
	$(call forcecopy,$(PCCENREPSOURCE)/000001ff.txt,$(PCCENREPPCTESTDIR)/000001ff.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/00001fff.crev2,$(PCCENREPPCTESTDIR)/00001fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00004fff.cre,$(PCCENREPPCTESTDIR)/00004fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/000001ff.txt,$(PCCENREPPCTESTDIR)/copy000001ff.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/00001fff.crev2,$(PCCENREPPCTESTDIR)/copy00001fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00004fff.cre,$(PCCENREPPCTESTDIR)/copy00004fff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/000002ff.crev2,$(PCCENREPPCTESTDIR)/000002ff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/000002ff.crev2,$(PCCENREPPCTESTDIR)/copy000002ff.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/winscwcre.crev2,$(PCCENREPPCTESTDIR)/ref_winscwcre.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/winscwtxt.crev2,$(PCCENREPPCTESTDIR)/ref_winscwtxt.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/88888880.txt,$(PCCENREPPCTESTDIR)/88888880.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/88888881.cre,$(PCCENREPPCTESTDIR)/88888881.cre)
	$(call forcecopy,$(PCCENREPSOURCE)/00022222.txt,$(PCCENREPPCTESTDIR)/00022222.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/00022222.crev2,$(PCCENREPPCTESTDIR)/copy00022222.cre)
# Shared files between the t_cenreppc (WINSCW) and testsymcenrep (TOOLS2) tests.
	$(call forcecopy,$(PCCENREPSOURCE)/common_crc.txt,$(PCCENREPTESTDIR)/common_crc.txt)
	$(call forcecopy,$(PCCENREPSOURCE)/common_ref_00022222.cre,$(PCCENREPDATADIR)/common_ref_00022222.cre)
endif

DO_NOTHING:
	@echo do nothing

#
# The targets invoked by bld...
#

BLD : $(SECURETARGETDIR) $(PCCENREPDATADIR) $(PCCENREPTESTDIR) $(PCCENREPSOURCE) $(PCCENREPPCTESTDIR) COPYFILES

CLEAN :

ifneq ($(findstring TOOLS2,$(PLATFORM)),TOOLS2)
	$(call forceremove,$(SECURETARGETDIR)/000001ff.txt)
	$(call forceremove,$(SECURETARGETDIR)/00001fff.cre)
	$(call forceremove,$(SECURETARGETDIR)/000002ff.cre)
	$(call forceremove,$(SECURETARGETDIR)/88888880.txt)
	$(call forceremove,$(SECURETARGETDIR)/88888881.cre)
	$(call forceremove,$(SECURETARGETDIR)/00004fff.cre)
	$(call forceremove,$(SECURETARGETDIR)/00022222.txt)

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	$(call forceremove,$(PCCENREPDATADIR)/000001ff.txt)
	$(call forceremove,$(PCCENREPDATADIR)/00001fff.cre)
	$(call forceremove,$(PCCENREPDATADIR)/000002ff.cre)
	$(call forceremove,$(PCCENREPDATADIR)/88888880.txt)
	$(call forceremove,$(PCCENREPDATADIR)/88888881.cre)
	$(call forceremove,$(PCCENREPDATADIR)/ref_winscwcre.cre)
	$(call forceremove,$(PCCENREPDATADIR)/ref_winscwtxt.cre)
	$(call forceremove,$(PCCENREPDATADIR)/copy000001ff.txt)
	$(call forceremove,$(PCCENREPDATADIR)/copy00001fff.cre)
	$(call forceremove,$(PCCENREPDATADIR)/copy000002ff.cre)
	$(call forceremove,$(PCCENREPDATADIR)/00004fff.cre)
	$(call forceremove,$(PCCENREPDATADIR)/copy00004fff.cre)
	$(call forceremove,$(PCCENREPDATADIR)/00022222.txt)
	$(call forceremove,$(PCCENREPDATADIR)/copy00022222.cre)
	$(call forceremove,$(PCCENREPTESTDIR)/000001ff.txt)
	$(call forceremove,$(PCCENREPTESTDIR)/00001fff.cre)
	$(call forceremove,$(PCCENREPTESTDIR)/copy00001fff.cre)
	$(call forceremove,$(PCCENREPTESTDIR)/00004fff.cre)
	$(call forceremove,$(PCCENREPTESTDIR)/copy00004fff.cre)
	$(call forceremove,$(PCCENREPTESTDIR)/00022222.txt)
	$(call forceremove,$(PCCENREPTESTDIR)/copy00022222.cre)
	$(call forceremove,$(PCCENREPTESTDIR)/common_crc.txt)
	$(call forceremove,$(PCCENREPDATADIR)/common_ref_00022222.cre)
endif


# TOOLS2
else
	$(call forceremove,$(PCCENREPPCTESTDIR)/000001ff.txt)
	$(call forceremove,$(PCCENREPPCTESTDIR)/00001fff.cre)
	$(call forceremove,$(PCCENREPPCTESTDIR)/00004fff.cre)
	$(call forceremove,$(PCCENREPPCTESTDIR)/copy000001ff.txt)
	$(call forceremove,$(PCCENREPPCTESTDIR)/copy00001fff.cre)
	$(call forceremove,$(PCCENREPPCTESTDIR)/000002ff.cre)
	$(call forceremove,$(PCCENREPPCTESTDIR)/copy000002ff.cre)
	$(call forceremove,$(PCCENREPPCTESTDIR)/ref_winscwcre.cre)
	$(call forceremove,$(PCCENREPPCTESTDIR)/ref_winscwtxt.cre)
	$(call forceremove,$(PCCENREPPCTESTDIR)/88888880.txt)
	$(call forceremove,$(PCCENREPPCTESTDIR)/88888881.cre)
	$(call forceremove,$(PCCENREPPCTESTDIR)/00022222.txt)
	$(call forceremove,$(PCCENREPPCTESTDIR)/copy00022222.cre)
	$(call forceremove,$(PCCENREPPCTESTDIR)/copy00004fff.cre)
	$(call forceremove,$(PCCENREPTESTDIR)/common_crc.txt)
	$(call forceremove,$(PCCENREPDATADIR)/common_ref_00022222.cre)

endif

RELEASABLES :
ifneq ($(findstring TOOLS2,$(PLATFORM)),TOOLS2)
	@echo $(SECURETARGETDIR)/000001ff.txt
	@echo $(SECURETARGETDIR)/00001fff.cre
	@echo $(SECURETARGETDIR)/000002ff.cre
	@echo $(SECURETARGETDIR)/88888880.txt
	@echo $(SECURETARGETDIR)/88888881.cre
	@echo $(SECURETARGETDIR)/00004fff.cre
	@echo $(SECURETARGETDIR)/00022222.txt

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	@echo $(PCCENREPDATADIR)/000001ff.txt
	@echo $(PCCENREPDATADIR)/00001fff.cre
	@echo $(PCCENREPDATADIR)/000002ff.cre
	@echo $(PCCENREPDATADIR)/88888880.txt
	@echo $(PCCENREPDATADIR)/88888881.cre
	@echo $(PCCENREPDATADIR)/ref_winscwcre.cre
	@echo $(PCCENREPDATADIR)/ref_winscwtxt.cre
	@echo $(PCCENREPDATADIR)/copy000001ff.txt
	@echo $(PCCENREPDATADIR)/copy00001fff.cre
	@echo $(PCCENREPDATADIR)/copy000002ff.cre
	@echo $(PCCENREPDATADIR)/00004fff.cre
	@echo $(PCCENREPDATADIR)/copy00004fff.cre
	@echo $(PCCENREPDATADIR)/00022222.txt
	@echo $(PCCENREPDATADIR)/copy00022222.cre
	@echo $(PCCENREPTESTDIR)/000001ff.txt
	@echo $(PCCENREPTESTDIR)/00001fff.cre
	@echo $(PCCENREPTESTDIR)/copy00001fff.cre
	@echo $(PCCENREPTESTDIR)/00004fff.cre
	@echo $(PCCENREPTESTDIR)/copy00004fff.cre
	@echo $(PCCENREPTESTDIR)/00022222.txt
	@echo $(PCCENREPTESTDIR)/copy00022222.cre
	@echo $(PCCENREPTESTDIR)/common_crc.txt
	@echo $(PCCENREPDATADIR)/common_ref_00022222.cre
endif

# TOOLS2
else 
	@echo $(PCCENREPPCTESTDIR)/000001ff.txt
	@echo $(PCCENREPPCTESTDIR)/00001fff.cre
	@echo $(PCCENREPPCTESTDIR)/copy000001ff.txt
	@echo $(PCCENREPPCTESTDIR)/copy00001fff.cre
	@echo $(PCCENREPPCTESTDIR)/000002ff.cre
	@echo $(PCCENREPPCTESTDIR)/copy000002ff.cre
	@echo $(PCCENREPPCTESTDIR)/ref_winscwcre.cre
	@echo $(PCCENREPPCTESTDIR)/ref_winscwtxt.cre
	@echo $(PCCENREPPCTESTDIR)/88888880.txt
	@echo $(PCCENREPPCTESTDIR)/88888881.cre
	@echo $(PCCENREPPCTESTDIR)/00022222.txt
	@echo $(PCCENREPPCTESTDIR)/copy00022222.cre
	@echo $(PCCENREPPCTESTDIR)/copy00004fff.cre
	@echo $(PCCENREPTESTDIR)/common_crc.txt
	@echo $(PCCENREPDATADIR)/common_ref_00022222.cre

endif

MAKMAKE : DO_NOTHING

RESOURCE : DO_NOTHING

SAVESPACE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

FINAL : DO_NOTHING

ROMFILE : DO_NOTHING
