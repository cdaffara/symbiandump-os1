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
# fm_copyfiles_to_winscw_zdrive.mk
# Arguments to makefile are:
# option FILE_TO_COPY <src/dest_filename_without_any_path>
# option SRC_PATH     <epocroot_starting_path>
# option DEST_PATH    <emulator_zdrive_path_starting_with_a_z>
# Job of the this makefile is to copy the specified file to 
# the epoc32/release/winscw/CFG/z/... folder for the current config e.g. UREL or
# UDEB.
# 
#


# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/
 
include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


SOURCE_FILE=$(SRC_PATH)/$(FILE_TO_COPY)
TARGET_DIR=$(EPOCROOT)epoc32/release/winscw/$(CFG_PATH)/$(DEST_PATH)
TARGET_FILE=$(TARGET_DIR)/$(FILE_TO_COPY)

DO_NOTHING : 
	@echo do nothing

$(TARGET_DIR) : 
	$(MKDIR) $(call slash2generic,$(TARGET_DIR))
	
#
# The targets invoked by abld...
#

MAKMAKE : DO_NOTHING

BLD : DO_NOTHING

SAVESPACE : DO_NOTHING

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

RESOURCE : DO_NOTHING

CLEAN : 
	@echo Erasing $(call slash2generic,$(TARGET_FILE))
	-$(ERASE) $(call slash2generic,$(TARGET_FILE))

RELEASABLES : 
	@echo $(TARGET_FILE)

FINAL : $(TARGET_DIR)
	@echo Copying $(call slash2generic,$(SOURCE_FILE)) to $(call slash2generic,$(TARGET_FILE))
	$(CP) $(call slash2generic,$(SOURCE_FILE)) $(call slash2generic,$(TARGET_FILE))

