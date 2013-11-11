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
# This makefile template is used to generate header/iby/features.dat file
# 
#

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


ifeq "$(CFG_PATH)" "rel"

FEAT_TOOL := perl -S $(call slash2generic,features.pl)

ifndef FEAT_DATABASE
FEAT_DATABASE := $(EPOCROOT)epoc32/rom/include/featuredatabase.xml
endif

ifndef FEAT_HEADER_PATH
FEAT_HEADER_PATH := $(EPOCROOT)epoc32/include
endif

ifndef FEAT_IBY_PATH
FEAT_IBY_PATH := $(EPOCROOT)epoc32/rom/include
endif

ifndef FEAT_DAT_PATH
FEAT_DAT_PATH := $(EPOCROOT)epoc32/data/config
endif

# Features tool will be invoked here
ALL:
	$(FEAT_TOOL) --datfile=$(FEAT_DAT_PATH) --hdrfile=$(FEAT_HEADER_PATH) --ibyfile=$(FEAT_IBY_PATH) $(FEAT_DATABASE)


BLD SAVESPACE: ALL

CLEAN :
	-$(ERASE) $(call slash2generic,$(FEAT_HEADER_PATH)/featureuids.h)
	-$(ERASE) $(call slash2generic,$(FEAT_IBY_PATH)/feature.iby)
	-$(ERASE) $(call slash2generic,$(FEAT_DAT_PATH)/features.dat)

RELEASABLES :
	@echo $(FEAT_HEADER_PATH)/featureuids.h
	@echo $(FEAT_IBY_PATH)/feature.iby
	@echo $(FEAT_DAT_PATH)/features.dat
	
DO_NOTHING :
	@echo do nothing
	
MAKMAKE : DO_NOTHING
FREEZE : DO_NOTHING
LIB : DO_NOTHING
CLEANLIB : DO_NOTHING
RESOURCE : DO_NOTHING
FINAL : DO_NOTHING

#if $(CFG_PATH) == "deb"
else

FINAL FREEZE LIB CLEANLIB RESOURCE RELEASABLES CLEAN BLD SAVESPACE MAKMAKE :

endif

