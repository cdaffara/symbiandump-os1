# Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk


ifneq ($(FIRST_PASS),completed)

FIRST_PASS=completed

SOURCE_DIRECTORY=$(EXTENSION_ROOT)/../data

TOOLS=\
$(EPOCROOT)epoc32/tools/PARSER.pm \
$(EPOCROOT)epoc32/tools/WRITER.pm \
$(EPOCROOT)epoc32/tools/cnvtool.pl

TARGET_DIRECTORY:=$(call generated,generatedcpp/charconv)
TARGET_DIRECTORY2:=$(EPOCROOT)epoc32/tools/charconv

endif


ifeq ($(TYPE),dat)

TARGET_FILES2=$(patsubst %,$(TARGET_DIRECTORY2)/%.dat,$(STEMS))

$(TARGET_FILES2) : $(TARGET_DIRECTORY2)/%.dat: $(SOURCE_DIRECTORY)/%.txt $(SOURCE_DIRECTORY)/%$(CTL_EXT).ctl $(EXTRA_DEP) $(TOOLS)
	@$(call createdir,"$(TARGET_DIRECTORY2)")
	perl $(EPOCROOT)epoc32/tools/cnvtool.pl $(subst .txt,$(CTL_EXT).ctl,$<) $< $@  -flattenHashAndSave $(foreach PARAM,$(EXTRA_PARAMS),"$(PARAM)" )

else

# This is required by parellel build

TARGET_FILES=$(patsubst %,$(TARGET_DIRECTORY)/g_%.cpp,$(STEMS)) 

$(TARGET_FILES) : $(TARGET_DIRECTORY)/g_%.cpp: $(SOURCE_DIRECTORY)/%.txt $(SOURCE_DIRECTORY)/%$(CTL_EXT).ctl $(EXTRA_DEP) $(TOOLS)
	@$(call createdir,"$(TARGET_DIRECTORY)")
	@$(call createdir,"$(TARGET_DIRECTORY2)")
	perl $(EPOCROOT)epoc32/tools/cnvtool.pl $(subst .txt,$(CTL_EXT).ctl,$<) $< $@ -generateSourceCode $(foreach PARAM,$(EXTRA_PARAMS),"$(PARAM)" )

endif



ifneq ($(TYPE),all)

DO_NOTHING :
# do nothing

  # the targets below are the public ones

MAKMAKE : $(TARGET_FILES2) $(TARGET_FILES)

BLD : $(TARGET_FILES2) $(TARGET_FILES)

SAVESPACE : BLD

FREEZE : DO_NOTHING

LIB : DO_NOTHING

CLEANLIB : DO_NOTHING
	
RESOURCE : DO_NOTHING

CLEAN :
	-$(ERASE) $(TARGET_FILES2) $(TARGET_FILES)

RELEASABLES : DO_NOTHING

FINAL : DO_NOTHING

endif



ifeq ($(TYPE),dat)

RELEASABLES : 
	$(call formatreleasables,$(TARGET_FILES2))

endif



ifeq ($(TYPE),all)

TYPE=dat
TARGET_FILES2=$(TARGET_FILES)

include $(call include)

endif

