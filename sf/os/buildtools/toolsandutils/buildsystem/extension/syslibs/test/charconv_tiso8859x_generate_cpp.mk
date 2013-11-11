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

TARGET_DIRECTORY = $(EXTENSION_ROOT)/../test/rtest/tsrc/main

TEXT_FILE_DIRECTORY = $(EXTENSION_ROOT)/../data

TARGET_CPP_FILES = $(TARGET_DIRECTORY)/g_tiso8859x.cpp

TOOLS = $(EXTENSION_ROOT)/../test/rtest/tsrc/main/tiso8859x_generate_cpp.pl

$(TARGET_DIRECTORY) :

$(TARGET_DIRECTORY)/g_tiso8859x.cpp : $(TOOLS) \
	$(TEXT_FILE_DIRECTORY)/iso88592.txt \
	$(TEXT_FILE_DIRECTORY)/iso88593.txt \
	$(TEXT_FILE_DIRECTORY)/iso88594.txt \
	$(TEXT_FILE_DIRECTORY)/iso88595.txt \
	$(TEXT_FILE_DIRECTORY)/iso88596.txt \
	$(TEXT_FILE_DIRECTORY)/iso88597.txt \
	$(TEXT_FILE_DIRECTORY)/iso88598.txt \
	$(TEXT_FILE_DIRECTORY)/iso88599.txt \
	$(TEXT_FILE_DIRECTORY)/iso885910.txt \
	$(TEXT_FILE_DIRECTORY)/iso885913.txt \
	$(TEXT_FILE_DIRECTORY)/iso885914.txt \
	$(TEXT_FILE_DIRECTORY)/iso885915.txt
	@perl -w $(call slash2generic, $(EXTENSION_ROOT)/../test/rtest/tsrc/main/tiso8859x_generate_cpp.pl '$(EXTENSION_ROOT)')

DO_NOTHING :
	@echo do nothing

#
# The targets invoked by bld...
#

MAKMAKE : $(TARGET_DIRECTORY) $(TARGET_CPP_FILES)

BLD : $(TARGET_DIRECTORY) $(TARGET_CPP_FILES)

SAVESPACE : BLD

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING

RESOURCE : DO_NOTHING

CLEAN : 
	-$(ERASE) $(TARGET_CPP_FILES)

RELEASABLES : 
	@echo $(TARGET_CPP_FILES)

FINAL : DO_NOTHING
