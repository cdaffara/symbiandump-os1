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
#

# To ensure that EPOCROOT always ends with a forward slash
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

include $(EPOCROOT)epoc32/tools/shell/$(notdir $(basename $(SHELL))).mk

SOURCE_HDR=$(SOURCES)
TARGET_CPP=$(TARGET)
CONVERSION_SCRIPT = $(EPOCROOT)epoc32$/tools$/conv_khronos_hdr_to_cpp.pl

MAKMAKE : $(TARGET_CPP)

FREEZE :

LIB :

CLEANLIB :

RESOURCE :

FINAL :

BLD SAVESPACE : $(TARGET_CPP)

RELEASABLES :

CLEAN :
	perl $(CONVERSION_SCRIPT) $(SOURCE_HDR) $(TARGET_CPP) delete

$(TARGET_CPP): $(SOURCE_HDR)
	perl $(CONVERSION_SCRIPT) $(SOURCE_HDR) $(TARGET_CPP) create
