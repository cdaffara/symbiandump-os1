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

ZDIR:=$(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/z

ifeq "$(PLATFORM)" "WINSCW"
COPIED_EXE=$(ZDIR)/sys/bin/upsserver.exe
endif


do_nothing:
	

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

BLD : do_nothing

ifdef COPIED_EXE
#
# Rules to create and package winscw Z drive upsserver.exe copy
#
.PHONY: FINAL
FINAL : $(COPIED_EXE)

$(COPIED_EXE) : $(EPOCROOT)epoc32/release/$(PLATFORM_PATH)/$(CFG_PATH)/upsserver.exe
	echo Copying upsserver.exe to emulator Z drive so the UPS romstub works.
	$(CP)  $(call slash2generic,$<) $(call slash2generic,$@)

CLEAN : 
	-$(ERASE)  $(COPIED_EXE) 

RELEASABLES : 
	 echo $(COPIED_EXE)

else

FINAL : do_nothing
CLEAN : do_nothing
RELEASABLES : do_nothing

endif

SAVESPACE : BLD

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing


