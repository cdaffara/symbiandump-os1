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

BUILD_DIR = $(EPOCROOT)epoc32/build/generated/xml

GENERATED_FILES = $(BUILD_DIR)/t_stringdictionary00tagtable.cpp

$(BUILD_DIR)/t_stringdictionary00tagtable.cpp : $(EXTENSION_ROOT)/../test/rtest/tsrc/t_stringdictionary00tagtable.st
	$(call createdir,$(BUILD_DIR))
	@perl $(EPOCROOT)epoc32/tools/ecopyfile.pl $(EXTENSION_ROOT)/../test/rtest/tsrc/t_stringdictionary00tagtable.st $(BUILD_DIR)$/t_stringdictionary00tagtable.st
	@perl $(EPOCROOT)epoc32/tools/stringtable.pl $(BUILD_DIR)/t_stringdictionary00tagtable.st 

DO_NOTHING:
	@echo do nothing

#
# The targets invoked by bld...
#

MAKMAKE : $(GENERATED_FILES)

BLD : MAKMAKE

SAVESPACE : MAKMAKE

CLEAN : 
	$(call createdir,$(BUILD_DIR))

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

RESOURCE : DO_NOTHING

FINAL : DO_NOTHING

RELEASABLES : 
	@echo $(GENERATED_FILES)
