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

ifeq ($(OS), "Windows_NT")
RMDIR := @rmdir 2>>nul
else
RMDIR := @rmdir
endif

ifeq ($(findstring WINS,$(PLATFORM)),WINS)
	DEST_DIR = $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z/system/xmltest/wml
else
	DEST_DIR = $(EPOCROOT)epoc32/data/z/system/xmltest/wml
endif

SOURCE_DIR = $(EXTENSION_ROOT)/../test/rtest/data/Wml

# Ensure we have a clean canvas - this also avoids us having to specify
# switches to commands that are not recognised across platforms.
CLEAN_DEST :
	$(call createdir,$(DEST_DIR))

MAKE_DIRS :
	$(call createdir,$(DEST_DIR)/1.1)
	$(call createdir,$(DEST_DIR)/codepage)
	$(call createdir,$(DEST_DIR)/corrupt)
	$(call createdir,$(DEST_DIR)/unknown)

COPYFILES : CLEAN_DEST MAKE_DIRS
	$(CP) $(call slash2generic, $(SOURCE_DIR)/1.1/*.wmlc $(DEST_DIR)/1.1/*.wmlc)
	$(CP) $(call slash2generic, $(SOURCE_DIR)/codepage/*.wmlc $(DEST_DIR)/codepage/*.wmlc)
	$(CP) $(call slash2generic, $(SOURCE_DIR)/corrupt/*.wmlc $(DEST_DIR)/corrupt/*.wmlc)
	$(CP) $(call slash2generic, $(SOURCE_DIR)/unknown/*.wmlc $(DEST_DIR)/unknown/*.wmlc)	

DO_NOTHING:
	@echo do nothing

#
# The targets invoked by bld...
#

MAKMAKE : DO_NOTHING

RESOURCE : COPYFILES

CLEAN :
	$(call createdir,$(DEST_DIR))

BLD : DO_NOTHING

SAVESPACE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

RESOURCE : DO_NOTHING

FINAL : DO_NOTHING

RELEASABLES :
	@echo $(DEST_DIR)/1.1/AllElements.wmlc
	@echo $(DEST_DIR)/1.1/CharEntities.wmlc
	@echo $(DEST_DIR)/1.1/data.wmlc
	@echo $(DEST_DIR)/1.1/http___www.bbc.co.uk_mobile_sportheads1.wmlc
	@echo $(DEST_DIR)/1.1/mob.wmlc
	@echo $(DEST_DIR)/1.1/Variables.wmlc
	@echo $(DEST_DIR)/1.1/wireless_char.wmlc
	@echo $(DEST_DIR)/codepage/wml_data_attr_copepage_255.wmlc
	@echo $(DEST_DIR)/corrupt/CorruptAttrVal_mob.wmlc
	@echo $(DEST_DIR)/corrupt/CorruptAttr_mob.wmlc
	@echo $(DEST_DIR)/corrupt/CorruptTag_AllElements.wmlc
	@echo $(DEST_DIR)/corrupt/NonNullTermInlineStr.wmlc
	@echo $(DEST_DIR)/unknown/UnknownPubId_AllElements.wmlc
