# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


BUILD_DIR = $(call generated,generated/fatcharsetconv)
	

SOURCE_DIR = $(EXTENSION_ROOT)/../unicodeTables

TARGET= $(BUILD_DIR)/cp950.cpp \
	$(BUILD_DIR)/cp949.cpp \
	$(BUILD_DIR)/cp936.cpp \
	$(BUILD_DIR)/cp932.cpp \
	$(BUILD_DIR)/cp874.cpp \
	$(BUILD_DIR)/cp1258.cpp \
	$(BUILD_DIR)/cp1257.cpp \
	$(BUILD_DIR)/cp1256.cpp \
	$(BUILD_DIR)/cp1255.cpp \
	$(BUILD_DIR)/cp1254.cpp \
	$(BUILD_DIR)/cp1253.cpp \
	$(BUILD_DIR)/cp1252.cpp \
	$(BUILD_DIR)/cp1251.cpp \
	$(BUILD_DIR)/cp1250.cpp

$(BUILD_DIR):
	$(call createdir,$(BUILD_DIR)) 

$(TARGET):$(BUILD_DIR)/cp%.cpp : $(SOURCE_DIR)/CP%.txt $(BUILD_DIR)
	perl $(EXTENSION_ROOT)/FatConversionTable.pl $< $@


do_nothing:
	@echo do nothing


MAKMAKE : $(TARGET)

BLD : do_nothing

SAVESPACE : do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB: do_nothing

RESOURCE : do_nothing

FINAL : do_nothing

# Do not echo 'do nothing'.
# Do not specify any 'epoc32/build' files, unless they are built into the CBR. 
RELEASABLES :

CLEAN :
	-$(ERASE) $(TARGET)
